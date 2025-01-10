/*
  Ctrl4Dkn - Floor Heating(/Cooling) Controller For Daikin (Hybrid) Heatpump Systems
  Last update: January 10, 2025
  (C) Copyright 2024-2025 by Arno van Amersfoort
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
#include <WiFi.h>
//#include <WiFiSTA.h>
#include <ESPmDNS.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include "daikinctrl.h"
#include "util.h"
#include "system.h"

// Version string:
#define MY_VERSION "0.39"

// Globals
WiFiClient g_wifiClient;
PubSubClient g_MQTTClient(g_wifiClient);
CDaikinCtrl g_DaikinCtrl(g_MQTTClient);

void MQTTPrintError(void)
{
  Serial.println("ERROR: Invalid MQTT data for topic");
}


void MQTTCallback(char* topic, byte *payload, const unsigned int length)
{
  Serial.println("-------new message from broker-----");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("data: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  float fVal;
  const bool bValidFloat = BytesToFloat(payload, length, fVal);

  int8_t iVal;
  const bool bValidInt = BytesToInt8(payload, length, iVal);

  if (STRIEQUALS(topic, MQTT_P1P2_P_ALTHERMA_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
        g_DaikinCtrl.SetP1P2AlthermaOn(true);
      else if (iVal == 0)
        g_DaikinCtrl.SetP1P2AlthermaOn(false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE))
  {
    if (bValidFloat)
      g_DaikinCtrl.SetP1P2PrimaryZoneRoomTemp(fVal);
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE))
  {
    if (bValidFloat)
      g_DaikinCtrl.SetP1P2PrimaryZoneTargetTemp(fVal);
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_P1P2_P_LEAVING_WATER_TEMP))
  {
    if (bValidFloat)
      g_DaikinCtrl.SetP1P2LeavingWaterTemp(fVal);
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_P1P2_P_VALVE_ZONE_MAIN))
  {
    if (bValidInt)
    {
      if (iVal == 1)
        g_DaikinCtrl.SetP1P2ValveZoneMain(true);
      else if (iVal == 0)
        g_DaikinCtrl.SetP1P2ValveZoneMain(false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_P1P2_P_CIRCULATION_PUMP_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
        g_DaikinCtrl.SetP1P2CirculationPumpOn(true);
      else if (iVal == 0)
        g_DaikinCtrl.SetP1P2CirculationPumpOn(false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_P1P2_P_HEATING_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
        g_DaikinCtrl.SetP1P2HeatingOn(true);
      else if (iVal == 0)
        g_DaikinCtrl.SetP1P2HeatingOn(false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_P1P2_P_COOLING_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
        g_DaikinCtrl.SetP1P2CoolingOn(true);
      else if (iVal == 0)
        g_DaikinCtrl.SetP1P2CoolingOn(false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }

  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_CONTROLLER_ON_OFF "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlOnOff((iVal == 1 || length == 0) ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ZONE_PRIMARY_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlZonePriEnable(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ZONE_SECONDARY_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlZoneSecEnable(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ZONE_SECONDARY_FORCE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlZoneSecForce(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_VALVE_PRIMARY_CLOSE_FORCE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlValvePriCloseForce(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_DAIKIN_SECONDARY_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlDaikinSecEnable(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_DAIKIN_SECONDARY_FORCE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlDaikinSecForce(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM1_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlRoom1Enable(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM2_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlRoom2Enable(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM3_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlRoom3Enable(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM4_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlRoom4Enable(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_GAS_ONLY "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlGasOnly(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
}


void MQTTPublishConfig(const char* strItem, CDaikinCtrl::HAConfigType_t HAConfigType)
{
  JsonDocument root;
  root["name"] = strItem;
  root["unique_id"] = String("Ctrl4Dkn_" MQTT_CTRL4DKN_HOST_ID "_") + strItem; // Optional
  root["retain"] = true;
  root["qos"] = 1;

//  root["value_template"] = "{{ value_json.state }}"; // Not used

  switch(HAConfigType)
  {
    case CDaikinCtrl::SWITCH:
    {
      root["state_topic"] = String(MQTT_CTRL4DKN_CTRL_PREFIX) + strItem;
      root["command_topic"] = String(MQTT_CTRL4DKN_CTRL_PREFIX) + strItem + "/set";
      root["payload_on"] = "1";
      root["payload_off"] = "0";
      root["state_on"] = "1";
      root["state_off"] = "0";
    }
    break;

    case CDaikinCtrl::BINARY_SENSOR:
    {
      root["state_topic"] = String(MQTT_CTRL4DKN_STATUS_PREFIX) + strItem;
      root["payload_on"] = "1";
      root["payload_off"] = "0";
    }
    break;

    case CDaikinCtrl::TEMP_SENSOR:
    {
      root["state_topic"] = String(MQTT_CTRL4DKN_STATUS_PREFIX) + strItem;
      root["unit_of_meas"] = "°C";
      root["dev_cla"] = "temperature";
    }
    break;
  }

  JsonObject device = root["device"].to<JsonObject>();
  device["name"] = "Ctrl4Dkn";
  device["model"] = "Daikin Floorheating Controller";
  device["manufacturer"] = "Arnova";
  device["identifiers"] = "Ctrl4Dkn_" MQTT_CTRL4DKN_HOST_ID;

  // Output to console
  serializeJsonPretty(root, Serial);
  Serial.println();

  // Serialize JSON for MQTT
  char message[MQTT_MAX_SIZE];
  serializeJson(root, message);
  Serial.println(message); //Prints it out on one line.

  String strTopic = String("homeassistant/");
  switch (HAConfigType)
  {
    case CDaikinCtrl::SWITCH:
    {
       strTopic += "switch";
    }
    break;

    case CDaikinCtrl::TEMP_SENSOR:
    {
      strTopic += "sensor";
    }
    break;

    case CDaikinCtrl::BINARY_SENSOR:
    {
      strTopic += "binary_sensor";
    }
    break;
  }
  strTopic += String("/ctrl4dkn_" MQTT_CTRL4DKN_HOST_ID "/") + strItem + "/config";
  strTopic.toLowerCase();
  strTopic.replace(' ', '_');

  g_MQTTClient.publish(strTopic.c_str(), message, true);
}


bool MQTTReconnect()
{
  Serial.print("Attempting MQTT connection...");
  // Create a random client ID
  String clientId = "ESP32But-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
//    if (MQTTClient.connect(clientId.c_str(), NULL, NULL, "test", 0, false, "not connected", false))
  if (!g_MQTTClient.connect(clientId.c_str()))
  {
    Serial.print("failed, rc=");
    Serial.print(g_MQTTClient.state());
    return false;
  }

  Serial.println("connected");

  // Subscribe to messages from p1p2serial etc.
  g_MQTTClient.subscribe(MQTT_P1P2_P_ALTHERMA_ON, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_LEAVING_WATER_TEMP, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_VALVE_ZONE_MAIN, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_CIRCULATION_PUMP_ON, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_HEATING_ON, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_COOLING_ON, 0);

  // Publish MQTT config for eg. HA discovery and subscribe to control topics
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_CONTROLLER_ON_OFF "/set", 1);
  MQTTPublishConfig(MQTT_CONTROLLER_ON_OFF, CDaikinCtrl::SWITCH);

  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ZONE_PRIMARY_ENABLE "/set", 1);
  MQTTPublishConfig(MQTT_ZONE_PRIMARY_ENABLE, CDaikinCtrl::SWITCH);

  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ZONE_SECONDARY_ENABLE "/set", 1);
  MQTTPublishConfig(MQTT_ZONE_SECONDARY_ENABLE, CDaikinCtrl::SWITCH);

  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ZONE_SECONDARY_FORCE "/set", 1);
  MQTTPublishConfig(MQTT_ZONE_SECONDARY_FORCE, CDaikinCtrl::SWITCH);

  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_VALVE_PRIMARY_CLOSE_FORCE "/set", 1);
  MQTTPublishConfig(MQTT_VALVE_PRIMARY_CLOSE_FORCE, CDaikinCtrl::SWITCH);

  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_DAIKIN_SECONDARY_ENABLE "/set", 1);
  MQTTPublishConfig(MQTT_DAIKIN_SECONDARY_ENABLE, CDaikinCtrl::SWITCH);

  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_DAIKIN_SECONDARY_FORCE "/set", 1);
  MQTTPublishConfig(MQTT_DAIKIN_SECONDARY_FORCE, CDaikinCtrl::SWITCH);

#ifdef DAIKIN_PREFERENTIAL_RELAY
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_GAS_ONLY "/set", 1);
  MQTTPublishConfig(MQTT_GAS_ONLY, CDaikinCtrl::SWITCH);
#endif

#ifdef ROOM1_VALVE_RELAY
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM1_ENABLE "/set", 1);
  MQTTPublishConfig(MQTT_ROOM1_ENABLE, CDaikinCtrl::SWITCH);
  MQTTPublishConfig(MQTT_VALVE_ROOM1_OPEN, CDaikinCtrl::BINARY_SENSOR);
#endif
#ifdef ROOM2_VALVE_RELAY
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM2_ENABLE "/set", 1);
  MQTTPublishConfig(MQTT_ROOM2_ENABLE, CDaikinCtrl::SWITCH);
  MQTTPublishConfig(MQTT_VALVE_ROOM2_OPEN, CDaikinCtrl::BINARY_SENSOR);
#endif
#ifdef ROOM3_VALVE_RELAY
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM3_ENABLE "/set", 1);
  MQTTPublishConfig(MQTT_ROOM3_ENABLE, CDaikinCtrl::SWITCH);
  MQTTPublishConfig(MQTT_VALVE_ROOM3_OPEN, CDaikinCtrl::BINARY_SENSOR);
#endif
#ifdef ROOM4_VALVE_RELAY
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM4_ENABLE "/set", 1);
  MQTTPublishConfig(MQTT_ROOM4_ENABLE, CDaikinCtrl::SWITCH);
  MQTTPublishConfig(MQTT_VALVE_ROOM4_OPEN, CDaikinCtrl::BINARY_SENSOR);
#endif
  MQTTPublishConfig(MQTT_VALVE_ZONE_PRIMARY_OPEN, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_DAIKIN_PRIMARY_ENABLED, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_DAIKIN_SECONDARY_ENABLED, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_LEAVING_WATER_TOO_HIGH, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_AVG_ROOM_TEMPERATURE, CDaikinCtrl::TEMP_SENSOR);
  MQTTPublishConfig(MQTT_ZONE_PRIMARY_REQUIRES_HEATING, CDaikinCtrl::BINARY_SENSOR);

  // Publish our f/w version
  g_MQTTClient.publish(MQTT_CTRL4DKN_STATUS_PREFIX MQTT_FW_VERSION, MY_VERSION, true);

  return true;
}


void SetupWifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize mDNS
  if (!MDNS.begin(HOSTNAME))
  {
    Serial.println("Error setting up MDNS responder!");
  }
  else
  {
    Serial.println("mDNS responder started");
  }

  // Need to explicitly set hostname as ArduinoOTA will override our mdns-name set above
  ArduinoOTA.setHostname(HOSTNAME);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  randomSeed(micros());
}


void setup()
{
  // Outputs
#ifdef LED_RED
  pinMode(LED_RED, OUTPUT);
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

  Serial.begin(9600);
  Serial.setTimeout(2000);

  SetupWifi();

  g_MQTTClient.setServer(mqtt_server, MQTT_PORT);
  g_MQTTClient.setBufferSize(MQTT_MAX_SIZE);
  g_MQTTClient.setCallback(MQTTCallback);

  // Allow the hardware to sort itself out
  delay(1500);

  MQTTReconnect();
}


void loop()
{
  static elapsedMillis MQTTReconnectTimer = 0;
  static elapsedMillis WifiReconnectTimer = 0;
  static elapsedMillis ledTimer = 0;

  if (WiFi.status() != WL_CONNECTED) // Check for wifi disconnects
  {
#ifdef LED_RED
    digitalWrite(LED_RED, LOW); // Always on: failure
#endif

    if (WifiReconnectTimer > 5000)
    {
      Serial.print(millis());
      Serial.println("Reconnecting to WiFi...");
      WiFi.disconnect();
      WiFi.reconnect();
      MQTTReconnect();
      WifiReconnectTimer = 0;
      MQTTReconnectTimer = 0;
    }
  }
  else if (!g_MQTTClient.connected()) // Check for MQTT disconnects
  {
#ifdef LED_RED
    digitalWrite(LED_RED, LOW); // Always on: failure
#endif

    if (MQTTReconnectTimer > 5000)
    {
      MQTTReconnect();
      MQTTReconnectTimer = 0;
    }
  }
  else
  {
    // Indicate we're running:
#ifdef LED_RED
    if (ledTimer > 2000)
    {
      digitalWrite(LED_RED, HIGH); // Off
      ledTimer = 0;
    }
    else if (ledTimer > 1000)
    {
      digitalWrite(LED_RED, LOW); // On
    }

#endif

    g_MQTTClient.loop();

    // Handle OTA-updates
    ArduinoOTA.handle();
  }

  g_DaikinCtrl.loop();
}
