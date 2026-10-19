#include "DaikinCtrl.h"
#include "util.h"

#include <EEPROM.h>

bool CDaikinCtrl::Float2HexStr(const float &fVal, char *strVal)
{
  return (snprintf(strVal, 3, "%02X", (uint8_t)(fVal * 10)) > 0);
}

bool CDaikinCtrl::Float2Str(const float &fVal, char *strVal)
{
  return (snprintf(strVal, 4, "%f", fVal));
}


CDaikinCtrl::CDaikinCtrl(CNetwork& network) : m_network(network)
{
  m_roomTempRollingAverager.SetAvgCount(PRIMARY_ZONE_ROOM_TEMP_AVG_TIME);
}


void CDaikinCtrl::Reset()
{
  m_iNetworkWatchdogCounter = 0;
  m_iNetworkWatchdogRecoveryCounter = 0;

  m_bPublishSettings = true;

  LoadSettings();
}


void CDaikinCtrl::LoadSettings()
{
  uint16_t iVal16 = 0;

  EEPROM.get(EEPROM_MQTT_INTERVAL, m_iMqttUpdateInterval);
  if (m_iMqttUpdateInterval < MQTT_UPDATE_TIME_MIN || m_iMqttUpdateInterval > MQTT_UPDATE_TIME_MAX)
  {
    m_iMqttUpdateInterval = MQTT_UPDATE_TIME_DEFAULT;
  }

  EEPROM.get(EEPROM_NET_WD_TIMEOUT, iVal16);
  if (iVal16 > NETWORK_WATCHDOG_TIMEOUT_MAX)
  {
    iVal16 = NETWORK_WATCHDOG_TIMEOUT_DEFAULT;
  }
  m_iNetWatchDogTimeout = iVal16;

  EEPROM.get(EEPROM_NET_WD_RECOVER, iVal16);
  if (iVal16 > NETWORK_WATCHDOG_RECOVERY_MAX)
  {
    iVal16 = NETWORK_WATCHDOG_RECOVERY_DEFAULT;
  }
  m_iNetWatchDogRecovery = iVal16;

  m_bPublishSettings = true;
}


void CDaikinCtrl::SetMqttUpdateInterval(const uint8_t iInterval)
{
  if (iInterval != m_iMqttUpdateInterval)
  {
    EEPROM.put(EEPROM_MQTT_INTERVAL, iInterval);
    EEPROM.commit();

    m_iMqttUpdateInterval = iInterval;
  
    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetNetWatchDogTimeout(const uint16_t iTime)
{
  if (iTime != m_iNetWatchDogTimeout)
  {
    EEPROM.put(EEPROM_NET_WD_TIMEOUT, iTime);
    EEPROM.commit();

    m_iNetWatchDogTimeout = iTime;

    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::SetNetWatchDogRecovery(const uint16_t iTime)
{
  if (iTime != m_iNetWatchDogRecovery)
  {
    EEPROM.put(EEPROM_NET_WD_RECOVER, iTime);
    EEPROM.commit();

    m_iNetWatchDogRecovery = iTime;

    m_bPublishSettings = true;
  }
}


void CDaikinCtrl::FactoryReset()
{
  SetMqttUpdateInterval(MQTT_UPDATE_TIME_DEFAULT);
  SetNetWatchDogTimeout(NETWORK_WATCHDOG_TIMEOUT_DEFAULT);
  SetNetWatchDogRecovery(NETWORK_WATCHDOG_RECOVERY_DEFAULT);

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
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_DATE_TIME, 0); // For watchdog
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_ALTHERMA_ON, 0);
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE, 0);
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE, 0);
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_LEAVING_WATER_TEMP, 0);
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_VALVE_ZONE_MAIN, 0);
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_DEFROST_ACTIVE, 0);
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_COMPRESSOR, 0);
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_CIRCULATION_PUMP_ON, 0);
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_HEATING_ON, 0);
  m_network.GetMqttClient().subscribe(MQTT_P1P2_P_COOLING_ON, 0);
}


void CDaikinCtrl::MqttPublishDiscovery()
{
  // Publish MQTT config for eg. HA discovery and subscribe to control topics
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_CONTROLLER_ON_OFF);
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_ZONE_PRIMARY_ENABLE);
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_ZONE_SECONDARY_ENABLE);
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_ZONE_SECONDARY_FORCE);
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_VALVE_PRIMARY_CLOSE_FORCE);
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_DAIKIN_DISABLE);

