#include <Arduino.h>
#include <WiFi.h>
#include <mDNS.h>
//#include <WiFi.h>
//#include <WiFiSTA.h>
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

  Serial.write(payload, length);
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
      g_DaikinCtrl.UpdateDaikinTargetTemperature(fVal); // FIXME: Need to only saved value
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_C_PRIMARY_ZONE_HEATING))
  {
    if (bValidInt)
    {
      if (iVal == 0 || iVal == 1)
        g_DaikinCtrl.SetCtrlPriZoneHeating(iVal);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_C_SECONDARY_ZONE_HEATING))
  {
    if (bValidInt)
    {
      if (iVal >= 0 && iVal <= 2)
        g_DaikinCtrl.SetCtrlSecZoneHeating(iVal);
      else
        MQTTPrintError();
    }
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
  else if (STRIEQUALS(topic, MQTT_C_HYSTERESIS_HACK_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
        g_DaikinCtrl.SetCtrlHysteresisHack(true);
      else if (iVal == 0)
        g_DaikinCtrl.SetCtrlHysteresisHack(false);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_C_PRIMARY_ZONE_SET_POINT))
  {
    if (bValidFloat)
      g_DaikinCtrl.UpdateDaikinTargetTemperature(fVal); // FIXME: Need to only saved value
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_C_EXTRA_ZONE_HEATING))
  {
    if (bValidInt)
    {
      if (iVal == 0 || iVal == 1)
        g_DaikinCtrl.SetCtrlExtraZoneHeating(iVal);
      else
        MQTTPrintError();
    }
    else
      MQTTPrintError();
  }
  else if (STRIEQUALS(topic, MQTT_C_GAS_ONLY_ON))
  {
    if (bValidInt)
    {
      if (iVal == 1)
        digitalWrite(DaikinPreferentialRelay, HIGH);
      else if (iVal == 0)
        digitalWrite(DaikinPreferentialRelay, LOW);
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
}


void MQTTPublishConfig()
{
  JsonDocument root;
  root["name"] = "Ctrl4Dkn Valve_Zone_Extra_Open";
  root["state_topic"] = MQTT_S_VALVE_ZONE_EXTRA_OPEN;
//  root["command_topic"] = "ctrl4dkn/" MQTT_S_VALVE_ZONE_EXTRA_OPEN "/set";
  root["command_topic"] = MQTT_C_EXTRA_ZONE_HEATING;
  root["payload_on"] = "1";
  root["payload_off"] = "0";
  root["state_on"] = "1";
  root["state_off"] = "0";
  root["unique_id"] = "ctrl4dkn_01"; // Optional
//  root["value_template"] = "{{ value_json.state }}"; // FIXME

  //JsonObject device  = root.createNestedObject("device");
  JsonObject device = root.to<JsonObject>();
  device["name"] = "Ctrl4Dkn";
  device["model"] = "Ctrl4Dkn";
  device["manufacturer"] = "Arnova";
  device["identifiers"][0] = "Ctrl4DknID00";

//  root["device_class"] = "sensor";
//  root["unit_of_meas"] = "%";
//  root["dev_cla"] = "humidity";
//  root["frc_upd"] = true;
  //root["val_tpl"] = "{{value_json['ESP32']['Humidity']}}";

  // Output to console
  serializeJsonPretty(root, Serial);
  Serial.println();

  // Serialize JSON for MQTT
  char message[MQTT_MAX_SIZE];
  serializeJson(root, message);
  Serial.println(message); //Prints it out on one line.
  g_MQTTClient.publish("homeassistant/switch/ctrl4dkn_01/" "Valve_Zone_Extra_Open" "/config", message, true);
}


void MQTTReconnect()
{
  // Loop until we're reconnected
  while (!g_MQTTClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32But-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
//    if (MQTTClient.connect(clientId.c_str(), NULL, NULL, "test", 0, false, "not connected", false))
    if (g_MQTTClient.connect(clientId.c_str()))
    {
      Serial.println("connected");
      //Once connected, publish an announcement...
      //MQTTClient.publish(MQTT_PUBLISH_TOPIC, "testing...");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(g_MQTTClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  // Subscribe to messages from p1p2serial etc.
  g_MQTTClient.subscribe(MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_LEAVING_WATER_TEMP, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_COMPRESSOR_ON, 0); // Use something else ?
  g_MQTTClient.subscribe(MQTT_P1P2_P_CIRCULATION_PUMP_ON, 0);
  g_MQTTClient.subscribe(MQTT_P1P2_P_HEATING_ON, 0);

  // CTRL4DKN topics
  g_MQTTClient.subscribe(MQTT_C_HYSTERESIS_HACK_ON, 0);
  g_MQTTClient.subscribe(MQTT_C_PRIMARY_ZONE_SET_POINT, 0);
  g_MQTTClient.subscribe(MQTT_C_PRIMARY_ZONE_HEATING, 0);
  g_MQTTClient.subscribe(MQTT_C_SECONDARY_ZONE_HEATING, 0);
  g_MQTTClient.subscribe(MQTT_C_EXTRA_ZONE_HEATING, 0);
  g_MQTTClient.subscribe(MQTT_C_GAS_ONLY_ON, 0);

  // Publish MQTT config for eg. HA discovery
  MQTTPublishConfig();
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

//  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void setup()
{
  // Outputs
  pinMode(LedRed, OUTPUT);
  pinMode(DaikinPrimaryZoneRelay, OUTPUT);
  pinMode(DaikinSecondaryZoneRelay, OUTPUT);
  pinMode(PrimaryZoneCloseValveRelay, OUTPUT);
  pinMode(ExtraZoneOpenValveRelay, OUTPUT);
  pinMode(DaikinPreferentialRelay, OUTPUT);

  // FIXME: These should be set according to the previous state?
  digitalWrite(DaikinPrimaryZoneRelay, LOW);
  digitalWrite(DaikinSecondaryZoneRelay, LOW);
  digitalWrite(PrimaryZoneCloseValveRelay, LOW);
  digitalWrite(ExtraZoneOpenValveRelay, HIGH); // FIXME
  digitalWrite(DaikinPreferentialRelay, LOW);

  // Inputs
  pinMode(SecondaryZoneThermostat, INPUT_PULLUP);
  pinMode(ExtraZoneThermostaat, INPUT_PULLUP);
  pinMode(HardwareMaxTempSensor, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.setTimeout(2000);

  SetupWifi();
  g_MQTTClient.setServer(mqtt_server, MQTT_PORT);

  g_MQTTClient.setCallback(MQTTCallback);
  g_MQTTClient.setBufferSize(MQTT_MAX_SIZE);
  MQTTReconnect();
}


void loop()
{
  // Handle OTA-updates
  ArduinoOTA.handle();

  // Listen for mqtt message and reconnect if disconnected
  if (!g_MQTTClient.connected())
  {
    digitalWrite(LedRed, HIGH); // Off

    MQTTReconnect();
  }
  else
  {
    // Indicate we're running:
    digitalWrite(LedRed, LOW); // On

    g_MQTTClient.loop();
  }

  g_DaikinCtrl.loop();
}
