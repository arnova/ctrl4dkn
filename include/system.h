#ifndef SYSTEM_H
#define SYSTEM_H

#include "secrets.h"

// WiFi settings
const char ssid[] = WIFI_SSID;          // Need to create + define in secrets.h
const char password[] = WIFI_PASSWORD;  // Need to create + define in secrets.h

// MQTT server settings
const char mqtt_server[] = "192.168.1.65";
#define MQTT_PORT 1883

#define MQTT_MAX_SIZE 1024

#define MQTT_P1P2_HOST_ID                           "066" // NOTE: Need to custimize this
#define MQTT_P1P2_P_PREFIX                          "P1P2/P/" MQTT_P1P2_HOST_ID "/"
#define MQTT_P1P2_W_TOPIC                           "P1P2/W/" MQTT_P1P2_HOST_ID "/"  // FIXME: HOST_ID ?

#define MQTT_CTRL4DKN_HOST_ID                       "067"
#define MQTT_CTRL4DKN_CTRL_PREFIX                   "ctrl4dkn/c/"
#define MQTT_CTRL4DKN_STATUS_PREFIX                 "ctrl4dkn/s/"

// P1P2 write topics
#define MQTT_P1P2_W_SET_POINT_CMD                   "E360"      // Room temperature setpoint
                                                                // Suffixed with 2-digit temperature hex-value
#define MQTT_P1P2_W_ON_OFF_CMD                      "E352F"     // Heating/cooling on/off
                                                                // Suffixed with either 0 (off) or 1 (on)
#define MQTT_P1P2_W_AWT_DEV_CMD                     "E368"      // AWT temperature deviation
                                                                // Suffixed with temperature:
                                                                // E36 8 A sets deviation to +1 C
                                                                // E36 8 FFF6 sets deviation to -1 C
#define MQTT_P1P2_W_SILENT_ON_OFF_CMD               "E3500030"  // Silent mode on/off
                                                                // Suffixed with either 0 (off) or 1 (on)
#define MQTT_P1P2_W_SILENT_LEVEL_CMD                "E3500010"  // Silent level
                                                                // Suffixed with 1, 2 or 3 to select level

// Subscribe topics
#define MQTT_P1P2_P_POWER_HEATPUMP                  MQTT_P1P2_P_PREFIX "M/9/Power_Heatpump" // Not used
#define MQTT_P1P2_P_HEATING_ON                      MQTT_P1P2_P_PREFIX "S/0/Heating_OnOff"
#define MQTT_P1P2_P_CIRCULATION_PUMP_ON             MQTT_P1P2_P_PREFIX "S/1/Circulation_Pump_OnOff"
#define MQTT_P1P2_P_COMPRESSOR_ON                   MQTT_P1P2_P_PREFIX "S/1/Compressor_OnOff" // Not used
#define MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE MQTT_P1P2_P_PREFIX "S/1/Target_Temperature_Room"
#define MQTT_P1P2_P_VALVE_ZONE_MAIN                 MQTT_P1P2_P_PREFIX "S/1/Valve_Zone_Main"
#define MQTT_P1P2_P_LEAVING_WATER_TEMP              MQTT_P1P2_P_PREFIX "T/1/Temperature_Leaving_Water"
#define MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE   MQTT_P1P2_P_PREFIX "T/1/Temperature_Room"

// Control topics
#define MQTT_CONTROLLER_ON_OFF                      "controller_on_off"
#define MQTT_PRIMARY_ZONE_HEATING                   "primary_zone_heating"
#define MQTT_SECONDARY_ZONE_HEATING                 "secondary_zone_heating"
#define MQTT_ROOM1_HEATING                          "room1_heating"
#define MQTT_ROOM2_HEATING                          "room2_heating"
#define MQTT_ROOM3_HEATING                          "room3_heating"
#define MQTT_PRIMARY_ZONE_SET_POINT                 "primary_zone_set_point" //FIXME
#define MQTT_GAS_ONLY_ON                            "gas_only" //FIXME