#ifdef DAIKIN_PREFERENTIAL_RELAY
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_GAS_ONLY);
#endif

  m_network.GetMqttClient().PublishSwitchConfig(MQTT_SHORT_CYCLE_PREVENT);

#ifdef ROOM1_VALVE_RELAY
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_ROOM1_ENABLE);
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_VALVE_ROOM1_OPEN);
#endif
#ifdef ROOM2_VALVE_RELAY
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_ROOM2_ENABLE);
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_VALVE_ROOM2_OPEN);
#endif
#ifdef ROOM3_VALVE_RELAY
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_ROOM3_ENABLE);
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_VALVE_ROOM3_OPEN);
#endif
#ifdef ROOM4_VALVE_RELAY
  m_network.GetMqttClient().PublishSwitchConfig(MQTT_ROOM4_ENABLE);
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_VALVE_ROOM4_OPEN);
#endif
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_VALVE_ZONE_PRIMARY_OPEN);
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_DAIKIN_PRIMARY_ENABLED);
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_DAIKIN_SECONDARY_ENABLED);
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_LEAVING_WATER_TOO_HIGH);
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_SHORT_CYCLE_PRIMARY_DETECTED);
  m_network.GetMqttClient().PublishBinarySensorConfig(MQTT_SHORT_CYCLE_SECONDARY_DETECTED);

  m_network.GetMqttClient().PublishSensorConfig(MQTT_AVG_ROOM_TEMPERATURE, "°C", "temperature", "measurement");

  m_network.GetMqttClient().PublishSensorConfig(MQTT_UPDATE_INTERVAL, "s", "", "", true);
  m_network.GetMqttClient().PublishSensorConfig(MQTT_NET_WD_TIMEOUT, "s", "", "", true);
  m_network.GetMqttClient().PublishSensorConfig(MQTT_NET_WD_RECOVERY, "s", "", "", true);
  m_network.GetMqttClient().PublishSensorConfig(MQTT_UP_TIME, "", "", "", true);
}


void CDaikinCtrl::UpdateLeavingWaterTooHigh(const bool bVal)
{
  if (m_bLeavingWaterTooHigh != bVal)
  {
    m_bLeavingWaterTooHigh = bVal;
    m_bUpdateLeavingWaterTooHigh = true;
  }
}


void CDaikinCtrl::UpdateValveZonePrimaryOpen(const bool bVal)
{
  if (bVal)
    digitalWrite(PRIMARY_ZONE_VALVE_RELAY, PRIMARY_ZONE_VALVE_POLARITY ? HIGH : LOW);
  else
    digitalWrite(PRIMARY_ZONE_VALVE_RELAY, PRIMARY_ZONE_VALVE_POLARITY ? LOW : HIGH);

  if (m_bValveZonePrimaryOpen != bVal)
  {
    m_bValveZonePrimaryOpen = bVal;
    m_bUpdateValveZonePrimaryOpen = true;
  }
}


