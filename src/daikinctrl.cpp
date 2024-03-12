#include "daikinctrl.h"
#include "system.h"

CDaikinCtrl::CDaikinCtrl(PubSubClient* MQTTClient)
{
  m_pMQTTClient = MQTTClient;
}


bool CDaikinCtrl::Float2HexStr(const float& fVal, char* strVal)
{
  return (snprintf(strVal, 3, "%02X", (uint8_t) (fVal * 10)) > 0);
}


bool CDaikinCtrl::Float2Str(const float& fVal, char* strVal)
{
  return (snprintf(strVal, 4, "%f", fVal));
}

bool CDaikinCtrl::P1P2SetTargetTemp(const float& fTemp)
{
  char strPayLoad[10] = MQTT_P1P2_W_SET_POINT_CMD;

  // Convert float value to hex string & send P1P2 MQTT command
  if (Float2HexStr(fTemp, strPayLoad + sizeof(MQTT_P1P2_W_SET_POINT_CMD)))
    return m_pMQTTClient->publish(MQTT_P1P2_W_TOPIC, strPayLoad); //FIXME

  return false;
}


bool CDaikinCtrl::UpdateDaikinTargetTemperature(float fNewVal)
{
  if (fNewVal == m_fP1P2PrimaryZoneTargetTemp)
    return false;

  if (fNewVal == m_fCtrlPrimaryZoneRealSetPoint)
    return false;

  m_fCtrlPrimaryZoneRealSetPoint = fNewVal;

  // When requested set point is the same as target, we're done
  if (m_fP1P2PrimaryZoneTargetTemp == fNewVal)
    return false;

  // Reset last updated timer
  m_lastTempUpdateTimer = 0;

  P1P2SetTargetTemp(fNewVal);

  return true;
}


void CDaikinCtrl::UpdateZonePrimaryRealSetPoint(const float fSetPoint)
{
  if (m_fZonePrimaryRealSetPoint != fSetPoint)
  {
    m_fZonePrimaryRealSetPoint = fSetPoint;
    m_bUpdateZonePrimaryRealSetPoint = true;
  }
}


void CDaikinCtrl::UpdateZonePrimarySavedSetPoint(const float fSetPoint)
{
  if (m_fZonePrimarySavedSetPoint != fSetPoint)
  {
    m_fZonePrimarySavedSetPoint = fSetPoint;
    m_bUpdateZonePrimarySavedSetPoint = true;
  }
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
  if (m_bValveZonePrimaryOpen != bVal)
  {
    m_bValveZonePrimaryOpen = bVal;
    m_bUpdateValveZonePrimaryOpen = true;
  }
}


void CDaikinCtrl::UpdateValveZoneExtraOpen(const bool bVal)
{
  if (m_bValveZoneExtraOpen != bVal)
  {
    m_bValveZoneExtraOpen = bVal;
    m_bUpdateValveZoneExtraOpen = true;
  }
}


void CDaikinCtrl::UpdateDaikinZonePrimaryEnable(const bool bVal)
{
  if (m_bDaikinZonePrimaryEnable != bVal)
  {
    m_bDaikinZonePrimaryEnable = bVal;
    m_bUpdateDaikinZonePrimaryEnable = true;
  }
}


void CDaikinCtrl::UpdateDaikinZoneSecondaryEnable(const bool bVal)
{
  if (m_bDaikinZoneSecondaryEnable != bVal)
  {
    m_bDaikinZoneSecondaryEnable = bVal;
    m_bUpdateDaikinZoneSecondaryEnable = true;
  }
}


