#ifndef SYSTEM_H
#define SYSTEM_H

#include "secrets.h"

// WiFi settings
const char ssid[] = WIFI_SSID;          // Need to create + define in secrets.h
const char password[] = WIFI_PASSWORD;  // Need to create + define in secrets.h

// MQTT server settings
const char mqtt_server[] = "192.168.1.65";
#define MQTT_PORT 1883

#define MQTT_P1P2_HOST_ID "066"                 // NOTE: Need to customize this

#define MQTT_CTRL4DKN_HOST_ID "067"             // NOTE: Need to customize this

// set output pin numbers
const int LED_RED = 25;                         // Onboard LED

// Low voltage relais:
const int DAIKIN_PREFERENTIAL_RELAY = 33;       // K1 relay: kWh preferential kWh (aka. gas only) rate relay (X5M 3/4) on the Daikin. NOT IMPLEMENTED (yet)

// High voltage relais:
                                                // K2 relay: Not used for now
const int ROOM3_OPEN_VALVE_RELAY = 32;          // K3 relay: Used to open/close room 3 valve (floor-heating)
const int ROOM2_OPEN_VALVE_RELAY = 13;          // K4 relay: Used to open/close room 2 valve (floor-heating)
const int ROOM1_OPEN_VALVE_RELAY = 21;          // K5 relay: Used to open/close room 1 valve (floor-heating)
const int PRIMARY_ZONE_CLOSE_VALVE_RELAY = 19;  // K6 relay: Used to close/open the primary zone (floor-heating)
const int DAIKIN_SECONDARY_ZONE_RELAY = 18;     // K7 relay: Enables the secondary zone input (X2M 1a+4) on the Daikin (if used. Requires configuration on Daikin)
const int DAIKIN_PRIMARY_ZONE_RELAY = 5;        // K8 relay: Enables the primary zone input (X2M 1+4) on the Daikin (if used. Requires configuration on Daikin)

// set input pin numbers
const int SECONDARY_ZONE_ENABLE = 22;           // Enable (thermostat) input for the secondary zone (low active)
const int ROOM1_HEATING_ENABLE = 23;            // Enable (thermostat) input for room 1 (floor-heating) (if used) (low active)
const int ROOM2_HEATING_ENABLE = 26;            // Enable (thermostat) input for room 2 (floor-heating) (if used) (low active)
const int ROOM3_HEATING_ENABLE = 27;            // Enable (thermostat) input for room 3 (floor-heating) (if used) (low active)

const int HARDWARE_MAX_TEMP_SENSOR = 14;        // Hardware sensor (clixon) to detect maximum temperature exceeded (if used) (low active)

#endif // SYSTEM_H