void CDaikinCtrl::UpdateRoom1ValveOpen(const bool bVal)
{
#ifdef ROOM1_VALVE_RELAY
  if (bVal)
    digitalWrite(ROOM1_VALVE_RELAY, ROOM1_VALVE_POLARITY ? HIGH : LOW);
  else
    digitalWrite(ROOM1_VALVE_RELAY, ROOM1_VALVE_POLARITY ? LOW : HIGH);

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
  if (bVal)
    digitalWrite(ROOM2_VALVE_RELAY, ROOM2_VALVE_POLARITY ? HIGH : LOW);
  else
    digitalWrite(ROOM2_VALVE_RELAY, ROOM2_VALVE_POLARITY ? LOW : HIGH);

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
  if (bVal)
    digitalWrite(ROOM3_VALVE_RELAY, ROOM3_VALVE_POLARITY ? HIGH : LOW);
  else
    digitalWrite(ROOM3_VALVE_RELAY, ROOM3_VALVE_POLARITY ? LOW : HIGH);

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
  if (bVal)
    digitalWrite(ROOM4_VALVE_RELAY, ROOM4_VALVE_POLARITY ? HIGH : LOW);
  else
    digitalWrite(ROOM4_VALVE_RELAY, ROOM4_VALVE_POLARITY ? LOW : HIGH);

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
  char strTemperature[5];
  snprintf(strTemperature, 5, "%.1f", fVal);

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
    m_network.GetMqttClient().PublishMessage(MQTT_FW_VERSION, MY_VERSION);
  }

  if (m_bPublishSettings || m_bMqttRepublish)
  {
    m_bPublishSettings = false;

    snprintf(strBuf, sizeof(strBuf), "%u", m_iMqttUpdateInterval);
    m_network.GetMqttClient().PublishMessage(MQTT_UPDATE_INTERVAL, strBuf);

    snprintf(strBuf, sizeof(strBuf), "%u", m_iNetWatchDogTimeout);
    m_network.GetMqttClient().PublishMessage(MQTT_NET_WD_TIMEOUT, strBuf);

    snprintf(strBuf, sizeof(strBuf), "%u", m_iNetWatchDogRecovery);
    m_network.GetMqttClient().PublishMessage(MQTT_NET_WD_RECOVERY, strBuf);
  }

  if (m_bUpdateCtrlEnable || m_bMqttRepublish)
  {
    m_bUpdateCtrlEnable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_CONTROLLER_ON_OFF, m_bCtrlEnable ? "1" : "0");
  }

  if (m_bUpdateCtrlZonePriEnable || m_bMqttRepublish)
  {
    m_bUpdateCtrlZonePriEnable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_ZONE_PRIMARY_ENABLE, m_bCtrlZonePriEnable ? "1" : "0");
  }

  if (m_bUpdateCtrlZoneSecEnable || m_bMqttRepublish)
  {
    m_bUpdateCtrlZoneSecEnable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_ZONE_SECONDARY_ENABLE, m_bCtrlZoneSecEnable ? "1" : "0");
  }

  if (m_bUpdateCtrlZoneSecForce || m_bMqttRepublish)
  {
    m_bUpdateCtrlZoneSecForce = false;
    m_network.GetMqttClient().PublishMessage(MQTT_ZONE_SECONDARY_FORCE, m_bCtrlZoneSecForce ? "1" : "0");
  }

  if (m_bUpdateCtrlValvePriCloseForce || m_bMqttRepublish)
  {
    m_bUpdateCtrlValvePriCloseForce = false;
    m_network.GetMqttClient().PublishMessage(MQTT_VALVE_PRIMARY_CLOSE_FORCE, m_bCtrlValvePriCloseForce ? "1" : "0");
  }

  if (m_bUpdateCtrlDaikinDisable || m_bMqttRepublish)
  {
    m_bUpdateCtrlDaikinDisable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_DAIKIN_DISABLE, m_bCtrlDaikinDisable ? "1" : "0");
  }

  if (m_bUpdateCtrlGasOnly || m_bMqttRepublish)
  {
    m_bUpdateCtrlGasOnly = false;
    m_network.GetMqttClient().PublishMessage(MQTT_GAS_ONLY, m_bCtrlGasOnly ? "1" : "0");
  }

  if (m_bUpdateCtrlShortCyclePrevention || m_bMqttRepublish)
  {
    m_bUpdateCtrlShortCyclePrevention = false;
    m_network.GetMqttClient().PublishMessage(MQTT_SHORT_CYCLE_PREVENT, m_bCtrlShortCyclePrevention ? "1" : "0");
  }

  if (m_bUpdateCtrlRoom1Enable || m_bMqttRepublish)
  {
    m_bUpdateCtrlRoom1Enable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_ROOM1_ENABLE, m_bCtrlRoom1Enable ? "1" : "0");
  }

  if (m_bUpdateCtrlRoom2Enable || m_bMqttRepublish)
  {
    m_bUpdateCtrlRoom2Enable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_ROOM2_ENABLE, m_bCtrlRoom2Enable ? "1" : "0");
  }

  if (m_bUpdateCtrlRoom3Enable || m_bMqttRepublish)
  {
    m_bUpdateCtrlRoom3Enable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_ROOM3_ENABLE, m_bCtrlRoom3Enable ? "1" : "0");
  }

  if (m_bUpdateCtrlRoom4Enable || m_bMqttRepublish)
  {
    m_bUpdateCtrlRoom4Enable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_ROOM4_ENABLE, m_bCtrlRoom4Enable ? "1" : "0");
  }

  if (m_bUpdateLeavingWaterTooHigh || m_bMqttRepublish)
  {
    m_bUpdateLeavingWaterTooHigh = false;
    m_network.GetMqttClient().PublishMessage(MQTT_LEAVING_WATER_TOO_HIGH, m_bLeavingWaterTooHigh ? "1" : "0");
  }

  if (m_bUpdateValveZonePrimaryOpen || m_bMqttRepublish)
  {
    m_bUpdateValveZonePrimaryOpen = false;
    m_network.GetMqttClient().PublishMessage(MQTT_VALVE_ZONE_PRIMARY_OPEN, m_bValveZonePrimaryOpen ? "1" : "0");
  }

  if (m_bUpdateRoom1ValveOpen || m_bMqttRepublish)
  {
    m_bUpdateRoom1ValveOpen = false;
    m_network.GetMqttClient().PublishMessage(MQTT_VALVE_ROOM1_OPEN, m_bRoom1ValveOpen ? "1" : "0");
  }

  if (m_bUpdateRoom2ValveOpen || m_bMqttRepublish)
  {
    m_bUpdateRoom2ValveOpen = false;
    m_network.GetMqttClient().PublishMessage(MQTT_VALVE_ROOM2_OPEN, m_bRoom2ValveOpen ? "1" : "0");
  }

  if (m_bUpdateRoom3ValveOpen || m_bMqttRepublish)
  {
    m_bUpdateRoom3ValveOpen = false;
    m_network.GetMqttClient().PublishMessage(MQTT_VALVE_ROOM3_OPEN, m_bRoom3ValveOpen ? "1" : "0");
  }

  if (m_bUpdateRoom4ValveOpen || m_bMqttRepublish)
  {
    m_bUpdateRoom4ValveOpen = false;
    m_network.GetMqttClient().PublishMessage(MQTT_VALVE_ROOM4_OPEN, m_bRoom4ValveOpen ? "1" : "0");
  }

  if (m_bUpdateDaikinZonePrimaryEnable || m_bMqttRepublish)
  {
    m_bUpdateDaikinZonePrimaryEnable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_DAIKIN_PRIMARY_ENABLED, m_bDaikinZonePrimaryEnable ? "1" : "0");
  }

  if (m_bUpdateDaikinZoneSecondaryEnable || m_bMqttRepublish)
  {
    m_bUpdateDaikinZoneSecondaryEnable = false;
    m_network.GetMqttClient().PublishMessage(MQTT_DAIKIN_SECONDARY_ENABLED, m_bDaikinZoneSecondaryEnable ? "1" : "0");
  }

  if (m_bUpdateAveragePrimaryZoneRoomTemp || m_bMqttRepublish)
  {
    m_bUpdateAveragePrimaryZoneRoomTemp = false;
    m_network.GetMqttClient().PublishMessage(MQTT_AVG_ROOM_TEMPERATURE, m_strAveragePrimaryZoneRoomTemp);
  }

  if (m_bUpdateZonePrimaryShortCycleDetected || m_bMqttRepublish)
  {
    m_bUpdateZonePrimaryShortCycleDetected = false;
    m_network.GetMqttClient().PublishMessage(MQTT_SHORT_CYCLE_PRIMARY_DETECTED, m_bZonePrimaryShortCycleDetected ? "1" : "0");
  }

  if (m_bUpdateZoneSecondaryShortCycleDetected || m_bMqttRepublish)
  {
    m_bUpdateZoneSecondaryShortCycleDetected = false;
    m_network.GetMqttClient().PublishMessage(MQTT_SHORT_CYCLE_SECONDARY_DETECTED, m_bZoneSecondaryShortCycleDetected ? "1" : "0");
  }

  // Publish uptime. Note: Always published since it changes every interval
  const CUptime::uptime_t upTime = GetUpTime();
  snprintf(strBuf, sizeof(strBuf), "%ud %02u:%02u:%02u", upTime.iDays, upTime.iHours, upTime.iMinutes, upTime.iSeconds);
  m_network.GetMqttClient().PublishMessage(MQTT_UP_TIME, strBuf);

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

  const bool bPrimaryZoneEnable = m_bCtrlZonePriEnable && !(m_bCtrlShortCyclePrevention && m_bZonePrimaryShortCycleDetected);

  bool bSecondaryZoneEnable = m_bCtrlZoneSecEnable && !(m_bCtrlShortCyclePrevention && m_bZoneSecondaryShortCycleDetected);