// Status topics
#define MQTT_VALVE_ZONE_PRIMARY_OPEN                "valve_zone_primary_open"
#define MQTT_VALVE_ROOM1_OPEN                       "valve_room1_open"
#define MQTT_VALVE_ROOM2_OPEN                       "valve_room2_open"
#define MQTT_VALVE_ROOM3_OPEN                       "valve_room3_open"
#define MQTT_DAIKIN_ZONE_PRIMARY_ENABLE             "daikin_zone_primary_enable"
#define MQTT_DAIKIN_ZONE_SECONDARY_ENABLE           "daikin_zone_secondary_enable"
#define MQTT_LEAVING_WATER_TOO_HIGH                 "leaving_water_too_high"
#define MQTT_ZONE_PRIMARY_REAL_SET_POINT            "zone_primary_real_set_point" //FIXME
#define MQTT_ZONE_PRIMARY_SAVED_SET_POINT           "zone_primary_saved_set_point" //FIXME

#define DAIKIN_HYSTERESIS                     1.0f
#define DAIKIN_PRIMARY_ZONE_OFF_TEMPERATURE  15.0f
#define DEFAULT_SET_POINT                    18.0f  // FIXME: Not used
#define LEAVING_WATER_MAX                    45.0f
#define LEAVING_WATER_MAX_MARGIN              3.0f
#define DAIKIN_ZONE_SWITCH_TIME               1     // Minute
#define PRIMARY_ZONE_DISABLE_TIME             5     // Minutes
#define PRIMARY_ZONE_VALVE_DELAY              3     // Minutes
#define PRIMARY_ZONE_PROTECTION_DELAY         5     // Minutes

#define CONTROL_LOOP_TIME                    60     // Seconds
#define MQTT_UPDATE_TIME                      1     // Seconds

// set output pin numbers
const int LED_RED = 25;                         // Onboard LED

// Low voltage relais:
const int DAIKIN_PREFERENTIAL_RELAY = 33;       // K1 relay: kWh preferential kWh (aka. gas only) rate relay (X5M 3/4) on the Daikin. NOT IMPLEMENTED (yet)

// High voltage relais:
const int DAIKIN_SECONDARY_ZONE_RELAY = 32;     // K2 relay: Enables the secondary zone input (X2M 1a+4) on the Daikin (if used. Requires configuration on Daikin)
const int DAIKIN_PRIMARY_ZONE_RELAY = 13;       // K3 relay: Enables the primary zone input (X2M 1+4) on the Daikin (if used. Requires configuration on Daikin)
const int ROOM3_OPEN_VALVE_RELAY = 21;          // K5 relay: Used to open/close room 3 valve (floor-heating)
const int ROOM2_OPEN_VALVE_RELAY = 19;          // K6 relay: Used to open/close room 2 valve (floor-heating)
const int ROOM1_OPEN_VALVE_RELAY = 18;          // K7 relay: Used to open/close room 1 valve (floor-heating)
const int PRIMARY_ZONE_CLOSE_VALVE_RELAY = 5;   // K8 relay: Used to close/open the primary zone (floor-heating)

// set input pin numbers
const int SECONDARY_ZONE_ENABLE = 22;           // Enable (thermostat) input for the secondary zone (low active)
const int ROOM1_HEATING_ENABLE = 23;            // Enable (thermostat) input for room 1 (floor-heating) (if used) (low active)
const int ROOM2_HEATING_ENABLE = 26;            // Enable (thermostat) input for room 2 (floor-heating) (if used) (low active)
const int ROOM3_HEATING_ENABLE = 27;            // Enable (thermostat) input for room 3 (floor-heating) (if used) (low active)

const int HARDWARE_MAX_TEMP_SENSOR = 14;        // Hardware sensor (clixon) to detect maximum temperature exceeded (if used) (low active)

#endif // SYSTEM_H
