#include "daikinctrl.h"

CDaikinCtrl::CDaikinCtrl(PubSubClient *MQTTClient)
{
  m_pMQTTClient = MQTTClient;
}

bool CDaikinCtrl::Float2HexStr(const float &fVal, char *strVal)
{
  return (snprintf(strVal, 3, "%02X", (uint8_t)(fVal * 10)) > 0);
}

bool CDaikinCtrl::Float2Str(const float &fVal, char *strVal)
{
  return (snprintf(strVal, 4, "%f", fVal));
}

bool CDaikinCtrl::P1P2SetTargetTemp(const float &fTemp)
{
  char strPayLoad[10] = MQTT_P1P2_W_SET_POINT_CMD;

  // Convert float value to hex string & send P1P2 MQTT command
  if (Float2HexStr(fTemp, strPayLoad + sizeof(MQTT_P1P2_W_SET_POINT_CMD)))
    return m_pMQTTClient->publish(MQTT_P1P2_W_TOPIC, strPayLoad); // FIXME

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
  if (bVal)
    digitalWrite(PRIMARY_ZONE_VALVE_RELAY, PRIMARY_ZONE_VALVE_POLARITY ? LOW : HIGH);
  else
    digitalWrite(PRIMARY_ZONE_VALVE_RELAY, PRIMARY_ZONE_VALVE_POLARITY ? HIGH : LOW);

  if (m_bValveZonePrimaryOpen != bVal)
  {
    m_bValveZonePrimaryOpen = bVal;
    m_bUpdateValveZonePrimaryOpen = true;
  }
}

void CDaikinCtrl::UpdateRoom1ValveOpen(const bool bVal)
{
  if (bVal)
    digitalWrite(ROOM1_VALVE_RELAY, ROOM1_VALVE_POLARITY ? LOW : HIGH);
  else
    digitalWrite(ROOM1_VALVE_RELAY, ROOM1_VALVE_POLARITY ? HIGH : LOW);

  if (m_bRoom1ValveOpen != bVal)
  {
    m_bRoom1ValveOpen = bVal;
    m_bUpdateRoom1ValveOpen = true;
  }
}

void CDaikinCtrl::UpdateRoom2ValveOpen(const bool bVal)
{
  if (bVal)
    digitalWrite(ROOM2_VALVE_RELAY, ROOM2_VALVE_POLARITY ? LOW : HIGH);
  else
    digitalWrite(ROOM2_VALVE_RELAY, ROOM2_VALVE_POLARITY ? HIGH : LOW);

  if (m_bRoom2ValveOpen != bVal)
  {
    m_bRoom2ValveOpen = bVal;
    m_bUpdateRoom2ValveOpen = true;
  }
}

void CDaikinCtrl::UpdateRoom3ValveOpen(const bool bVal)
{
  if (bVal)
    digitalWrite(ROOM3_VALVE_RELAY, ROOM3_VALVE_POLARITY ? LOW : HIGH);
  else
    digitalWrite(ROOM3_VALVE_RELAY, ROOM3_VALVE_POLARITY ? HIGH : LOW);

  if (m_bRoom3ValveOpen != bVal)
  {
    m_bRoom3ValveOpen = bVal;
    m_bUpdateRoom3ValveOpen = true;
  }
}

void CDaikinCtrl::UpdateDaikinZonePrimaryEnable(const bool bVal)
{
  digitalWrite(DAIKIN_PRIMARY_ZONE_RELAY, bVal ? HIGH : LOW);

  if (m_bDaikinZonePrimaryEnable != bVal)
  {
    m_bDaikinZonePrimaryEnable = bVal;
    m_bUpdateDaikinZonePrimaryEnable = true;
  }
}

void CDaikinCtrl::UpdateDaikinZoneSecondaryEnable(const bool bVal)
{
  digitalWrite(DAIKIN_SECONDARY_ZONE_RELAY, bVal ? HIGH : LOW);

  if (m_bDaikinZoneSecondaryEnable != bVal)
  {
    m_bDaikinZoneSecondaryEnable = bVal;
    m_bUpdateDaikinZoneSecondaryEnable = true;
  }
}

