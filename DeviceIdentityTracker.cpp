#include <DeviceIdentityTracker.h>
#include <Preferences.h>

// NVS keys, namespace "productdef" (shared with Esp32SecretManager::saveProductDefinition, which
// sets identPend to DEVICE_IDENTITY_REASON_LABEL_CHANGED).
static const char *NS = "productdef";
static const char *KEY_PENDING = "identPend";
static const char *KEY_LAST_SENT = "identSent";

static void copyField(char *dest, size_t size, const String &value) {
  memset(dest, 0, size);
  strncpy(dest, value.c_str(), size - 1);
}

void DeviceIdentityTracker::loadState() {
  if (_loaded) return;
  Preferences prefs;
  prefs.begin(NS, true);
  _pendingReason = prefs.getUChar(KEY_PENDING, 0);
  _lastSent = prefs.getULong(KEY_LAST_SENT, 0);
  prefs.end();
  _loaded = true;
}

void DeviceIdentityTracker::begin(bool realReset) {
  loadState();
  // A label change still waiting to go out is more informative than "reset", keep it.
  if (realReset && _pendingReason == 0) {
    _pendingReason = DEVICE_IDENTITY_REASON_RESET;
    Preferences prefs;
    prefs.begin(NS, false);
    prefs.putUChar(KEY_PENDING, _pendingReason);
    prefs.end();
  }
}

bool DeviceIdentityTracker::isDue(uint32_t nowEpoch) {
  _loaded = false;  // SetProductDefinition may have set identPend since the last check
  loadState();
  if (_pendingReason != 0) return true;
  if (nowEpoch == 0) return false;
  return _lastSent == 0 || nowEpoch < _lastSent || nowEpoch - _lastSent >= DEVICE_IDENTITY_INTERVAL_SEC;
}

DeviceIdentityRecord DeviceIdentityTracker::buildRecord(Esp32SecretManager &secretManager, const uint8_t serialnumber[8], uint32_t firmwareBuild) {
  loadState();
  DeviceIdentityRecord r;
  memcpy(r.serialnumberarray, serialnumber, sizeof(r.serialnumberarray));
  r.firmwareBuild = firmwareBuild;
  r.labelBuild = secretManager.getProductDefinitionBuild();
  r.commissionDate = secretManager.getCommissionDate();
  r.reason = _pendingReason != 0 ? _pendingReason : DEVICE_IDENTITY_REASON_DAILY;
  String name, powerSource, battery, pcbs, firmware;
  secretManager.getProductDefinition(name, powerSource, battery, pcbs, firmware);
  copyField(r.name, sizeof(r.name), name);
  copyField(r.firmware, sizeof(r.firmware), firmware);
  copyField(r.pcbs, sizeof(r.pcbs), pcbs);
  copyField(r.powerSource, sizeof(r.powerSource), powerSource);
  copyField(r.battery, sizeof(r.battery), battery);
  return r;
}

void DeviceIdentityTracker::markSent(uint32_t nowEpoch) {
  _pendingReason = 0;
  _lastSent = nowEpoch;
  Preferences prefs;
  prefs.begin(NS, false);
  prefs.putUChar(KEY_PENDING, 0);
  prefs.putULong(KEY_LAST_SENT, nowEpoch);
  prefs.end();
}
