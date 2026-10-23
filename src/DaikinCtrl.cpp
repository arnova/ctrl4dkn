#include "DaikinCtrl.h"
#include "util.h"

#include <EEPROM.h>

CDaikinCtrl::CDaikinCtrl(CNetwork& network) : m_network(network), m_mqttClient(network.GetMqttClient())
{
  m_roomTempRollingAverager.SetAvgCount(PRIMARY_ZONE_ROOM_TEMP_AVG_TIME);
}


// Load (and validate) setting from EEPROM, returns default when out of range
static uint16_t LoadUInt16Setting(const int iAddr, const uint16_t iMin, const uint16_t iMax, const uint16_t iDefault)
{
  uint16_t iVal16 = 0;
  EEPROM.get(iAddr, iVal16);

  return (iVal16 < iMin || iVal16 > iMax) ? iDefault : iVal16;
}


// Load (and validate) setting from EEPROM, returns default when out of range
static uint8_t LoadUInt8Setting(const int iAddr, const uint8_t iMin, const uint8_t iMax, const uint8_t iDefault)
{
  uint8_t iVal8 = 0;
  EEPROM.get(iAddr, iVal8);

  return (iVal8 < iMin || iVal8 > iMax) ? iDefault : iVal8;
}


// Load (and validate) setting from EEPROM, returns default when out of range (or NaN)
static float LoadFloatSetting(const int iAddr, const float fMin, const float fMax, const float fDefault, const bool bAllowZero = false)
{
  float fVal = 0.0f;
  EEPROM.get(iAddr, fVal);

  if (bAllowZero && fVal == 0.0f)
    return fVal;

  // NOTE: Written as in-range check, so NaN (eg. unprogrammed EEPROM) also results in default
  return (fVal >= fMin && fVal <= fMax) ? fVal : fDefault;
}


// Store setting to EEPROM (and member) when changed, returns true when changed
template <typename T>
static bool StoreSetting(const int iAddr, T& member, const T val)
{
  if (val == member)
    return false;

  EEPROM.put(iAddr, val);
  EEPROM.commit();
  member = val;

  return true;
}


void CDaikinCtrl::Reset()
{
  m_iNetworkWatchdogCounter = 0;
  m_iNetworkWatchdogRecoveryCounter = 0;

  // Clear short cycle history (also ends any active short cycle recovery)
  m_shortCyclePrimary.Reset();
  m_shortCycleSecondary.Reset();

  m_roomTempRollingAverager.Reset();

  m_bPublishSettings = true;
  m_bMqttRepublish = true;

  LoadSettings();
}


void CDaikinCtrl::LoadSettings()
{
  m_iMqttUpdateInterval = LoadUInt8Setting(EEPROM_MQTT_INTERVAL, MQTT_UPDATE_TIME_MIN, MQTT_UPDATE_TIME_MAX, MQTT_UPDATE_TIME_DEFAULT);

  // NOTE: 0 = disabled is also allowed (below min)
  uint16_t iVal16 = LoadUInt16Setting(EEPROM_NET_WD_TIMEOUT, 0, NETWORK_WATCHDOG_TIMEOUT_MAX, NETWORK_WATCHDOG_TIMEOUT_DEFAULT);
  m_iNetWatchDogTimeout = (iVal16 != 0 && iVal16 < NETWORK_WATCHDOG_TIMEOUT_MIN) ? NETWORK_WATCHDOG_TIMEOUT_DEFAULT : iVal16;

  m_iNetWatchDogRecovery = LoadUInt16Setting(EEPROM_NET_WD_RECOVER, NETWORK_WATCHDOG_RECOVERY_MIN, NETWORK_WATCHDOG_RECOVERY_MAX, NETWORK_WATCHDOG_RECOVERY_DEFAULT);

  m_bLowTempSecondaryZone = LoadUInt8Setting(EEPROM_LOW_TEMP_SEC_ZONE, 0, 1, LOW_TEMP_SECONDARY_ZONE_DEFAULT) == 1;
  m_bShortCyclePrevention = LoadUInt8Setting(EEPROM_SC_PREVENTION, 0, 1, SHORT_CYCLE_PREVENTION_DEFAULT) == 1;

  // Expert settings
  // NOTE: 0 = disabled is also allowed
  m_fLeavingWaterMax = LoadFloatSetting(EEPROM_LWT_MAX, LEAVING_WATER_MAX_MIN, LEAVING_WATER_MAX_MAX, LEAVING_WATER_MAX_DEFAULT, true);

  m_iPrimaryZoneValveDelay = LoadUInt16Setting(EEPROM_VALVE_DELAY, PRIMARY_ZONE_VALVE_DELAY_MIN, PRIMARY_ZONE_VALVE_DELAY_MAX, PRIMARY_ZONE_VALVE_DELAY_DEFAULT);
  m_iFloorProtectionDelay = LoadUInt16Setting(EEPROM_FLOOR_DELAY, FLOOR_PROTECTION_DELAY_MIN, FLOOR_PROTECTION_DELAY_MAX, FLOOR_PROTECTION_DELAY_DEFAULT);
  m_iLwtCoolDownDelay = LoadUInt16Setting(EEPROM_LWT_COOL_DOWN, LWT_COOL_DOWN_DELAY_TIME_MIN, LWT_COOL_DOWN_DELAY_TIME_MAX, LWT_COOL_DOWN_DELAY_TIME_DEFAULT);

  m_iShortCycleSmallWindow = LoadUInt16Setting(EEPROM_SC_SMALL_WINDOW, SHORT_CYCLE_SMALL_WINDOW_MIN, SHORT_CYCLE_SMALL_WINDOW_MAX, SHORT_CYCLE_SMALL_WINDOW_DEFAULT);
  m_iShortCycleSmallWindowCount = LoadUInt8Setting(EEPROM_SC_SMALL_COUNT, SHORT_CYCLE_SMALL_WINDOW_COUNT_MIN, SHORT_CYCLE_SMALL_WINDOW_COUNT_MAX, SHORT_CYCLE_SMALL_WINDOW_COUNT_DEFAULT);
  m_iShortCycleLargeWindow = LoadUInt16Setting(EEPROM_SC_LARGE_WINDOW, SHORT_CYCLE_LARGE_WINDOW_MIN, SHORT_CYCLE_LARGE_WINDOW_MAX, SHORT_CYCLE_LARGE_WINDOW_DEFAULT);
  m_iShortCycleLargeWindowCount = LoadUInt8Setting(EEPROM_SC_LARGE_COUNT, SHORT_CYCLE_LARGE_WINDOW_COUNT_MIN, SHORT_CYCLE_LARGE_WINDOW_COUNT_MAX, SHORT_CYCLE_LARGE_WINDOW_COUNT_DEFAULT);
  m_iShortCycleRecoveryTime = LoadUInt16Setting(EEPROM_SC_RECOVERY, SHORT_CYCLE_RECOVERY_TIME_MIN, SHORT_CYCLE_RECOVERY_TIME_MAX, SHORT_CYCLE_RECOVERY_TIME_DEFAULT);

  ApplyShortCycleConfig();

  m_bPublishSettings = true;
}