bool CDaikinCtrl::MQTTPublishValues()
{
  if (m_bUpdateCtrlEnable)
  {
    m_bUpdateCtrlEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_CONTROLLER_ON_OFF, m_bCtrlEnable ? "1" : "0", true);
  }

  if (m_bUpdateCtrlPriZoneHeating)
  {
    m_bUpdateCtrlPriZoneHeating = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_PRIMARY_ZONE_HEATING, m_bCtrlPriZoneHeating ? "1" : "0", true);
  }

  if (m_bUpdateCtrlSecZoneHeating)
  {
    m_bUpdateCtrlSecZoneHeating = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_SECONDARY_ZONE_HEATING, m_bCtrlSecZoneHeating ? "1" : "0", true);
  }

  if (m_bUpdateCtrlRoom1Enable)
  {
    m_bUpdateCtrlRoom1Enable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM1_ENABLE, m_bCtrlRoom1Enable ? "1" : "0", true);
  }

  if (m_bUpdateCtrlRoom2Enable)
  {
    m_bUpdateCtrlRoom2Enable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM2_ENABLE, m_bCtrlRoom2Enable ? "1" : "0", true);
  }

  if (m_bUpdateCtrlRoom3Enable)
  {
    m_bUpdateCtrlRoom3Enable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM3_ENABLE, m_bCtrlRoom3Enable ? "1" : "0", true);
  }

  if (m_bUpdateZonePrimaryRealSetPoint)
  {
    m_bUpdateZonePrimaryRealSetPoint = false;
    char strTemp[5] = {0};
    Float2Str(m_fZonePrimaryRealSetPoint, strTemp);

    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_ZONE_PRIMARY_REAL_SET_POINT, strTemp, true);
  }

  if (m_bUpdateZonePrimarySavedSetPoint)
  {
    m_bUpdateZonePrimarySavedSetPoint = false;
    char strTemp[5] = {0};
    Float2Str(m_fZonePrimarySavedSetPoint, strTemp);

    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_ZONE_PRIMARY_SAVED_SET_POINT, strTemp, true);
  }

  if (m_bUpdateLeavingWaterTooHigh)
  {
    m_bUpdateLeavingWaterTooHigh = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_LEAVING_WATER_TOO_HIGH, m_bLeavingWaterTooHigh ? "1" : "0", true);
  }

  if (m_bUpdateValveZonePrimaryOpen)
  {
    m_bUpdateValveZonePrimaryOpen = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_VALVE_ZONE_PRIMARY_OPEN, m_bValveZonePrimaryOpen ? "1" : "0", true);
  }

  if (m_bUpdateRoom1ValveOpen)
  {
    m_bUpdateRoom1ValveOpen = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_VALVE_ROOM1_OPEN, m_bRoom1ValveOpen ? "1" : "0", true);
  }

  if (m_bUpdateRoom2ValveOpen)
  {
    m_bUpdateRoom2ValveOpen = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_VALVE_ROOM2_OPEN, m_bRoom2ValveOpen ? "1" : "0", true);
  }

  if (m_bUpdateRoom3ValveOpen)
  {
    m_bUpdateRoom3ValveOpen = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_VALVE_ROOM3_OPEN, m_bRoom3ValveOpen ? "1" : "0", true);
  }

  if (m_bUpdateDaikinZonePrimaryEnable)
  {
    m_bUpdateDaikinZonePrimaryEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_DAIKIN_ZONE_PRIMARY_ENABLE, m_bDaikinZonePrimaryEnable ? "1" : "0", true);
  }

  if (m_bUpdateDaikinZoneSecondaryEnable)
  {
    m_bUpdateDaikinZoneSecondaryEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_DAIKIN_ZONE_SECONDARY_ENABLE, m_bDaikinZoneSecondaryEnable ? "1" : "0", true);
  }

  return true;
}

