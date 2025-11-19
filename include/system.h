#ifndef SYSTEM_H
#define SYSTEM_H

#include "secrets.h"

#define CONTROL_LOOP_TIME                       1   // Seconds

// WiFi settings
const char HOSTNAME[] = "ctrl4dkn";
const char SSID[] = WIFI_SSID;          // Need to create + define in secrets.h
const char PASSWORD[] = WIFI_PASSWORD;  // Need to create + define in secrets.h

#define MQTT_CTRL4DKN_HOST_ID "067"             // NOTE: Need to customize this

// MQTT server settings
const char mqtt_server[] = "192.168.1.65";
#define MQTT_PORT 1883

// Select the appropriate one below (depends on P1P2Serial/bridge firmware) 
//#define MQTT_P1P2_HOST_ID "066"               // v0.9.44 or older. NOTE: Need to customize this
#define MQTT_P1P2_HOST_ID "P1P2MQTT/bridge0"    // v0.9.45 or newer. NOTE: *May* need to customize this

// Enable below when a bridge with older firmware (<v0.9.45) is used
#define MQTT_P1P2_OLD_BRIDGE

// Enable this when (Daikin) secondary zone uses low temperature AWT range that is safe for floor-heated rooms
#define LOW_TEMP_SECONDARY_ZONE

// Enable this to enable hysteresis hack when using Daikin wall room thermostat
//#define HYSTERSIS_HACK

/**************************
 * Output i/o pin numbers *
 **************************/
#define LED_RED 25                              // Onboard LED

// Low voltage relais:
//#define DAIKIN_PREFERENTIAL_RELAY 33            // K1 relay: kWh preferential kWh (aka. gas only) rate relay (X5M 3/4) on the Daikin

// High voltage relays:
//#define ROOM4_VALVE_RELAY 32                    // K2 relay: Used to open/close room 4 valve (floor-heating)
#define ROOM4_VALVE_POLARITY 1                  // 1 = Normally closed, 0 = Normally open

#define ROOM3_VALVE_RELAY 13                    // K3 relay: Used to open/close room 3 valve (floor-heating)
#define ROOM3_VALVE_POLARITY 0                  // 1 = Normally closed, 0 = Normally open

#define ROOM2_VALVE_RELAY 12                    // K4 relay: Used to open/close room 2 valve (floor-heating)
#define ROOM2_VALVE_POLARITY 0                  // 1 = Normally closed, 0 = Normally open

#define ROOM1_VALVE_RELAY 21                    // K5 relay: Used to open/close room 1 valve (floor-heating)
#define ROOM1_VALVE_POLARITY 1                  // 1 = Normally closed, 0 = Normally open

#define PRIMARY_ZONE_VALVE_RELAY 19             // K6 relay: Used to close/open the primary zone (floor-heating)
#define PRIMARY_ZONE_VALVE_POLARITY 0           // 1 = Normally closed, 0 = Normally open

#define DAIKIN_SECONDARY_ZONE_RELAY 18          // K7 relay: Enables the secondary zone input (X2M 1a+4) on the Daikin (if used. Requires configuration on Daikin)
#define DAIKIN_PRIMARY_ZONE_RELAY 5             // K8 relay: Enables the primary zone input (X2M 1+4) on the Daikin (if used. Requires configuration on Daikin)

// set input pin numbers
//#define SECONDARY_ZONE_ENABLE 22                // Enable (thermostat) input for the secondary zone (low active)
//#define ROOM1_ENABLE 23                         // Enable (thermostat) input for room 1 (floor-heating) (if used) (low active)
//#define ROOM2_ENABLE 26                         // Enable (thermostat) input for room 2 (floor-heating) (if used) (low active)
//#define ROOM3_ENABLE 27                         // Enable (thermostat) input for room 3 (floor-heating) (if used) (low active)
//#define ROOM4_ENABLE 34                         // Enable (thermostat) input for room 4 (floor-heating) (if used) (low active)

//#define HARDWARE_MAX_TEMP_SENSOR 14             // Hardware sensor (clixon) to detect maximum temperature exceeded (if used) (low active)

#endif // SYSTEM_H