bool CDaikinCtrl::MQTTPublishValues()
{
  if (m_bUpdateCtrlPriZoneHeating)
  {
    m_bUpdateCtrlPriZoneHeating = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_PRIMARY_ZONE_HEATING, m_bCtrlPriZoneHeating ? "1" : "0");
  }

  if (m_bUpdateCtrlSecZoneHeating)
  {
    m_bUpdateCtrlSecZoneHeating = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_SECONDARY_ZONE_HEATING, m_bCtrlSecZoneHeating ? "1" : "0");
  }

  if (m_bUpdateCtrlExtraZoneHeating)
  {
    m_bUpdateCtrlExtraZoneHeating = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_EXTRA_ZONE_HEATING, m_bCtrlExtraZoneHeating ? "1" : "0");
  }

  if (m_bUpdateZonePrimaryRealSetPoint)
  {
    m_bUpdateZonePrimaryRealSetPoint = false;
    char strTemp[5] = { 0 };
    Float2Str(m_fZonePrimaryRealSetPoint, strTemp);

    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_ZONE_PRIMARY_REAL_SET_POINT, strTemp);
  }

  if (m_bUpdateZonePrimarySavedSetPoint)
  {
    m_bUpdateZonePrimarySavedSetPoint = false;
    char strTemp[5] = { 0 };
    Float2Str(m_fZonePrimarySavedSetPoint, strTemp);

    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_ZONE_PRIMARY_SAVED_SET_POINT, strTemp);
  }

  if (m_bUpdateLeavingWaterTooHigh)
  {
    m_bUpdateLeavingWaterTooHigh = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_LEAVING_WATER_TOO_HIGH, m_bLeavingWaterTooHigh ? "1" : "0");
  }

  if (m_bUpdateValveZonePrimaryOpen)
  {
    m_bUpdateValveZonePrimaryOpen = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_VALVE_ZONE_PRIMARY_OPEN, m_bValveZonePrimaryOpen ? "1": "0");
  }

  if (m_bUpdateValveZoneExtraOpen)
  {
    m_bUpdateValveZoneExtraOpen = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_VALVE_ZONE_EXTRA_OPEN, m_bValveZoneExtraOpen ? "1" : "0");
  }

  if (m_bUpdateDaikinZonePrimaryEnable)
  {
    m_bUpdateDaikinZonePrimaryEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_DAIKIN_ZONE_PRIMARY_ENABLE, m_bDaikinZonePrimaryEnable ? "1": "0");
  }

  if (m_bUpdateDaikinZoneSecondaryEnable)
  {
    m_bUpdateDaikinZoneSecondaryEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_DAIKIN_ZONE_SECONDARY_ENABLE, m_bDaikinZoneSecondaryEnable ? "1": "0");
  }

  return true;
}


