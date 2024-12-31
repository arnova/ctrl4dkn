#include "daikinctrl.h"
#include "util.h"

CDaikinCtrl::CDaikinCtrl(PubSubClient& MQTTClient)
{
  m_pMQTTClient = &MQTTClient;
  m_roomTempRollingAverager.SetAvgCount(PRIMARY_ZONE_ROOM_TEMP_AVG_TIME);
}

bool CDaikinCtrl::Float2HexStr(const float &fVal, char *strVal)
{
  return (snprintf(strVal, 3, "%02X", (uint8_t)(fVal * 10)) > 0);
}

bool CDaikinCtrl::Float2Str(const float &fVal, char *strVal)
{
  return (snprintf(strVal, 4, "%f", fVal));
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

bool CDaikinCtrl::MQTTPublishValues()
{
  if (m_bUpdateCtrlEnable)
  {
    m_bUpdateCtrlEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_CONTROLLER_ON_OFF, m_bCtrlEnable ? "1" : "0", true);
  }

  if (m_bUpdateCtrlZonePriEnable)
  {
    m_bUpdateCtrlZonePriEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ZONE_PRIMARY_ENABLE, m_bCtrlZonePriEnable ? "1" : "0", true);
  }

  if (m_bUpdateCtrlZoneSecEnable)
  {
    m_bUpdateCtrlZoneSecEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ZONE_SECONDARY_ENABLE, m_bCtrlZoneSecEnable ? "1" : "0", true);
  }

  if (m_bUpdateCtrlZoneSecOnly)
  {
    m_bUpdateCtrlZoneSecOnly = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ZONE_SECONDARY_ONLY, m_bCtrlZoneSecOnly ? "1" : "0", true);
  }

  if (m_bUpdateCtrlValvePriCloseForce)
  {
    m_bUpdateCtrlValvePriCloseForce = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_VALVE_PRIMARY_CLOSE_FORCE, m_bCtrlValvePriCloseForce ? "1" : "0", true);
  }

  if (m_bUpdateCtrlDaikinSecEnable)
  {
    m_bUpdateCtrlDaikinSecEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_DAIKIN_SECONDARY_ENABLE, m_bCtrlDaikinSecEnable ? "1" : "0", true);
  }

  if (m_bUpdateCtrlDaikinSecForce)
  {
    m_bUpdateCtrlDaikinSecForce = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_DAIKIN_SECONDARY_FORCE, m_bCtrlDaikinSecForce ? "1" : "0", true);
  }

  if (m_bUpdateCtrlGasOnly)
  {
    m_bUpdateCtrlGasOnly = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_GAS_ONLY, m_bCtrlGasOnly ? "1" : "0", true);
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

  if (m_bUpdateCtrlRoom4Enable)
  {
    m_bUpdateCtrlRoom4Enable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM4_ENABLE, m_bCtrlRoom4Enable ? "1" : "0", true);
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

  if (m_bUpdateRoom4ValveOpen)
  {
    m_bUpdateRoom4ValveOpen = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_VALVE_ROOM4_OPEN, m_bRoom4ValveOpen ? "1" : "0", true);
  }

  if (m_bUpdateDaikinZonePrimaryEnable)
  {
    m_bUpdateDaikinZonePrimaryEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_DAIKIN_PRIMARY_ENABLED, m_bDaikinZonePrimaryEnable ? "1" : "0", true);
  }

  if (m_bUpdateDaikinZoneSecondaryEnable)
  {
    m_bUpdateDaikinZoneSecondaryEnable = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_DAIKIN_SECONDARY_ENABLED, m_bDaikinZoneSecondaryEnable ? "1" : "0", true);
  }

  if (m_bUpdateAveragePrimaryZoneRoomTemp)
  {
    m_bUpdateAveragePrimaryZoneRoomTemp = false;
    m_pMQTTClient->publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_AVG_ROOM_TEMPERATURE, m_strAveragePrimaryZoneRoomTemp, true);
  }

  return true;
}


void CDaikinCtrl::StateMachine()
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

  bool bSecondaryZoneEnable = m_bCtrlZoneSecEnable;
#ifdef SECONDARY_ZONE_ENABLE
  bSecondaryZoneEnable |= !digitalRead(SECONDARY_ZONE_ENABLE);