void CDaikinCtrl::SetMqttUpdateInterval(const uint8_t iInterval)
{
  if (StoreSetting(EEPROM_MQTT_INTERVAL, m_iMqttUpdateInterval, iInterval))
  {
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetNetWatchDogTimeout(const uint16_t iTime)
{
  if (StoreSetting(EEPROM_NET_WD_TIMEOUT, m_iNetWatchDogTimeout, iTime))
  {
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetNetWatchDogRecovery(const uint16_t iTime)
{
  if (StoreSetting(EEPROM_NET_WD_RECOVER, m_iNetWatchDogRecovery, iTime))
  {
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetLowTempSecondaryZone(const bool bVal)
{
  if (StoreSetting(EEPROM_LOW_TEMP_SEC_ZONE, m_bLowTempSecondaryZone, bVal))
  {
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetShortCyclePrevention(const bool bVal)
{
  if (StoreSetting(EEPROM_SC_PREVENTION, m_bShortCyclePrevention, bVal))
  {
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetLeavingWaterMax(const float fVal)
{
  if (StoreSetting(EEPROM_LWT_MAX, m_fLeavingWaterMax, fVal))
  {
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetPrimaryZoneValveDelay(const uint16_t iTime)
{
  if (StoreSetting(EEPROM_VALVE_DELAY, m_iPrimaryZoneValveDelay, iTime))
  {
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetFloorProtectionDelay(const uint16_t iTime)
{
  if (StoreSetting(EEPROM_FLOOR_DELAY, m_iFloorProtectionDelay, iTime))
  {
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetLwtCoolDownDelay(const uint16_t iTime)
{
  if (StoreSetting(EEPROM_LWT_COOL_DOWN, m_iLwtCoolDownDelay, iTime))
  {
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::ApplyShortCycleConfig()
{
  m_shortCyclePrimary.SetConfig(m_iShortCycleSmallWindow, m_iShortCycleSmallWindowCount, m_iShortCycleLargeWindow, m_iShortCycleLargeWindowCount, m_iShortCycleRecoveryTime);
  m_shortCycleSecondary.SetConfig(m_iShortCycleSmallWindow, m_iShortCycleSmallWindowCount, m_iShortCycleLargeWindow, m_iShortCycleLargeWindowCount, m_iShortCycleRecoveryTime);
}


void CDaikinCtrl::SetShortCycleSmallWindow(const uint16_t iTime)
{
  if (StoreSetting(EEPROM_SC_SMALL_WINDOW, m_iShortCycleSmallWindow, iTime))
  {
    ApplyShortCycleConfig();
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetShortCycleSmallWindowCount(const uint8_t iCount)
{
  if (StoreSetting(EEPROM_SC_SMALL_COUNT, m_iShortCycleSmallWindowCount, iCount))
  {
    ApplyShortCycleConfig();
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetShortCycleLargeWindow(const uint16_t iTime)
{
  if (StoreSetting(EEPROM_SC_LARGE_WINDOW, m_iShortCycleLargeWindow, iTime))
  {
    ApplyShortCycleConfig();
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetShortCycleLargeWindowCount(const uint8_t iCount)
{
  if (StoreSetting(EEPROM_SC_LARGE_COUNT, m_iShortCycleLargeWindowCount, iCount))
  {
    ApplyShortCycleConfig();
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetShortCycleRecoveryTime(const uint16_t iTime)
{
  if (StoreSetting(EEPROM_SC_RECOVERY, m_iShortCycleRecoveryTime, iTime))
  {
    ApplyShortCycleConfig();
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::FactoryReset()
{
  SetMqttUpdateInterval(MQTT_UPDATE_TIME_DEFAULT);
  SetNetWatchDogTimeout(NETWORK_WATCHDOG_TIMEOUT_DEFAULT);
  SetNetWatchDogRecovery(NETWORK_WATCHDOG_RECOVERY_DEFAULT);
  SetLowTempSecondaryZone(LOW_TEMP_SECONDARY_ZONE_DEFAULT);
  SetShortCyclePrevention(SHORT_CYCLE_PREVENTION_DEFAULT);

  SetLeavingWaterMax(LEAVING_WATER_MAX_DEFAULT);
  SetPrimaryZoneValveDelay(PRIMARY_ZONE_VALVE_DELAY_DEFAULT);
  SetFloorProtectionDelay(FLOOR_PROTECTION_DELAY_DEFAULT);
  SetLwtCoolDownDelay(LWT_COOL_DOWN_DELAY_TIME_DEFAULT);
  SetShortCycleSmallWindow(SHORT_CYCLE_SMALL_WINDOW_DEFAULT);
  SetShortCycleSmallWindowCount(SHORT_CYCLE_SMALL_WINDOW_COUNT_DEFAULT);
  SetShortCycleLargeWindow(SHORT_CYCLE_LARGE_WINDOW_DEFAULT);
  SetShortCycleLargeWindowCount(SHORT_CYCLE_LARGE_WINDOW_COUNT_DEFAULT);
  SetShortCycleRecoveryTime(SHORT_CYCLE_RECOVERY_TIME_DEFAULT);

  // Reset controller
  Reset();
}


void CDaikinCtrl::MqttOnConnect()
{
  MqttPublishDiscovery();
  MqttP1P2Subscribe();
}


void CDaikinCtrl::MqttP1P2Subscribe()
{
  // Subscribe to messages from p1p2serial etc.
  m_mqttClient.subscribe(MQTT_P1P2_P_DATE_TIME, 0); // For watchdog
  m_mqttClient.subscribe(MQTT_P1P2_P_ALTHERMA_ON, 0);
  m_mqttClient.subscribe(MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE, 0);
  m_mqttClient.subscribe(MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE, 0);
  m_mqttClient.subscribe(MQTT_P1P2_P_LEAVING_WATER_TEMP, 0);
  m_mqttClient.subscribe(MQTT_P1P2_P_VALVE_ZONE_MAIN, 0);
  m_mqttClient.subscribe(MQTT_P1P2_P_DEFROST_ACTIVE, 0);
  m_mqttClient.subscribe(MQTT_P1P2_P_COMPRESSOR, 0);
  m_mqttClient.subscribe(MQTT_P1P2_P_CIRCULATION_PUMP_ON, 0);
  m_mqttClient.subscribe(MQTT_P1P2_P_HEATING_ON, 0);
  m_mqttClient.subscribe(MQTT_P1P2_P_COOLING_ON, 0);
}


void CDaikinCtrl::MqttPublishDiscovery()
{
  // Publish MQTT config for eg. HA discovery and subscribe to control topics
  m_mqttClient.PublishSwitchConfig(MQTT_CONTROLLER_ON_OFF);
  m_mqttClient.PublishSwitchConfig(MQTT_ZONE_PRIMARY_ENABLE);
  m_mqttClient.PublishSwitchConfig(MQTT_ZONE_SECONDARY_ENABLE);
  m_mqttClient.PublishSwitchConfig(MQTT_ZONE_SECONDARY_FORCE);
  m_mqttClient.PublishSwitchConfig(MQTT_VALVE_PRIMARY_CLOSE_FORCE);
  m_mqttClient.PublishSwitchConfig(MQTT_DAIKIN_DISABLE);

#ifdef DAIKIN_PREFERENTIAL_RELAY
  m_mqttClient.PublishSwitchConfig(MQTT_GAS_ONLY);
#endif

#ifdef ROOM1_VALVE_RELAY
  m_mqttClient.PublishSwitchConfig(MQTT_ROOM1_ENABLE);
  m_mqttClient.PublishBinarySensorConfig(MQTT_VALVE_ROOM1_OPEN);
#endif
#ifdef ROOM2_VALVE_RELAY
  m_mqttClient.PublishSwitchConfig(MQTT_ROOM2_ENABLE);
  m_mqttClient.PublishBinarySensorConfig(MQTT_VALVE_ROOM2_OPEN);
#endif
#ifdef ROOM3_VALVE_RELAY
  m_mqttClient.PublishSwitchConfig(MQTT_ROOM3_ENABLE);
  m_mqttClient.PublishBinarySensorConfig(MQTT_VALVE_ROOM3_OPEN);
#endif
#ifdef ROOM4_VALVE_RELAY
  m_mqttClient.PublishSwitchConfig(MQTT_ROOM4_ENABLE);
  m_mqttClient.PublishBinarySensorConfig(MQTT_VALVE_ROOM4_OPEN);
#endif
  m_mqttClient.PublishBinarySensorConfig(MQTT_VALVE_ZONE_PRIMARY_OPEN);
  m_mqttClient.PublishBinarySensorConfig(MQTT_DAIKIN_PRIMARY_ENABLED);
  m_mqttClient.PublishBinarySensorConfig(MQTT_DAIKIN_SECONDARY_ENABLED);
  m_mqttClient.PublishBinarySensorConfig(MQTT_LEAVING_WATER_TOO_HIGH);
  m_mqttClient.PublishBinarySensorConfig(MQTT_SHORT_CYCLE_PRIMARY_DETECTED);
  m_mqttClient.PublishBinarySensorConfig(MQTT_SHORT_CYCLE_SECONDARY_DETECTED);

  m_mqttClient.PublishSensorConfig(MQTT_AVG_ROOM_TEMPERATURE, "°C", "temperature", "measurement");

  m_mqttClient.PublishSensorConfig(MQTT_UPDATE_INTERVAL, "s", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_NET_WD_TIMEOUT, "s", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_NET_WD_RECOVERY, "s", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_UP_TIME, "", "", "", true);

  m_mqttClient.PublishSensorConfig(MQTT_LWT_MAX, "°C", "temperature", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_FLOOR_PROTECTION_DELAY, "s", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_PRIMARY_ZONE_VALVE_DELAY, "s", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_LWT_COOL_DOWN_DELAY, "s", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_SC_SMALL_WINDOW, "min", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_SC_SMALL_WINDOW_COUNT, "", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_SC_LARGE_WINDOW, "min", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_SC_LARGE_WINDOW_COUNT, "", "", "", true);
  m_mqttClient.PublishSensorConfig(MQTT_SC_RECOVERY_TIME, "min", "", "", true);
}


void CDaikinCtrl::UpdateLeavingWaterTooHigh(const bool bVal)
{
  if (m_bLeavingWaterTooHigh != bVal)
  {
    m_bLeavingWaterTooHigh = bVal;
    m_bUpdateLeavingWaterTooHigh = true;
  }
}


// Write valve relay, taking its polarity into account (1 = normally closed, 0 = normally open)
static void WriteValveRelay(const uint8_t iPin, const bool bPolarity, const bool bOpen)
{
  digitalWrite(iPin, (bOpen == bPolarity) ? HIGH : LOW);
}


void CDaikinCtrl::UpdateValveZonePrimaryOpen(const bool bVal)
{
  WriteValveRelay(PRIMARY_ZONE_VALVE_RELAY, PRIMARY_ZONE_VALVE_POLARITY, bVal);

  if (m_bValveZonePrimaryOpen != bVal)
  {
    m_bValveZonePrimaryOpen = bVal;
    m_bUpdateValveZonePrimaryOpen = true;
  }
}


void CDaikinCtrl::UpdateRoom1ValveOpen(const bool bVal)
{
#ifdef ROOM1_VALVE_RELAY
  WriteValveRelay(ROOM1_VALVE_RELAY, ROOM1_VALVE_POLARITY, bVal);

  if (m_bRoom1ValveOpen != bVal)
  {
    m_bRoom1ValveOpen = bVal;
    m_bUpdateRoom1ValveOpen = true;
  }
#endif
}


void CDaikinCtrl::UpdateRoom2ValveOpen(const bool bVal)
{
#ifdef ROOM2_VALVE_RELAY
  WriteValveRelay(ROOM2_VALVE_RELAY, ROOM2_VALVE_POLARITY, bVal);

  if (m_bRoom2ValveOpen != bVal)
  {
    m_bRoom2ValveOpen = bVal;
    m_bUpdateRoom2ValveOpen = true;
  }
#endif
}


void CDaikinCtrl::UpdateRoom3ValveOpen(const bool bVal)
{
#ifdef ROOM3_VALVE_RELAY
  WriteValveRelay(ROOM3_VALVE_RELAY, ROOM3_VALVE_POLARITY, bVal);

  if (m_bRoom3ValveOpen != bVal)
  {
    m_bRoom3ValveOpen = bVal;
    m_bUpdateRoom3ValveOpen = true;
  }
#endif
}


void CDaikinCtrl::UpdateRoom4ValveOpen(const bool bVal)
{
#ifdef ROOM4_VALVE_RELAY
  WriteValveRelay(ROOM4_VALVE_RELAY, ROOM4_VALVE_POLARITY, bVal);

  if (m_bRoom4ValveOpen != bVal)
  {
    m_bRoom4ValveOpen = bVal;
    m_bUpdateRoom4ValveOpen = true;
  }
#endif
}


void CDaikinCtrl::UpdateDaikinZonePrimaryEnable(const bool bVal)
{
#ifdef DAIKIN_PRIMARY_ZONE_RELAY
  digitalWrite(DAIKIN_PRIMARY_ZONE_RELAY, bVal ? HIGH : LOW);

  if (m_bDaikinZonePrimaryEnable != bVal)
  {
    m_bDaikinZonePrimaryEnable = bVal;
    m_bUpdateDaikinZonePrimaryEnable = true;
  }
#endif
}


void CDaikinCtrl::UpdateDaikinZoneSecondaryEnable(const bool bVal)
{
#ifdef DAIKIN_SECONDARY_ZONE_RELAY
  digitalWrite(DAIKIN_SECONDARY_ZONE_RELAY, bVal ? HIGH : LOW);

  if (m_bDaikinZoneSecondaryEnable != bVal)
  {
    m_bDaikinZoneSecondaryEnable = bVal;
    m_bUpdateDaikinZoneSecondaryEnable = true;
  }
#endif
}


void CDaikinCtrl::UpdateAveragePrimaryZoneRoomTemp(const float fVal)
{
  char strTemperature[sizeof(m_strAveragePrimaryZoneRoomTemp)];
  snprintf(strTemperature, sizeof(strTemperature), "%.1f", fVal);

  if (!STRIEQUALS(strTemperature, m_strAveragePrimaryZoneRoomTemp))
  {
    strcpy(m_strAveragePrimaryZoneRoomTemp, strTemperature);
    m_bUpdateAveragePrimaryZoneRoomTemp = true;
  }
}


void CDaikinCtrl::UpdateZonePrimaryShortCycleDetected(const bool bVal)
{
  if (m_bZonePrimaryShortCycleDetected != bVal)
  {
    m_bZonePrimaryShortCycleDetected = bVal;
    m_bUpdateZonePrimaryShortCycleDetected = true;
  }
}


void CDaikinCtrl::UpdateZoneSecondaryShortCycleDetected(const bool bVal)
{
  if (m_bZoneSecondaryShortCycleDetected != bVal)
  {
    m_bZoneSecondaryShortCycleDetected = bVal;
    m_bUpdateZoneSecondaryShortCycleDetected = true;
  }
}


void CDaikinCtrl::PublishBool(const char* strTopic, bool& bUpdate, const bool bVal)
{
  // Publish when changed (or republish requested)
  if (bUpdate || m_bMqttRepublish)
  {
    bUpdate = false;
    m_mqttClient.PublishMessage(strTopic, bVal ? "1" : "0");
  }
}


void CDaikinCtrl::PublishUInt(const char* strTopic, const uint16_t iVal)
{
  char strBuf[6]; // Enough room for unsigned 16 bit number
  snprintf(strBuf, sizeof(strBuf), "%u", iVal);
  m_mqttClient.PublishMessage(strTopic, strBuf);
}


bool CDaikinCtrl::MqttPublishValues()
{
  if (!m_network.IsMqttConnected())
    return false;

  // Publish new MQTT values (if any) when timer expires (and connected)
  if (m_mqttPublishTimer < m_iMqttUpdateInterval * 1000 && !m_bMqttRepublish)
  {
    return false;
  }

  m_mqttPublishTimer = 0;

  char strBuf[24]; // Enough room for signed/unsigned 32 bit number or our floats with 4 digit precision

  // Publish our f/w version. Note: Static so only published on request
  if (m_bMqttRepublish)
  {
    m_mqttClient.PublishMessage(MQTT_FW_VERSION, MY_VERSION);
  }

  if (m_bPublishSettings || m_bMqttRepublish)
  {
    m_bPublishSettings = false;

    PublishUInt(MQTT_UPDATE_INTERVAL, m_iMqttUpdateInterval);
    PublishUInt(MQTT_NET_WD_TIMEOUT, m_iNetWatchDogTimeout);
    PublishUInt(MQTT_NET_WD_RECOVERY, m_iNetWatchDogRecovery);

    if (m_fLeavingWaterMax == 0.0f)
    {
      strcpy(strBuf, ""); // Disabled: HA shows unknown
    }
    else
    {
      snprintf(strBuf, sizeof(strBuf), "%.1f", m_fLeavingWaterMax);
    }
    m_mqttClient.PublishMessage(MQTT_LWT_MAX, strBuf);

    PublishUInt(MQTT_FLOOR_PROTECTION_DELAY, m_iFloorProtectionDelay);
    PublishUInt(MQTT_PRIMARY_ZONE_VALVE_DELAY, m_iPrimaryZoneValveDelay);
    PublishUInt(MQTT_LWT_COOL_DOWN_DELAY, m_iLwtCoolDownDelay);
    PublishUInt(MQTT_SC_SMALL_WINDOW, m_iShortCycleSmallWindow);
    PublishUInt(MQTT_SC_SMALL_WINDOW_COUNT, m_iShortCycleSmallWindowCount);
    PublishUInt(MQTT_SC_LARGE_WINDOW, m_iShortCycleLargeWindow);
    PublishUInt(MQTT_SC_LARGE_WINDOW_COUNT, m_iShortCycleLargeWindowCount);
    PublishUInt(MQTT_SC_RECOVERY_TIME, m_iShortCycleRecoveryTime);
  }

  PublishBool(MQTT_CONTROLLER_ON_OFF, m_bUpdateCtrlEnable, m_bCtrlEnable);
  PublishBool(MQTT_ZONE_PRIMARY_ENABLE, m_bUpdateCtrlZonePriEnable, m_bCtrlZonePriEnable);
  PublishBool(MQTT_ZONE_SECONDARY_ENABLE, m_bUpdateCtrlZoneSecEnable, m_bCtrlZoneSecEnable);
  PublishBool(MQTT_ZONE_SECONDARY_FORCE, m_bUpdateCtrlZoneSecForce, m_bCtrlZoneSecForce);
  PublishBool(MQTT_VALVE_PRIMARY_CLOSE_FORCE, m_bUpdateCtrlValvePriCloseForce, m_bCtrlValvePriCloseForce);
  PublishBool(MQTT_DAIKIN_DISABLE, m_bUpdateCtrlDaikinDisable, m_bCtrlDaikinDisable);
  PublishBool(MQTT_GAS_ONLY, m_bUpdateCtrlGasOnly, m_bCtrlGasOnly);
  PublishBool(MQTT_ROOM1_ENABLE, m_bUpdateCtrlRoom1Enable, m_bCtrlRoom1Enable);
  PublishBool(MQTT_ROOM2_ENABLE, m_bUpdateCtrlRoom2Enable, m_bCtrlRoom2Enable);
  PublishBool(MQTT_ROOM3_ENABLE, m_bUpdateCtrlRoom3Enable, m_bCtrlRoom3Enable);
  PublishBool(MQTT_ROOM4_ENABLE, m_bUpdateCtrlRoom4Enable, m_bCtrlRoom4Enable);
  PublishBool(MQTT_LEAVING_WATER_TOO_HIGH, m_bUpdateLeavingWaterTooHigh, m_bLeavingWaterTooHigh);
  PublishBool(MQTT_VALVE_ZONE_PRIMARY_OPEN, m_bUpdateValveZonePrimaryOpen, m_bValveZonePrimaryOpen);
  PublishBool(MQTT_VALVE_ROOM1_OPEN, m_bUpdateRoom1ValveOpen, m_bRoom1ValveOpen);
  PublishBool(MQTT_VALVE_ROOM2_OPEN, m_bUpdateRoom2ValveOpen, m_bRoom2ValveOpen);
  PublishBool(MQTT_VALVE_ROOM3_OPEN, m_bUpdateRoom3ValveOpen, m_bRoom3ValveOpen);
  PublishBool(MQTT_VALVE_ROOM4_OPEN, m_bUpdateRoom4ValveOpen, m_bRoom4ValveOpen);
  PublishBool(MQTT_DAIKIN_PRIMARY_ENABLED, m_bUpdateDaikinZonePrimaryEnable, m_bDaikinZonePrimaryEnable);
  PublishBool(MQTT_DAIKIN_SECONDARY_ENABLED, m_bUpdateDaikinZoneSecondaryEnable, m_bDaikinZoneSecondaryEnable);

  if (m_bUpdateAveragePrimaryZoneRoomTemp || m_bMqttRepublish)
  {
    m_bUpdateAveragePrimaryZoneRoomTemp = false;
    m_mqttClient.PublishMessage(MQTT_AVG_ROOM_TEMPERATURE, m_strAveragePrimaryZoneRoomTemp);
  }

  PublishBool(MQTT_SHORT_CYCLE_PRIMARY_DETECTED, m_bUpdateZonePrimaryShortCycleDetected, m_bZonePrimaryShortCycleDetected);
  PublishBool(MQTT_SHORT_CYCLE_SECONDARY_DETECTED, m_bUpdateZoneSecondaryShortCycleDetected, m_bZoneSecondaryShortCycleDetected);

  // Publish uptime. Note: Always published since it changes every interval
  const CUptime::uptime_t upTime = GetUpTime();
  snprintf(strBuf, sizeof(strBuf), "%ud %02u:%02u:%02u", upTime.iDays, upTime.iHours, upTime.iMinutes, upTime.iSeconds);
  m_mqttClient.PublishMessage(MQTT_UP_TIME, strBuf);

  m_bMqttRepublish = false;

  return true;
}


void CDaikinCtrl::ShortCycleHandling()
{
  const bool bHeatpumpOn = m_bP1P2CompressorOn || m_bP1P2DefrostActive;

  if (!m_bAlthermaOn)
  {
    m_shortCyclePrimary.Reset();
    m_shortCycleSecondary.Reset();
  }
  else
  {
    m_shortCycleSecondary.Loop(bHeatpumpOn && m_bDaikinSecondaryZoneOn);
    m_shortCyclePrimary.Loop(bHeatpumpOn && !m_bDaikinSecondaryZoneOn); // When on, assume primary is active
  }

  // Publish to mqtt
  UpdateZonePrimaryShortCycleDetected(m_shortCyclePrimary.IsRecoveryActive());
  UpdateZoneSecondaryShortCycleDetected(m_shortCycleSecondary.IsRecoveryActive());
}


void CDaikinCtrl::Update()
{
  // Update if room valves need to open
  m_bRoom1ValveOpenRq = m_bCtrlRoom1Enable;
#ifdef ROOM1_ENABLE
  m_bRoom1ValveOpenRq |= !digitalRead(ROOM1_ENABLE);
#endif

  m_bRoom2ValveOpenRq = m_bCtrlRoom2Enable;
#ifdef ROOM2_ENABLE
  m_bRoom2ValveOpenRq |= !digitalRead(ROOM2_ENABLE);
#endif

  m_bRoom3ValveOpenRq = m_bCtrlRoom3Enable;
#ifdef ROOM3_ENABLE
  m_bRoom3ValveOpenRq |= !digitalRead(ROOM3_ENABLE);
#endif

  m_bRoom4ValveOpenRq = m_bCtrlRoom4Enable;
#ifdef ROOM4_ENABLE
  m_bRoom4ValveOpenRq |= !digitalRead(ROOM4_ENABLE);
#endif

  const bool bPrimaryZoneEnable = m_bCtrlZonePriEnable && !(m_bShortCyclePrevention && m_bZonePrimaryShortCycleDetected);

  bool bSecondaryZoneEnable = m_bCtrlZoneSecEnable && !(m_bShortCyclePrevention && m_bZoneSecondaryShortCycleDetected);

#ifdef SECONDARY_ZONE_ENABLE
  bSecondaryZoneEnable |= !digitalRead(SECONDARY_ZONE_ENABLE);
#endif

  if (m_fP1P2PrimaryZoneRoomTemp <= 0.0f)
    m_roomTempRollingAverager.RemoveValue();
  else
    m_roomTempRollingAverager.UpdateValue(m_fP1P2PrimaryZoneRoomTemp);

  if (m_roomTempRollingAverager.HasValue())
  {
    UpdateAveragePrimaryZoneRoomTemp(m_roomTempRollingAverager.GetValue());
  }

  // Handle watchdog (timeout 0 = disabled)
  if (m_iNetWatchDogTimeout == 0)
  {
    m_iNetworkWatchdogCounter = 0;
    m_iNetworkWatchdogRecoveryCounter = 0;
  }
  else if (m_iNetworkWatchdogCounter < m_iNetWatchDogTimeout)
  {
    m_iNetworkWatchdogCounter++;
    if (m_iNetworkWatchdogRecoveryCounter > 0)
    {
      m_iNetworkWatchdogRecoveryCounter--;
    }
  }
  else
  {
    m_iNetworkWatchdogRecoveryCounter = m_iNetWatchDogRecovery;
  }

  ShortCycleHandling();

  // When Daikin is switched on, we incorperate a startup time
  if (!m_bP1P2AlthermaOn)
  {
    m_iDaikinStartupCounter = 0;
    m_bAlthermaOn = false;
  }
  else
  {
    // Consider Daikin on after DAIKIN_STARTUP_TIME seconds
    if (m_iDaikinStartupCounter < DAIKIN_STARTUP_TIME)
    {
      m_iDaikinStartupCounter++;
      m_iDaikinActiveOffCounter = 0; // NOTE: Need to keep active-off counter reset
    }
    else
    {
      m_bAlthermaOn = true;
    }
  }

  if (m_bP1P2CirculationPumpOn)
  {
    m_iDaikinActiveOffCounter = 0;
    m_bDaikinActive = true;
  }
  else
  {
    // Consider Daikin inactive after DAIKIN_ACTIVE_OFF_TIME seconds
    if (m_iDaikinActiveOffCounter < DAIKIN_ACTIVE_OFF_TIME)
    {
      m_iDaikinActiveOffCounter++;
    }
    else
    {
      m_bDaikinActive = false;
    }
  }

  if (m_bP1P2CoolingOn)
  {
    m_bPrimaryZoneValveClose = m_bCtrlZoneSecForce || m_bCtrlValvePriCloseForce;// && bSecondaryZoneEnable;
    m_bDaikinPrimaryZoneOn = bPrimaryZoneEnable && !m_bPrimaryZoneValveClose;
    m_bDaikinSecondaryZoneOn = !m_bDaikinPrimaryZoneOn && bSecondaryZoneEnable;
    m_iState = STATE_IDLE;
    return; // Cooling: Bypass statemachine
  }

  if (!m_bP1P2HeatingOn)
  {
    m_bPrimaryZoneValveClose = false;
    m_bDaikinPrimaryZoneOn = false;
    m_bDaikinSecondaryZoneOn = false;
    m_iState = STATE_IDLE;
    return; // Not heating (or cooling): Bypass statemachine
  }

  switch (m_iState)
  {
    case STATE_IDLE:
    {
      m_iState = STATE_WAIT_STATE;
    }
    break;

    case STATE_WAIT_STATE:
    {
      // NOTE: Need to enable secondary zone as soon as the primary zone is at set-point (not + half hysteresis!).
      //       This is due to (possible) modulation else it may take forever before we switch over.
      //       Furthermore we don't want wp shutting on-off-on when switching over from primary to secondary.
      if (!bPrimaryZoneEnable || m_bCtrlZoneSecForce || m_bCtrlValvePriCloseForce)
      {
        // Check if secondary zone requires heating
        if ((bSecondaryZoneEnable || m_bCtrlZoneSecForce || m_bCtrlValvePriCloseForce) && !m_bCtrlDaikinDisable)
        {
          if (m_bLowTempSecondaryZone)
          {
            // Enable (disable secondary zone on Daikin, if requested but primary zone doesn't require heating
            m_bDaikinSecondaryZoneOn = ((!bPrimaryZoneEnable && bSecondaryZoneEnable) || m_bCtrlZoneSecForce);

            if (!m_bPrimaryZoneValveClose)
            {
              m_bPrimaryZoneValveClose = true;

              // We need to wait until primary zone valve is closed
              if (m_bDaikinSecondaryZoneOn)
                m_iState = STATE_PRIMARY_VALVE_DELAY;
            }
            else if (m_bDaikinSecondaryZoneOn)
            {
              m_bDaikinPrimaryZoneOn = false;
            }
          }
          // When rooms (with floor-heating) are requesting heat, check whether Daikin secondary zone is enabled
          else if ((m_bDaikinSecondaryZoneOn && (m_bRoom1ValveOpenRq || m_bRoom2ValveOpenRq || m_bRoom3ValveOpenRq || m_bRoom4ValveOpenRq)))
          {
            m_bPrimaryZoneValveClose = true;

            if (!m_bDaikinPrimaryZoneOn)
            {
              m_bDaikinPrimaryZoneOn = true;

              // Daikin secondary zone heating is enabled, so disable that first
              m_iState = STATE_DAIKIN_ZONE_SWITCH_DELAY;
            }
            else
            {
              m_bDaikinSecondaryZoneOn = false;
            }
          }
          else if (!m_bPrimaryZoneValveClose)
          {
            m_bPrimaryZoneValveClose = true;

            // Check if Daikin secondary will be enabled below, if so wait till primary valve is closed
            if ((!bPrimaryZoneEnable && bSecondaryZoneEnable) || m_bCtrlZoneSecForce)
              m_iState = STATE_PRIMARY_VALVE_DELAY;
          }
          else if ((!bPrimaryZoneEnable && bSecondaryZoneEnable) || m_bCtrlZoneSecForce)
          {
            m_bDaikinSecondaryZoneOn = true;

            m_iState = STATE_DAIKIN_ZONE_SWITCH_DELAY;
          }
          else if (m_bDaikinSecondaryZoneOn)
          {
            m_bDaikinPrimaryZoneOn = false;
          }
        }
        else
        {
          m_bPrimaryZoneValveClose = false;
          m_bDaikinPrimaryZoneOn = false;
          m_bDaikinSecondaryZoneOn = false;
          m_iState = STATE_IDLE;
        }
      }
      else
      {
        // We arrive in this block when we *may* start heating primary zone again
        // The following conditions were met:
        // - Primary zone is requesting heat
        // - Secondary zone only is not enabled and secondary zone heating is disabled
        if (!m_bDaikinPrimaryZoneOn)
        {
          m_bDaikinPrimaryZoneOn = true;
          m_iState = STATE_DAIKIN_ZONE_SWITCH_DELAY;
        }
        else if (m_bDaikinSecondaryZoneOn)
        {
          m_bDaikinSecondaryZoneOn = false;

          // Wait for AWT temperature to cool down before opening primary valve
          // FIXME: Perhaps monitor LWT temperature?
          if (!m_bLowTempSecondaryZone)
            m_iState = STATE_LWT_COOL_DOWN_DELAY;
        }
        else
        {
          m_bPrimaryZoneValveClose = false;
        }
      }
    }
    break;

    case STATE_PRIMARY_VALVE_DELAY:
    {
      m_iSMDelayCounter = m_iPrimaryZoneValveDelay;
      m_iState = STATE_DELAY_WAIT;
    }
    break;

    case STATE_DAIKIN_ZONE_SWITCH_DELAY:
    {
      m_iSMDelayCounter = DAIKIN_ZONE_SWITCH_TIME;
      m_iState = STATE_DELAY_WAIT;
    }
    break;

    case STATE_LWT_COOL_DOWN_DELAY:
    {
      m_iSMDelayCounter = m_iLwtCoolDownDelay;
      m_iState = STATE_DELAY_WAIT;
    }
    break;

    case STATE_DELAY_WAIT:
    {
      // Wait for delay counter to expire
      if (--m_iSMDelayCounter == 0)
      {
        m_iState = STATE_IDLE;
      }
    }
  }
}


void CDaikinCtrl::UpdateRelays()
{
  bool bSafeForFloorHeating = !m_bFloorProtection;

  // When Daikin secondary zone uses high temperature, make sure we don't enable floor heated rooms
  if (!m_bLowTempSecondaryZone)
    bSafeForFloorHeating &= (!m_bDaikinSecondaryZoneOn || m_bP1P2CoolingOn); // FIXME: Delay this from SM?

  if ((!m_bDaikinActive || !m_bCtrlEnable) && bSafeForFloorHeating)
  {
    // Idle states:
    UpdateValveZonePrimaryOpen(PRIMARY_ZONE_VALVE_POLARITY ? false : true);
    UpdateRoom1ValveOpen(ROOM1_VALVE_POLARITY ? false : true);
    UpdateRoom2ValveOpen(ROOM2_VALVE_POLARITY ? false : true);
    UpdateRoom3ValveOpen(ROOM3_VALVE_POLARITY ? false : true);
    UpdateRoom4ValveOpen(ROOM4_VALVE_POLARITY ? false : true);
  }
  else
  {
    UpdateValveZonePrimaryOpen(!m_bPrimaryZoneValveClose && bSafeForFloorHeating);
    UpdateRoom1ValveOpen(m_bRoom1ValveOpenRq && bSafeForFloorHeating);
    UpdateRoom2ValveOpen(m_bRoom2ValveOpenRq && bSafeForFloorHeating);
    UpdateRoom3ValveOpen(m_bRoom3ValveOpenRq && bSafeForFloorHeating);
    UpdateRoom4ValveOpen(m_bRoom4ValveOpenRq && bSafeForFloorHeating);
  }

  const bool bShutdown = m_bFloorProtection && !m_bPrimaryZoneValveClose;
  const bool bDaikinAllowed = m_bCtrlEnable && !m_bCtrlDaikinDisable && !bShutdown && m_iNetworkWatchdogRecoveryCounter == 0;

  // Enable/disable primary zone heating on Daikin
  UpdateDaikinZonePrimaryEnable(m_bDaikinPrimaryZoneOn && bDaikinAllowed);

  // FIXME?: Instead of selecting secondary curve, we can also increase AWT deviation but only when primary zone is active

  // Enable/disable secondary zone heating on Daikin
  UpdateDaikinZoneSecondaryEnable(m_bDaikinSecondaryZoneOn && bDaikinAllowed);

#ifdef DAIKIN_PREFERENTIAL_RELAY
  digitalWrite(DAIKIN_PREFERENTIAL_RELAY, m_bCtrlGasOnly ? HIGH : LOW);
#endif
}


void CDaikinCtrl::Loop()
{
  // Update uptime
  m_upTime.Update();

  // Run timed control loop
  if (m_loopTimer > CONTROL_LOOP_TIME)
  {
    if (m_iFloorProtectionCounter > 0)
    {
      if (--m_iFloorProtectionCounter == 0)
      {
        m_bFloorProtection = false;
      }
    }

    Update();

    m_loopTimer = 0;
  }

  MqttPublishValues();

  // Leaving water max. 0 = disabled (hardware max. temp. sensor (if any) still applies)
  const bool bLeavingWaterMaxEnabled = (m_fLeavingWaterMax != 0.0f);

  bool bLeavingWaterTooHigh = bLeavingWaterMaxEnabled && (m_fP1P2LeavingWaterTemp > m_fLeavingWaterMax);
#ifdef HARDWARE_MAX_TEMP_SENSOR
  bLeavingWaterTooHigh |= !digitalRead(HARDWARE_MAX_TEMP_SENSOR);
#endif
  if (bLeavingWaterTooHigh)
  {
    // Abort any running protection delay
    m_iFloorProtectionCounter = 0;

    if (!m_bFloorProtection)
    {
      // Leaving water temperature too high, enable protection
      m_bFloorProtection = true;

      UpdateLeavingWaterTooHigh(true);
    }
  }
  else
  {
    UpdateLeavingWaterTooHigh(false);

    if (m_bFloorProtection &&
        (!bLeavingWaterMaxEnabled || m_fP1P2LeavingWaterTemp < m_fLeavingWaterMax - LEAVING_WATER_MAX_MARGIN))
    {
      // Start protection delay (once, it's counted down by the timed control loop)
      if (m_iFloorProtectionCounter == 0)
      {
        m_iFloorProtectionCounter = m_iFloorProtectionDelay;
      }
    }
    else
    {
      // Abort any running protection delay (temperature not below margin (anymore))
      m_iFloorProtectionCounter = 0;
    }
  }

  UpdateRelays();
}
