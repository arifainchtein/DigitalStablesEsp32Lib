#ifndef DEVICECOMMANDQUEUE_H
#define DEVICECOMMANDQUEUE_H

#include <stdint.h>
#include <string.h>
#include "DeviceCommandRecord.h"

// Commands waiting to reach a device (2026-10-14), shared by Annabelle and Paula so both deliver
// them the same way. One slot per device; a new request replaces that device's earlier one.
//
// Delivery:
//  - right away (an awake Daffodil listens all the time),
//  - after each pre-sleep data packet (sleepTime > 0): the device listens for a moment after the
//    VitalSigns that follows it, so the send is due DEVICE_COMMAND_AFTER_VS_MS after that VitalSigns,
//    or DEVICE_COMMAND_FALLBACK_MS after the data packet if the VitalSigns is missed,
//  - again after an awake data packet, at most every DEVICE_COMMAND_AWAKE_RESEND_MS,
// until a data packet shows the WiFi in the requested state (done) or DEVICE_COMMAND_TIMEOUT_MS.
//
// No locking and no I/O here: the caller holds its own lock around every call (Annabelle's web
// task reads the status while loop() changes it), and transmits outside that lock - takeDue()
// only fills in serial/command/minutes; the caller stamps senderTime, the code and the checksum
// right before the radio sends it.

#ifndef DEVICE_COMMAND_SLOTS
#define DEVICE_COMMAND_SLOTS 4   // devices with a command in flight or recently done (~32 B each, DRAM)
#endif
#define DEVICE_COMMAND_TIMEOUT_MS (15UL * 60UL * 1000UL)
#define DEVICE_COMMAND_AFTER_VS_MS 100        // give the device time to switch its radio to receive
#define DEVICE_COMMAND_FALLBACK_MS 2200       // data packet + 1 s gap + VitalSigns airtime + margin
#define DEVICE_COMMAND_AWAKE_RESEND_MS 20000UL

enum DeviceCommandState : uint8_t {
  DEVICE_COMMAND_STATE_NONE = 0,
  DEVICE_COMMAND_STATE_PENDING,   // not confirmed yet
  DEVICE_COMMAND_STATE_DONE,      // a data packet showed the requested WiFi state
  DEVICE_COMMAND_STATE_EXPIRED    // gave up after DEVICE_COMMAND_TIMEOUT_MS
};

class DeviceCommandQueue {
 public:
  struct Slot {
    uint8_t serial[8];
    uint8_t command;
    uint8_t minutes;
    uint8_t state;        // DeviceCommandState
    bool due;             // send at dueMs
    uint16_t sends;
    uint32_t createdMs;
    uint32_t lastSentMs;
    uint32_t dueMs;
    uint32_t finishedMs;  // DONE / EXPIRED time
  };

  void request(const uint8_t serial[8], uint8_t command, uint8_t minutes, uint32_t nowMs) {
    Slot *s = slotFor(serial, true);
    memcpy(s->serial, serial, 8);
    s->command = command;
    s->minutes = minutes;
    s->state = DEVICE_COMMAND_STATE_PENDING;
    s->due = true;
    s->dueMs = nowMs;
    s->sends = 0;
    s->createdMs = nowMs;
    s->lastSentMs = 0;
    s->finishedMs = 0;
  }

  // Every data packet that checked out. wifiOn = its wifiStatus != 0, sleeping = sleepTime > 0.
  void noteDataPacket(const uint8_t serial[8], bool wifiOn, bool sleeping, uint32_t nowMs) {
    Slot *s = slotFor(serial, false);
    if (!s || s->state != DEVICE_COMMAND_STATE_PENDING) return;
    if (s->sends > 0 && wifiOn == (s->command == DEVICE_COMMAND_WIFI_ON)) {
      s->state = DEVICE_COMMAND_STATE_DONE;
      s->due = false;
      s->finishedMs = nowMs;
      return;
    }
    if (sleeping) {
      s->due = true;
      s->dueMs = nowMs + DEVICE_COMMAND_FALLBACK_MS;
    } else if (!s->due && nowMs - s->lastSentMs >= DEVICE_COMMAND_AWAKE_RESEND_MS) {
      s->due = true;
      s->dueMs = nowMs;
    }
  }

  // A VitalSigns that checked out: if a send is waiting for this device's listen window, it is now.
  void noteVitalSigns(const uint8_t serial[8], uint32_t nowMs) {
    Slot *s = slotFor(serial, false);
    if (s && s->state == DEVICE_COMMAND_STATE_PENDING && s->due && (int32_t)(s->dueMs - nowMs) > (int32_t)DEVICE_COMMAND_AFTER_VS_MS) {
      s->dueMs = nowMs + DEVICE_COMMAND_AFTER_VS_MS;
    }
  }

  // The next command to transmit now, if any (also expires old ones).
  bool takeDue(uint32_t nowMs, DeviceCommandRecord &out) {
    for (int i = 0; i < DEVICE_COMMAND_SLOTS; i++) {
      Slot &s = slots[i];
      if (s.state != DEVICE_COMMAND_STATE_PENDING) continue;
      if (nowMs - s.createdMs >= DEVICE_COMMAND_TIMEOUT_MS) {
        s.state = DEVICE_COMMAND_STATE_EXPIRED;
        s.due = false;
        s.finishedMs = nowMs;
        continue;
      }
      if (!s.due || (int32_t)(nowMs - s.dueMs) < 0) continue;
      s.due = false;
      s.sends++;
      s.lastSentMs = nowMs;
      out = DeviceCommandRecord();
      memcpy(out.serialnumberarray, s.serial, 8);
      out.command = s.command;
      out.minutes = s.minutes;
      return true;
    }
    return false;
  }

  // Copy of a device's slot (state NONE if it never had a command).
  Slot status(const uint8_t serial[8]) const {
    for (int i = 0; i < DEVICE_COMMAND_SLOTS; i++) {
      if (slots[i].state != DEVICE_COMMAND_STATE_NONE && memcmp(slots[i].serial, serial, 8) == 0) return slots[i];
    }
    Slot none = {};
    return none;
  }

 private:
  Slot slots[DEVICE_COMMAND_SLOTS] = {};

  // The device's slot; with create, else a free one, else the oldest finished one, else the oldest.
  Slot *slotFor(const uint8_t serial[8], bool create) {
    for (int i = 0; i < DEVICE_COMMAND_SLOTS; i++) {
      if (slots[i].state != DEVICE_COMMAND_STATE_NONE && memcmp(slots[i].serial, serial, 8) == 0) return &slots[i];
    }
    if (!create) return nullptr;
    for (int i = 0; i < DEVICE_COMMAND_SLOTS; i++) {
      if (slots[i].state == DEVICE_COMMAND_STATE_NONE) return &slots[i];
    }
    Slot *oldestFinished = nullptr, *oldest = &slots[0];
    for (int i = 0; i < DEVICE_COMMAND_SLOTS; i++) {
      Slot *s = &slots[i];
      if (s->state != DEVICE_COMMAND_STATE_PENDING && (!oldestFinished || s->createdMs < oldestFinished->createdMs)) oldestFinished = s;
      if (s->createdMs < oldest->createdMs) oldest = s;
    }
    return oldestFinished ? oldestFinished : oldest;
  }
};

#endif