#ifdef SECONDARY_ZONE_ENABLE
  bSecondaryZoneEnable |= !digitalRead(SECONDARY_ZONE_ENABLE);
#endif

  if (m_fP1P2PrimaryZoneRoomTemp <= 0.0f)
    m_roomTempRollingAverager.RemoveValue();
  else
    m_roomTempRollingAverager.UpdateValue(m_fP1P2PrimaryZoneRoomTemp);

  float fAveragePrimaryZoneRoomTemp = 0.0f;
  if (m_roomTempRollingAverager.HasValue())
  {
    fAveragePrimaryZoneRoomTemp = m_roomTempRollingAverager.GetValue();
    UpdateAveragePrimaryZoneRoomTemp(fAveragePrimaryZoneRoomTemp);
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
    else if (!m_bAlthermaOn)
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
    m_bP1P2ValveZoneMainLast = false;
    m_iState = STATE_IDLE;
    return; // Cooling: Bypass statemachine
  }

  if (!m_bP1P2HeatingOn)
  {
    m_bPrimaryZoneValveClose = false;
    m_bDaikinPrimaryZoneOn = false;
    m_bDaikinSecondaryZoneOn = false;
    m_bP1P2ValveZoneMainLast = false;
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
#ifdef LOW_TEMP_SECONDARY_ZONE
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
#else
          // When rooms (with floor-heating) are requesting heat, check whether Daikin secondary zone is enabled
          if ((m_bDaikinSecondaryZoneOn && (m_bRoom1ValveOpenRq || m_bRoom2ValveOpenRq || m_bRoom3ValveOpenRq || m_bRoom4ValveOpenRq)))
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
#endif
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
#ifndef LOW_TEMP_SECONDARY_ZONE
          // Wait for AWT temperature to cool down before opening primary valve
          // FIXME: Perhaps monitor LWT temperature?
          m_iState = STATE_LWT_COOL_DOWN_DELAY;
#endif
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
      m_iSMDelayCounter = PRIMARY_ZONE_VALVE_DELAY;
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
      m_iSMDelayCounter = LWT_COOL_DOWN_DELAY_TIME;
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
#ifndef LOW_TEMP_SECONDARY_ZONE
  // When Daikin secondary zone uses high temperature, make sure we don't enable floor heated rooms
  bSafeForFloorHeating &= (!m_bDaikinSecondaryZoneOn || m_bP1P2CoolingOn); // FIXME: Delay this from SM?
#endif

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
  if (m_bDaikinPrimaryZoneOn && m_bCtrlEnable && !m_bCtrlDaikinDisable && !bShutdown && m_iNetworkWatchdogRecoveryCounter == 0)
  {
    // Enable primary zone heating on Daikin
    UpdateDaikinZonePrimaryEnable(true);
  }
  else
  {
    // Disable primary zone heating on Daikin
    UpdateDaikinZonePrimaryEnable(false);
  }

  if (m_bDaikinSecondaryZoneOn && m_bCtrlEnable && !m_bCtrlDaikinDisable && !bShutdown && m_iNetworkWatchdogRecoveryCounter == 0)
  {
    // FIXME?: Instead of selecting secondary curve, we can also increase AWT deviation but only when primary zone is active

    // Enable secondary zone heating on Daikin
    UpdateDaikinZoneSecondaryEnable(true);
  }
  else
  {
    // Disable secondary zone heating on Daikin
    UpdateDaikinZoneSecondaryEnable(false);
  }

#ifdef DAIKIN_PREFERENTIAL_RELAY
  if (m_bCtrlGasOnly)
  {
    digitalWrite(DAIKIN_PREFERENTIAL_RELAY, HIGH);
  }
  else
  {
    digitalWrite(DAIKIN_PREFERENTIAL_RELAY, LOW);
  }
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

  bool bLeavingWaterTooHigh = (m_fP1P2LeavingWaterTemp > LEAVING_WATER_MAX);
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
        m_fP1P2LeavingWaterTemp < LEAVING_WATER_MAX - LEAVING_WATER_MAX_MARGIN)
    {
      // Start protection delay (once, it's counted down by the timed control loop)
      if (m_iFloorProtectionCounter == 0)
      {
        m_iFloorProtectionCounter = FLOOR_PROTECTION_DELAY;
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
