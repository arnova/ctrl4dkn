/*
  Ctrl4Dkn - Floor Heating(/Cooling) Controller For Daikin (Hybrid) Heatpump Systems
  Last update: October 19, 2026
  (C) Copyright 2024-2026 by Arno van Amersfoort
  Web                   : https://github.com/arnova/ctrl4dkn
  Email                 : a r n o DOT v a n DOT a m e r s f o o r t AT g m a i l DOT c o m
                          (note: you must remove all spaces and substitute the @ and the . at the proper locations!)
  ----------------------------------------------------------------------------------------------------------------------
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ---------------------------------------------------------------------------------------------------------------------- 
*/

#include <Arduino.h>
#include <EEPROM.h>

#include "Terminal.h"
#include "util.h"
#include "App.h"
#include "system.h"

// Application global instance
CApp g_app;
uint16_t g_iChecksumLast = 0;

void MqttCallback(char* topic, byte *payload, const unsigned int length)
{
#ifdef MQTT_DEBUG
  CTerminal::println("-------new message from broker-----");
  CTerminal::print("topic: ");
  CTerminal::println(topic);
  CTerminal::print("data: ");
  for (unsigned int i = 0; i < length; i++)
  {
    CTerminal::print((char) payload[i]);
  }
  CTerminal::println("");
#endif

  float fVal;
  const bool bValidFloat = bytes_to_float(payload, length, &fVal);

  int32_t iVal;
  const bool bValidInt = bytes_to_int32(payload, length, &iVal);

  if (STRIEQUALS(topic, MQTT_P1P2_P_DATE_TIME))
  {
    uint16_t iChecksum = 0;
    for (unsigned int idx = 0; idx < length; idx++)
    {
      iChecksum += payload[idx];
    }
    if (iChecksum != g_iChecksumLast)
    {
      g_app.GetDaikinCtrl().TriggerWatchdog();
    }
    g_iChecksumLast = iChecksum;

    return;
  }
  
  if (STRIEQUALS(topic, MQTT_P1P2_P_ALTHERMA_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
      {
        g_app.GetDaikinCtrl().SetP1P2AlthermaOn(true);
      }
      else if (iVal == 0)
      {
        g_app.GetDaikinCtrl().SetP1P2AlthermaOn(false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(topic, MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE))
  {
    if (bValidFloat)
    {
      g_app.GetDaikinCtrl().SetP1P2PrimaryZoneRoomTemp(fVal);
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(topic, MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE))
  {
    if (bValidFloat)
    {
      g_app.GetDaikinCtrl().SetP1P2PrimaryZoneTargetTemp(fVal);
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(topic, MQTT_P1P2_P_LEAVING_WATER_TEMP))
  {
    if (bValidFloat)
    {
      g_app.GetDaikinCtrl().SetP1P2LeavingWaterTemp(fVal);
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(topic, MQTT_P1P2_P_VALVE_ZONE_MAIN))
  {
    if (bValidInt)
    {
      if (iVal == 1)
      {
        g_app.GetDaikinCtrl().SetP1P2ValveZoneMain(true);
      }
      else if (iVal == 0)
      {
        g_app.GetDaikinCtrl().SetP1P2ValveZoneMain(false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(topic, MQTT_P1P2_P_DEFROST_ACTIVE))
  {
    if (bValidInt)
    {
      if (iVal == 1)
      {
        g_app.GetDaikinCtrl().SetP1P2DefrostActive(true);
      }
      else if (iVal == 0)
      {
        g_app.GetDaikinCtrl().SetP1P2DefrostActive(false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }


  if (STRIEQUALS(topic, MQTT_P1P2_P_COMPRESSOR))
  {
    if (bValidInt)
    {
      if (iVal == 1)
      {
        g_app.GetDaikinCtrl().SetP1P2Compressor(true);
      }
      else if (iVal == 0)
      {
        g_app.GetDaikinCtrl().SetP1P2Compressor(false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }


  if (STRIEQUALS(topic, MQTT_P1P2_P_CIRCULATION_PUMP_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
      {
        g_app.GetDaikinCtrl().SetP1P2CirculationPumpOn(true);
      }
      else if (iVal == 0)
      {
        g_app.GetDaikinCtrl().SetP1P2CirculationPumpOn(false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(topic, MQTT_P1P2_P_HEATING_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
      {
        g_app.GetDaikinCtrl().SetP1P2HeatingOn(true);
      }
      else if (iVal == 0)
      {
        g_app.GetDaikinCtrl().SetP1P2HeatingOn(false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(topic, MQTT_P1P2_P_COOLING_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
      {
        g_app.GetDaikinCtrl().SetP1P2CoolingOn(true);
      }
      else if (iVal == 0)
      {
        g_app.GetDaikinCtrl().SetP1P2CoolingOn(false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(topic, MQTT_HA_STATUS_TOPIC))
  {
    // Home Assistant (re)started: it lost our (non-retained) states so republish them
    if (length == 6 && strncasecmp((const char*) payload, "online", 6) == 0)
    {
      g_app.MqttRequestRepublish();
    }
    return;
  }

  const size_t iLen = strlen(g_app.GetNetwork().GetHostName());
  if (strncmp(topic, g_app.GetNetwork().GetHostName(), iLen) != 0 ||
      strlen(topic) < iLen + 2)
  {
    return; // Hostname mismatch or length too small
  }

#if 0
  char strVal[32] = { 0 };
  memcpy(strVal, payload, (length < sizeof(strVal)) ? length : sizeof(strVal) - 1);
#endif

  // Get rid of host prefix
  char* subTopic = topic + iLen;

  if (STRIEQUALS(subTopic, "/" MQTT_CONTROLLER_ON_OFF "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlOnOff((iVal == 1 || length == 0) ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_ZONE_PRIMARY_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlZonePriEnable(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_ZONE_SECONDARY_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlZoneSecEnable(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_ZONE_SECONDARY_FORCE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlZoneSecForce(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_VALVE_PRIMARY_CLOSE_FORCE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlValvePriCloseForce(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_DAIKIN_DISABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlDaikinDisable(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_ROOM1_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlRoom1Enable(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_ROOM2_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlRoom2Enable(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_ROOM3_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlRoom3Enable(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_ROOM4_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlRoom4Enable(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_GAS_ONLY "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlGasOnly(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }

  if (STRIEQUALS(subTopic, "/" MQTT_SHORT_CYCLE_PREVENT "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
      {
        g_app.GetDaikinCtrl().SetCtrlShortCyclePrevention(iVal == 1 ? true : false);
      }
      else
      {
        CMqttClient::PrintDataError();
      }
    }
    else
    {
      CMqttClient::PrintDataError();
    }

    return;
  }
}


void setup()
{
  EEPROM.begin(EEPROM_MAX_SIZE); // Reserve room for eeprom settings

  randomSeed(micros());

  // Outputs
#ifdef STATUS_LED
  pinMode(STATUS_LED, OUTPUT);
#endif

#ifdef DAIKIN_PRIMARY_ZONE_RELAY
  pinMode(DAIKIN_PRIMARY_ZONE_RELAY, OUTPUT);
  digitalWrite(DAIKIN_PRIMARY_ZONE_RELAY, LOW);
#endif

#ifdef DAIKIN_SECONDARY_ZONE_RELAY
  pinMode(DAIKIN_SECONDARY_ZONE_RELAY, OUTPUT);
  digitalWrite(DAIKIN_SECONDARY_ZONE_RELAY, LOW);
#endif

  pinMode(PRIMARY_ZONE_VALVE_RELAY, OUTPUT);
  digitalWrite(PRIMARY_ZONE_VALVE_RELAY, PRIMARY_ZONE_VALVE_POLARITY ? HIGH : LOW);
  
#ifdef ROOM1_VALVE_RELAY
  pinMode(ROOM1_VALVE_RELAY, OUTPUT);
  digitalWrite(ROOM1_VALVE_RELAY, ROOM1_VALVE_POLARITY ? LOW : HIGH);
#endif
#ifdef ROOM2_VALVE_RELAY
  pinMode(ROOM2_VALVE_RELAY, OUTPUT);
  digitalWrite(ROOM2_VALVE_RELAY, ROOM2_VALVE_POLARITY ? LOW : HIGH);
#endif
#ifdef ROOM3_VALVE_RELAY
  pinMode(ROOM3_VALVE_RELAY, OUTPUT);
  digitalWrite(ROOM3_VALVE_RELAY, ROOM3_VALVE_POLARITY ? LOW : HIGH);
#endif
#ifdef ROOM4_VALVE_RELAY
  pinMode(ROOM4_VALVE_RELAY, OUTPUT);
  digitalWrite(ROOM4_VALVE_RELAY, ROOM4_VALVE_POLARITY ? LOW : HIGH);
#endif
#ifdef DAIKIN_PREFERENTIAL_RELAY
  pinMode(DAIKIN_PREFERENTIAL_RELAY, OUTPUT);
  digitalWrite(DAIKIN_PREFERENTIAL_RELAY, LOW);
#endif

  // Inputs
#ifdef SECONDARY_ZONE_ENABLE
  pinMode(SECONDARY_ZONE_ENABLE, INPUT_PULLUP);
#endif
#ifdef ROOM1_ENABLE
  pinMode(ROOM1_ENABLE, INPUT_PULLUP);
#endif
#ifdef ROOM2_ENABLE
  pinMode(ROOM2_ENABLE, INPUT_PULLUP);
#endif
#ifdef ROOM3_ENABLE
  pinMode(ROOM3_ENABLE, INPUT_PULLUP);
#endif
#ifdef ROOM4_ENABLE
  pinMode(ROOM4_ENABLE, INPUT_PULLUP);
#endif
#ifdef HARDWARE_MAX_TEMP_SENSOR
  pinMode(HARDWARE_MAX_TEMP_SENSOR, INPUT_PULLUP);
#endif

  TERM_SERIAL.begin(BAUD_RATE);
  TERM_SERIAL.setTimeout(2000);

  // Init g_app instance
  g_app.Init();

  // Setup the MQTT client callback
  g_app.GetNetwork().GetMqttClient().setCallback(MqttCallback);

  // Allow the hardware to sort itself out
  delay(1500);

  // Have the terminal start with a newline
  CTerminal::println("");

  // Print out version info
  CTerminal::println(FPSTR(VER_STR_P));
}


void loop()
{
  g_app.Loop();
}
