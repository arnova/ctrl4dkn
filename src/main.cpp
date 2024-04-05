#include <Arduino.h>
#include <WiFi.h>
//#include <WiFiSTA.h>
//#include <mDNS.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>

#include "daikinctrl.h"
#include "util.h"
#include "system.h"


// Globals
WiFiClient g_wifiClient;
PubSubClient g_MQTTClient(g_wifiClient);
CDaikinCtrl g_DaikinCtrl(&g_MQTTClient);

void MQTTPrintError(void)
{
  Serial.println("ERROR: Invalid MQTT data for topic.");
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

  if (STRIEQUALS(topic, MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE))
  {
    if (bValidFloat)
      g_DaikinCtrl.SetP1P2PrimaryZoneRoomTemp(fVal);
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE))
  {
    if (bValidFloat)
      g_DaikinCtrl.SetP1P2PrimaryZoneTargetTemp(fVal); // FIXME: Need to only saved value
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
  else if (STRIEQUALS(topic, MQTT_P1P2_P_COMPRESSOR_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
        g_DaikinCtrl.SetP1P2CompressorOn(true);
      else if (iVal == 0)
        g_DaikinCtrl.SetP1P2CompressorOn(false);
      else
        MQTTPrintError();
    }
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
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_PRIMARY_ZONE_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlPriZoneEnable(iVal == 1 ? true : false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_SECONDARY_ZONE_ENABLE "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 0 || iVal == 1 || length == 0)
        g_DaikinCtrl.SetCtrlSecZoneEnable(iVal == 1 ? true : false);
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
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_PRIMARY_ZONE_SET_POINT "/set"))
  {
    if (bValidFloat)
      g_DaikinCtrl.UpdateDaikinTargetTemperature(fVal); // FIXME: Need to only saved value
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_CTRL4DKN_CTRL_PREFIX MQTT_GAS_ONLY_ON "/set"))
  {
    if (bValidInt || length == 0)
    {
      if (iVal == 1)
        digitalWrite(DAIKIN_PREFERENTIAL_RELAY, HIGH);
      else if (iVal == 0 || length == 0)
        digitalWrite(DAIKIN_PREFERENTIAL_RELAY, LOW);
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
    case CDaikinCtrl::SENSOR:
    {
      root["state_topic"] = String(MQTT_CTRL4DKN_STATUS_PREFIX) + strItem;
      root["payload_on"] = "1";
      root["payload_off"] = "0";
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

    case CDaikinCtrl::SENSOR:
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
  g_MQTTClient.subscribe(MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_LEAVING_WATER_TEMP, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_COMPRESSOR_ON, 0); // Use something else ?
  g_MQTTClient.subscribe(MQTT_P1P2_P_CIRCULATION_PUMP_ON, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_HEATING_ON, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_COOLING_ON, 0);

  // Control topics
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_CONTROLLER_ON_OFF "/set", 1);
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_PRIMARY_ZONE_ENABLE "/set", 1);
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_SECONDARY_ZONE_ENABLE "/set", 1);
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM1_ENABLE "/set", 1);
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM2_ENABLE "/set", 1);
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_ROOM3_ENABLE "/set", 1);
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_PRIMARY_ZONE_SET_POINT "/set", 1);
  g_MQTTClient.subscribe(MQTT_CTRL4DKN_CTRL_PREFIX MQTT_GAS_ONLY_ON "/set", 1);

  // Publish MQTT config for eg. HA discovery
  MQTTPublishConfig(MQTT_CONTROLLER_ON_OFF, CDaikinCtrl::SWITCH);
  MQTTPublishConfig(MQTT_PRIMARY_ZONE_ENABLE, CDaikinCtrl::SWITCH);
  MQTTPublishConfig(MQTT_SECONDARY_ZONE_ENABLE, CDaikinCtrl::SWITCH);
  MQTTPublishConfig(MQTT_ROOM1_ENABLE, CDaikinCtrl::SWITCH);
  MQTTPublishConfig(MQTT_ROOM2_ENABLE, CDaikinCtrl::SWITCH);
  MQTTPublishConfig(MQTT_ROOM3_ENABLE, CDaikinCtrl::SWITCH);

  MQTTPublishConfig(MQTT_VALVE_ZONE_PRIMARY_OPEN, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_VALVE_ROOM1_OPEN, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_VALVE_ROOM2_OPEN, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_VALVE_ROOM3_OPEN, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_DAIKIN_ZONE_PRIMARY_ENABLE, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_DAIKIN_ZONE_SECONDARY_ENABLE, CDaikinCtrl::BINARY_SENSOR);
  MQTTPublishConfig(MQTT_LEAVING_WATER_TOO_HIGH, CDaikinCtrl::BINARY_SENSOR);

  return true;
}


void SetupWifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
//    delay(5000);
//    ESP.restart();
  }

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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup()
{
  // Outputs
  pinMode(LED_RED, OUTPUT);
  pinMode(DAIKIN_PRIMARY_ZONE_RELAY, OUTPUT);
  pinMode(DAIKIN_SECONDARY_ZONE_RELAY, OUTPUT);
  pinMode(PRIMARY_ZONE_VALVE_RELAY, OUTPUT);
  pinMode(ROOM1_VALVE_RELAY, OUTPUT);
  pinMode(ROOM2_VALVE_RELAY, OUTPUT);
  pinMode(ROOM3_VALVE_RELAY, OUTPUT);
  pinMode(DAIKIN_PREFERENTIAL_RELAY, OUTPUT);

  digitalWrite(DAIKIN_PRIMARY_ZONE_RELAY, LOW);
  digitalWrite(DAIKIN_SECONDARY_ZONE_RELAY, LOW);
  digitalWrite(PRIMARY_ZONE_VALVE_RELAY, PRIMARY_ZONE_VALVE_POLARITY ? HIGH : LOW);
  digitalWrite(ROOM1_VALVE_RELAY, ROOM1_VALVE_POLARITY ? LOW : HIGH);
  digitalWrite(ROOM2_VALVE_RELAY, ROOM2_VALVE_POLARITY ? LOW : HIGH);
  digitalWrite(ROOM3_VALVE_RELAY, ROOM3_VALVE_POLARITY ? LOW : HIGH);
  digitalWrite(DAIKIN_PREFERENTIAL_RELAY, LOW);

  // Inputs
  pinMode(SECONDARY_ZONE_ENABLE, INPUT_PULLUP);
  pinMode(ROOM1_HEATING_ENABLE, INPUT_PULLUP);
  pinMode(HARDWARE_MAX_TEMP_SENSOR, INPUT_PULLUP);

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

  // Handle OTA-updates
  ArduinoOTA.handle();

  // Listen for mqtt message and reconnect if disconnected
  if (!g_MQTTClient.connected())
  {
    digitalWrite(LED_RED, HIGH); // Off

    if (MQTTReconnectTimer > 5000)
    {
      MQTTReconnect();
      MQTTReconnectTimer = 0;
    }
  }
  else
  {
    // Indicate we're running:
    digitalWrite(LED_RED, LOW); // On

    g_MQTTClient.loop();
  }

  g_DaikinCtrl.loop();
}