void CDaikinCtrl::loop()
{
  // Run timed control loop
  if (m_loopTimer > CONTROL_LOOP_TIME * 1000)
  {
    if (m_iFloorProtectionCounter > 0)
    {
      if (--m_iFloorProtectionCounter)
      {
        m_bFloorProtection = false;
      }
    }

    // Check if rooms require heat
    m_bRoom1HeatRq = (m_bCtrlRoom1Enable || !digitalRead(ROOM1_HEATING_ENABLE));
    m_bRoom2HeatRq = (m_bCtrlRoom2Enable || !digitalRead(ROOM2_HEATING_ENABLE));
    m_bRoom3HeatRq = (m_bCtrlRoom3Enable || !digitalRead(ROOM3_HEATING_ENABLE));

    switch (m_iState)
    {
      case STATE_IDLE:
      {
        m_iPrimaryZoneDisableCounter = 0; // For the next round
        m_iState = STATE_WAIT_STATE;
      }
      break;

      case STATE_WAIT_STATE:
      {
        // NOTE: Need to enable secondary zone as soon as the primary zone is at set-point (not + half hyseresis!).
        //       This is due to (possible) modulation else it may take forever before we switch over.
        //       Furthermore we don't want wp shutting on-off-on when switching over from primary to secondary.
        if ((!m_bCtrlPriZoneHeating &&
             (m_fP1P2PrimaryZoneRoomTemp == 0.0f ||
              m_fP1P2PrimaryZoneTargetTemp == 0.0f ||
              m_fP1P2PrimaryZoneRoomTemp >= m_fP1P2PrimaryZoneTargetTemp)) ||
             (m_bCtrlSecZoneForceHeating && (m_bCtrlSecZoneHeating || !digitalRead(SECONDARY_ZONE_ENABLE))))
        {
          // Check if secondary zone requires heating
          if (m_bCtrlSecZoneHeating || !digitalRead(SECONDARY_ZONE_ENABLE))
          {
#ifndef LOW_TEMP_SECONDARY_ZONE
            // When rooms (with floor-heating) are requesting heat, check whether Daikin secondary zone is enabled
            if (m_bRoom1HeatRq || m_bRoom2HeatRq || m_bRoom3HeatRq)
            {
              m_bPrimaryZoneValveClose = true;
              m_bDaikinPrimaryZoneOn = true;

              if (m_bDaikinSecondaryZoneOn)
              {
                // Daikin secondary zone heating is enabled, so disable that first
                m_iSMDelayCounter = DAIKIN_ZONE_SWITCH_TIME;
                m_iState = STATE_DAIKIN_SEC_ZONE_DISABLE;
              }
            }
            else
#endif
            if (!m_bDaikinSecondaryZoneOn)
            {
              // Primary zone should be at target temperature for at least PRIMARY_ZONE_DISABLE_TIME minutes!
              if (++m_iPrimaryZoneDisableCounter >= PRIMARY_ZONE_DISABLE_TIME)
              {
                m_bPrimaryZoneValveClose = true;

                // When we want to enable secondary heating on Daikin, we need to wait
                // until primary zone valve is closed:
                m_iSMDelayCounter = PRIMARY_ZONE_VALVE_DELAY;
                m_iState = STATE_PRIMARY_VALVE_CLOSING; // Wait for primary value to be closed & enable secondary zone on Daikin
              }
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
        else if (m_bCtrlPriZoneHeating ||
                (m_fP1P2PrimaryZoneRoomTemp > 0.0f &&
                 m_fP1P2PrimaryZoneTargetTemp > 0.0f &&
                 m_fP1P2PrimaryZoneRoomTemp < (m_fP1P2PrimaryZoneTargetTemp - (DAIKIN_HYSTERESIS / 2))))
        {
          // FIXME: When modulation is used the above isn't true. We must probably check primary zone valve
          //        In that case we can also lower the hysteresis value :-)
          //        Perhaps there's another P1P2 property we can use to determine primary zone requires heating?
          m_bDaikinPrimaryZoneOn = true;

          if (m_bDaikinSecondaryZoneOn)
          {
            // Daikin secondary zone heating is enabled, so delay enabling/opening (daikin/valve) primary zone
            m_iSMDelayCounter = DAIKIN_ZONE_SWITCH_TIME;
            m_iState = STATE_DAIKIN_SEC_ZONE_DISABLE;
          }
          else
          {
            // Daikin secondary zone heating is not enabled (anymore), so immediately open primary zone
            m_bPrimaryZoneValveClose = false;
            m_iState = STATE_IDLE;
          }
        }
        else
        {
          m_iState = STATE_IDLE;
        }
      }
      break;

      case STATE_PRIMARY_VALVE_CLOSING:
      {
        // Wait for primary zone valve to be closed:
        if (--m_iSMDelayCounter == 0)
        {
          m_iSMDelayCounter = DAIKIN_ZONE_SWITCH_TIME;
          m_bDaikinSecondaryZoneOn = true;
          m_iState = STATE_SECONDARY_ZONE_ON;
        }
      }
      break;

      case STATE_SECONDARY_ZONE_ON:
      {
        if (--m_iSMDelayCounter == 0)
        {
          m_bDaikinPrimaryZoneOn = false;
          m_iState = STATE_IDLE;
        }
      }
      break;

      case STATE_DAIKIN_SEC_ZONE_DISABLE:
      {
        // Wait for primary heating to be enabled (again):
        if (--m_iSMDelayCounter == 0)
        {
          m_bDaikinSecondaryZoneOn = false;
          m_bPrimaryZoneValveClose = false;
          m_iState = STATE_IDLE;
        }
      }
    }

    m_loopTimer = 0;
  }

  // Publish new MQTT values (if any) when timer expires (and connected)
  if (m_MQTTTimer > MQTT_UPDATE_TIME * 1000 && m_pMQTTClient->connected())
  {
    MQTTPublishValues();

    m_MQTTTimer = 0;
  }

  if (m_fP1P2LeavingWaterTemp > LEAVING_WATER_MAX || !digitalRead(HARDWARE_MAX_TEMP_SENSOR))
  {
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
      m_iFloorProtectionCounter = FLOOR_PROTECTION_DELAY;
    }
  }

  if ((!IsHeatingActive() || !m_bCtrlEnable) && !m_bFloorProtection)
  {
    UpdateValveZonePrimaryOpen(PRIMARY_ZONE_VALVE_POLARITY ? false : true); // Idle state
  }
  else
  {
    UpdateValveZonePrimaryOpen(!m_bFloorProtection && !m_bPrimaryZoneValveClose);
  }

  bool bAllowRoomValvesOpen = !m_bFloorProtection;
#ifndef LOW_TEMP_SECONDARY_ZONE
  // When Daikin secondary zone uses high temperature, make sure we don't enable floor heated rooms
  bAllowRoomValvesOpen &= !m_bDaikinSecondaryZoneOn; // FIXME: Delay this from SM?
#endif

  if ((!IsHeatingActive() || !m_bCtrlEnable) && bAllowRoomValvesOpen)
  {
    UpdateRoom1ValveOpen(ROOM1_VALVE_POLARITY ? false : true); // Idle state
  }
  else
  {
    UpdateRoom1ValveOpen(m_bRoom1HeatRq && bAllowRoomValvesOpen);
  }

  if ((!IsHeatingActive() || !m_bCtrlEnable) && bAllowRoomValvesOpen)
  {
    UpdateRoom2ValveOpen(ROOM2_VALVE_POLARITY ? false : true); // Idle state
  }
  else
  {
    UpdateRoom2ValveOpen(m_bRoom2HeatRq && bAllowRoomValvesOpen);
  }

  if ((!IsHeatingActive() || !m_bCtrlEnable) && bAllowRoomValvesOpen)
  {
    UpdateRoom3ValveOpen(ROOM3_VALVE_POLARITY ? false : true); // Idle state
  }
  else
  {
    UpdateRoom3ValveOpen(m_bRoom3HeatRq && bAllowRoomValvesOpen);
  }

  if (m_bDaikinPrimaryZoneOn && m_bCtrlEnable && !m_bFloorProtection)
  {
#if 0
    // FIXME:
    // Restore temperature
    if (m_fP1P2PrimaryZoneTargetTemp != -1 &&
        m_fP1P2PrimaryZoneTargetTempSave != -1 &&
        m_fP1P2PrimaryZoneTargetTemp != m_fP1P2PrimaryZoneTargetTempSave)
    {
      if (UpdateDaikinTargetTemperature(m_fP1P2PrimaryZoneTargetTempSave))
      {
        m_fP1P2PrimaryZoneTargetTemp = m_fP1P2PrimaryZoneTargetTempSave; // FIXME
        m_fP1P2PrimaryZoneTargetTempSave = -1;
        UpdateZonePrimarySavedSetPoint(m_fP1P2PrimaryZoneTargetTempSave);
      }
    }
#endif
    // Enable primary zone heating on Daikin
    UpdateDaikinZonePrimaryEnable(true);
  }
  else
  {
#if 0
    // FIXME:
    if (m_fP1P2PrimaryZoneTargetTemp != -1 && m_lastTempUpdateTimer > MIN_P1P2_WRITE_INTERVAL * 1000 * 60) // FIXME: Need to check last changed value instead
    {
      if (UpdateDaikinTargetTemperature(DAIKIN_PRIMARY_ZONE_OFF_TEMPERATURE))
      {
        m_fP1P2PrimaryZoneTargetTempSave = m_fP1P2PrimaryZoneTargetTemp;
        UpdateZonePrimarySavedSetPoint(m_fP1P2PrimaryZoneTargetTempSave);
      }
    }
#endif
    // Disable primary zone heating on Daikin
    UpdateDaikinZonePrimaryEnable(false);
  }

  if (m_bDaikinSecondaryZoneOn && m_bCtrlEnable)
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
}