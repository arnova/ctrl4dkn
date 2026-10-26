#pragma once
#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

// Version string:
#define MY_VERSION "1.00"

// Firmware version string
const char VER_STR_P[] PROGMEM = "Ctrl4Dkn " MY_VERSION " - (C) 2024-2026 Arno van Amersfoort (Arnova)";

// Misc. (home assistant) settings
#define DEFAULT_HOST_NAME                       "Ctrl4Dkn"
#define DEVICE_NAME                             "Ctrl4Dkn"
#define DEVICE_MODEL                            "Ctrl4Dkn"
#define MANUFACTURER                            "Arnova"

// Enable below for additional wifi / mqtt debug messages
#define WIFI_DEBUG
//#define MQTT_DEBUG

// Settle time after (re)connecting/subscribing: lets the broker resend retained setters (before we publish our own)
// and HA discover new entities (before we publish their values)
#define MQTT_SETTLE_TIME_MS                     5000  // ms

// Socket server port. Uncomment to disable
#define SOCKET_SERVER_PORT                      8000

// UART/socket settings
#define BAUD_RATE                               115200

// GPIO pin numbers
#define STATUS_LED 25                           // Onboard LED

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
