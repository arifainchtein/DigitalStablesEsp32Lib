#ifndef SENTBY_H
#define SENTBY_H
#include <Arduino.h>

// The last token of a data line Annabelle sends the Pi (2026-11-11): "sentby:<name>" - the relay
// that resent the packet (sentbyarray: "Paula", "Lan1", "Chinamp"), or "sentby:" when the device
// sent it itself. Keyed, not positional, so a Pi deserializer finds it whatever the line's length;
// an older deserializer just ignores the extra token. Printable characters only, never '#'.
inline void printSentByToken(Print &out, const uint8_t *sentby) {
  out.print("sentby:");
  for (int i = 0; i < 8; i++) {
    uint8_t c = sentby[i];
    if (!c) break;
    if (c > 32 && c < 127 && c != '#') out.print((char)c);
  }
}

// Routes (2026-11-11): every route that delivered the same reading to Annabelle - "direct" and/or
// the relays' names, e.g. "direct,Lan1,Paula". Annabelle collects them for a few seconds before the
// reading is queued for the Pi (she keeps only one copy) and registers a lookup here; the serializers
// print "routes:<list>" after the sentby token. No lookup registered (any other sketch), or a reading
// it no longer remembers = "routes:" with nothing after it.
typedef bool (*RoutesLookupFn)(const uint8_t *serial, long secondsTime, char *out, size_t n);
inline RoutesLookupFn &routesLookupHook() {
  static RoutesLookupFn fn = nullptr;
  return fn;
}
inline void printRoutesToken(Print &out, const uint8_t *serial, long secondsTime) {
  out.print("routes:");
  char buf[48];
  RoutesLookupFn fn = routesLookupHook();
  if (fn && fn(serial, secondsTime, buf, sizeof(buf))) {
    for (size_t i = 0; i < sizeof(buf) && buf[i]; i++)
      if (buf[i] > 32 && buf[i] < 127 && buf[i] != '#') out.print(buf[i]);
  }
}
#endif