void CDaikinCtrl::loop()
{
  // Run timed control loop
  if (m_loopTimer > CONTROL_LOOP_TIME * 1000)
  {
    if (m_iPrimaryZoneProtectionCounter > 0)
    {
      if (--m_iPrimaryZoneProtectionCounter)
      {
        m_bPrimaryZoneProtection = false;
      }
    }

    // NOTE: Need to enable secondary zone as soon as the primary zone is at set-point (not + half hyseresis!).
    //       This is due to (possible) modulation else it may take forever before we switch over.
    //       Furthermore we don't want wp shutting on-off-on when switching over from primary to secondary.
    if (m_bP1P2HeatingOn &&
       (m_fP1P2PrimaryZoneRoomTemp > 0.0f &&
        m_fP1P2PrimaryZoneTargetTemp > 0.0f &&
        m_fP1P2PrimaryZoneRoomTemp >= m_fP1P2PrimaryZoneTargetTemp &&
       (m_bCtrlSecZoneHeating || !digitalRead(SECONDARY_ZONE_THERMOSTAT))) || m_bCtrlSecZoneForceHeating)
    {
      // Primary zone should be at target temperature for at least PRIMARY_ZONE_DISABLE_TIME minutes!
      if (!m_bDaikinSecondaryZoneOn && ++m_iPrimaryZoneDisableCounter >= PRIMARY_ZONE_DISABLE_TIME)
      {
        m_bDaikinSecondaryZoneOn = true;

        // Need to wait PRIMARY_ZONE_VALVE_DELAY minutes) before primary zone valve(s) are closed
        if (!m_bSecZoneOnly && ++m_iPrimaryZoneValveCounter >= PRIMARY_ZONE_VALVE_DELAY)
        {
          m_bSecZoneOnly = true;
          m_bDaikinPrimaryZoneOn = false;
        }
      }
    }
    else
    {
      m_iPrimaryZoneDisableCounter = 0;
      m_iPrimaryZoneValveCounter = 0;
      m_bDaikinSecondaryZoneOn = false;

      // FIXME: When modulation is used this isn't true. We must probably check primary zone valve also
      //        In that case we can also lower the hysteresis value :-)
      //        Perhaps there's another P1P2 property we can use to determine primary zone requires heating?
      if (m_fP1P2PrimaryZoneRoomTemp <= (m_fP1P2PrimaryZoneTargetTemp - (DAIKIN_HYSTERESIS / 2)))
      {
        m_bDaikinPrimaryZoneOn = true;
        m_bSecZoneOnly = false;
      }
    }

    m_loopTimer = 0;
  }

  // Publish new MQTT values (if any) when timer expires
  if (m_MQTTTimer > MQTT_UPDATE_TIME * 1000)
  {
    MQTTPublishValues();

    m_MQTTTimer = 0;
  }

#ifdef LEAVING_WATER_MAX
  if (m_fP1P2LeavingWaterTemp > LEAVING_WATER_MAX)
  {
    if (!m_bPrimaryZoneProtection)
    {
      // Leaving water temperature too high, enable protection
      m_bPrimaryZoneProtection = true;

      UpdateLeavingWaterTooHigh(true);
    }
  }
  else
  {
    UpdateLeavingWaterTooHigh(false);

    if (m_bPrimaryZoneProtection &&
        m_fP1P2LeavingWaterTemp < LEAVING_WATER_MAX - LEAVING_WATER_MAX_MARGIN)
    {
      m_iPrimaryZoneProtectionCounter = PRIMARY_ZONE_PROTECTION_DELAY;
    }
  }
#endif

  if (!IsHeatingActive() || m_bSecZoneOnly || m_bPrimaryZoneProtection)
  {
    // Close primary zone valve
    digitalWrite(PRIMARY_ZONE_CLOSE_VALVE_RELAY, HIGH);

    UpdateValveZonePrimaryOpen(false);
  }
  else
  {
    // Open primary zone valve
    digitalWrite(PRIMARY_ZONE_CLOSE_VALVE_RELAY, LOW);

    UpdateValveZonePrimaryOpen(true);
  }

  if (IsHeatingActive() && !m_bPrimaryZoneProtection && (!digitalRead(EXTRA_ZONE_THERMOSTAT) || m_bCtrlExtraZoneHeating))
  {
    // Open extra zone valve
    digitalWrite(EXTRA_ZONE_OPEN_VALVE_RELAY, HIGH);

    UpdateValveZoneExtraOpen(true);
  }
  else
  {
    // Close extra zone valve
    digitalWrite(EXTRA_ZONE_OPEN_VALVE_RELAY, LOW);

    UpdateValveZoneExtraOpen(false);
  }

  if (m_bDaikinPrimaryZoneOn && !m_bPrimaryZoneProtection)
  {
    // Enable primary zone heating relay on Daikin
    digitalWrite(DAIKIN_PRIMARY_ZONE_RELAY, HIGH);

    // Restore temperature
    if (m_fP1P2PrimaryZoneRoomTemp != -1 &&
        m_fP1P2PrimaryZoneRoomTempSave != -1 &&
        m_fP1P2PrimaryZoneRoomTemp != m_fP1P2PrimaryZoneRoomTempSave)
    {
      if (UpdateDaikinTargetTemperature(m_fP1P2PrimaryZoneRoomTempSave))
      {
        m_fP1P2PrimaryZoneRoomTemp = m_fP1P2PrimaryZoneRoomTempSave; //FIXME
        m_fP1P2PrimaryZoneRoomTempSave = -1;
        UpdateZonePrimarySavedSetPoint(m_fP1P2PrimaryZoneRoomTempSave);
      }
    }

    UpdateDaikinZonePrimaryEnable(true);
  }
  else
  {
    // Disable primary zone heating on Daikin
    digitalWrite(DAIKIN_PRIMARY_ZONE_RELAY, LOW);

    if (m_fP1P2PrimaryZoneRoomTemp != -1 && m_lastTempUpdateTimer > MIN_P1P2_WRITE_INTERVAL * 1000 * 60) // FIXME: Need to check last changed value instead
    {
      if (UpdateDaikinTargetTemperature(DAIKIN_PRIMARY_ZONE_OFF_TEMPERATURE))
      {
        m_fP1P2PrimaryZoneRoomTempSave = m_fP1P2PrimaryZoneRoomTemp;
        UpdateZonePrimarySavedSetPoint(m_fP1P2PrimaryZoneRoomTempSave);
      }
    }

    UpdateDaikinZonePrimaryEnable(false);
  }

  if (m_bDaikinSecondaryZoneOn)
  {
    // FIXME: Instead of selecting secondary curve, we can also increase AWT deviation
    digitalWrite(DAIKIN_SECONDARY_ZONE_RELAY, HIGH); // Enable secondary zone heating on Daikin. FIXME: Only when heating active

    UpdateDaikinZoneSecondaryEnable(true);
  }
  else
  {
    digitalWrite(DAIKIN_SECONDARY_ZONE_RELAY, LOW); // Disable secondary zone heating on Daikin

    UpdateDaikinZoneSecondaryEnable(false);
  }
}