#endif

  if (m_bP1P2CoolingOn)
  {
    m_bPrimaryZoneValveClose = m_bCtrlZoneSecOnly || m_bCtrlValvePriCloseForce;// && bSecondaryZoneEnable;
    m_bDaikinPrimaryZoneOn = m_bCtrlZonePriEnable && !m_bPrimaryZoneValveClose;
    m_bDaikinSecondaryZoneOn = !m_bDaikinPrimaryZoneOn && bSecondaryZoneEnable && m_bCtrlDaikinSecEnable;
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

  float fAveragePrimaryZoneRoomTemp;
  if (m_fP1P2PrimaryZoneRoomTemp <= 0.0f)
    fAveragePrimaryZoneRoomTemp = m_roomTempRollingAverager.RemoveValue();
  else
    fAveragePrimaryZoneRoomTemp = m_roomTempRollingAverager.UpdateValue(m_fP1P2PrimaryZoneRoomTemp);

  UpdateAveragePrimaryZoneRoomTemp(fAveragePrimaryZoneRoomTemp);

  // Primary zone requires heating when either room temp < target temp and main valve is enabled from Daikin or when requested via mqtt
  if (!m_roomTempRollingAverager.HasValue() ||
       m_fP1P2PrimaryZoneTargetTemp == 0.0f ||
       m_bCtrlZonePriEnable ||
      ((fAveragePrimaryZoneRoomTemp < m_fP1P2PrimaryZoneTargetTemp - (DAIKIN_HYSTERESIS / 2)) && m_bP1P2ValveZoneMain))
  {
    m_bPrimaryZoneRequiresHeating = true;
  }
  else if (fAveragePrimaryZoneRoomTemp >= m_fP1P2PrimaryZoneTargetTemp || !m_bP1P2ValveZoneMain)
  {
    m_bPrimaryZoneRequiresHeating = false;
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
      if (!m_bPrimaryZoneRequiresHeating || m_bCtrlZoneSecOnly || m_bCtrlValvePriCloseForce)
      {
        // Check if secondary zone requires heating
        if (bSecondaryZoneEnable || m_bCtrlZoneSecOnly || m_bCtrlValvePriCloseForce)
        {
#ifdef LOW_TEMP_SECONDARY_ZONE
          // Enable (disable secondary zone on Daikin, if requested but primary zone doesn't require heating
          m_bDaikinSecondaryZoneOn = m_bCtrlDaikinSecEnable && ((!m_bPrimaryZoneRequiresHeating && bSecondaryZoneEnable) || m_bCtrlZoneSecOnly);

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
          if ((m_bDaikinSecondaryZoneOn && (m_bRoom1ValveOpenRq || m_bRoom2ValveOpenRq || m_bRoom3ValveOpenRq || m_bRoom4ValveOpenRq)) ||
              !m_bCtrlDaikinSecEnable)
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
            if (m_bCtrlDaikinSecEnable && ((!m_bPrimaryZoneRequiresHeating && bSecondaryZoneEnable) || m_bCtrlZoneSecOnly))
              m_iState = STATE_PRIMARY_VALVE_DELAY;
          }
          else if (m_bCtrlDaikinSecEnable && ((!m_bPrimaryZoneRequiresHeating && bSecondaryZoneEnable) || m_bCtrlZoneSecOnly))
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
#ifdef LOW_TEMP_SECONDARY_ZONE
        if (m_bPrimaryZoneValveClose)
        {
          m_bPrimaryZoneValveClose = false;
          if (m_bDaikinSecondaryZoneOn)
            m_iState = STATE_PRIMARY_VALVE_DELAY;
        }
        else if (!m_bDaikinPrimaryZoneOn)
        {
          m_bDaikinPrimaryZoneOn = true;
          if (m_bDaikinSecondaryZoneOn)
            m_iState = STATE_DAIKIN_ZONE_SWITCH_DELAY;
        }
        else
        {
          m_bDaikinSecondaryZoneOn = false;
        }
#else
        if (!m_bDaikinPrimaryZoneOn)
        {
          m_bDaikinPrimaryZoneOn = true;
          m_iState = STATE_DAIKIN_ZONE_SWITCH_DELAY;
        }
        else if (m_bDaikinSecondaryZoneOn)
        {
          m_bDaikinSecondaryZoneOn = false;
          // FIXME: We should check AWT temperature and wait with opening primary valve until it's safe
        }
        else if (m_bPrimaryZoneValveClose)
        {
          m_bPrimaryZoneValveClose = false;
        }
#endif
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

  if ((!IsDaikinActive() || !m_bCtrlEnable) && bSafeForFloorHeating)
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
  if (m_bDaikinPrimaryZoneOn && m_bCtrlEnable && !bShutdown)
  {
    // Enable primary zone heating on Daikin
    UpdateDaikinZonePrimaryEnable(true);
  }
  else
  {
    // Disable primary zone heating on Daikin
    UpdateDaikinZonePrimaryEnable(false);
  }

  if ((m_bDaikinSecondaryZoneOn || m_bCtrlDaikinSecForce) && m_bCtrlEnable && !bShutdown)
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

    StateMachine();

    m_loopTimer = 0;
  }

  // Publish new MQTT values (if any) when timer expires (and connected)
  if (m_MQTTTimer > MQTT_UPDATE_TIME * 1000 && m_pMQTTClient->connected())
  {
    MQTTPublishValues();

    m_MQTTTimer = 0;
  }

  bool bLeavingWaterTooHigh = (m_fP1P2LeavingWaterTemp > LEAVING_WATER_MAX);
#ifdef HARDWARE_MAX_TEMP_SENSOR
  bLeavingWaterTooHigh |= !digitalRead(HARDWARE_MAX_TEMP_SENSOR);
#endif
  if (bLeavingWaterTooHigh)
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

  UpdateRelays();
}