#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

// Version string:
#define MY_VERSION "1.00"

// Firmware version string
const char VER_STR_P[] PROGMEM = "Ctrl4Dkn " MY_VERSION " - (C) 2024-2026 Arno van Amersfoort (Arnova)";

// Network watchdog timer settings
#define NETWORK_WATCHDOG_TIMEOUT_DEFAULT        900   // Seconds = 15 minutes
#define NETWORK_WATCHDOG_TIMEOUT_MIN            60    // Seconds = 1 minute (0 = disabled)
#define NETWORK_WATCHDOG_TIMEOUT_MAX            65000

#define NETWORK_WATCHDOG_RECOVERY_DEFAULT       900   // Seconds = 15 minutes
#define NETWORK_WATCHDOG_RECOVERY_MIN           60    // Seconds = 1 minute
#define NETWORK_WATCHDOG_RECOVERY_MAX           65000

#define MQTT_UPDATE_TIME_DEFAULT                10     // Seconds
#define MQTT_UPDATE_TIME_MIN                    1      // Seconds
#define MQTT_UPDATE_TIME_MAX                    254    // Seconds

// Enable below for additional wifi / mqtt debug messages
#define WIFI_DEBUG
//#define MQTT_DEBUG

// Wifi connect timeout
#define WIFI_CONNECT_TIMEOUT                    10000 // ms

// Wifi disconnect timeout (max. wait for disconnect to complete when restarting WiFi)
#define WIFI_DISCONNECT_TIMEOUT                 1000  // ms

// MQTT connect timeout
#define MQTT_CONNECT_TIMEOUT                    10000 // ms

// Time to wait after MQTT (re)subscribing before publishing setter values (allow retained values to arrive first)
// Settle time after (re)connecting/subscribing: lets the broker resend retained setters (before we publish our own)
// and HA discover new entities (before we publish their values)
#define MQTT_SETTLE_TIME_MS                     5000  // ms

// MQTT settings
#define MQTT_PORT                               1883
#define MQTT_MAX_MESSAGE_SIZE                   1024
#define MQTT_MAX_TOPIC_ITEM_SIZE                64
#define MQTT_MAX_CONFIG_TOPIC_SIZE              128

// Select the appropriate one below (depends on P1P2Serial/bridge firmware) 
//#define MQTT_P1P2_HOST_ID "066"               // v0.9.44 or older. NOTE: Need to customize this
#define MQTT_P1P2_HOST_ID "P1P2MQTT/bridge0"    // v0.9.45 or newer. NOTE: *May* need to customize this

// Enable below when a bridge with older firmware (<v0.9.45) is used
//#define MQTT_P1P2_OLD_BRIDGE // FIXME: Setting

// Default for when (Daikin) secondary zone uses low temperature AWT range that is safe for floor-heated rooms
#define LOW_TEMP_SECONDARY_ZONE_DEFAULT         false

// Default for short cycle prevention (disable zone when short cycling is detected)
#define SHORT_CYCLE_PREVENTION_DEFAULT          true

// Socket server settings
#define SOCKET_SERVER_PORT                      8000

// Misc. (home assistant) settings
#define DEFAULT_HOST_NAME                       "Ctrl4Dkn"
#define DEVICE_NAME                             "Ctrl4Dkn"
#define DEVICE_MODEL                            "Ctrl4Dkn"
#define MANUFACTURER                            "Arnova"

// UART/socket settings
#define BAUD_RATE                     9600
#define CMD_BUF_SIZE                  80
#define RESULT_BUF_SIZE               80
#define SOCKET_CLIENT_TIMEOUT_MS      100 // ms

// EEPROM byte sizes
#define EEPROM_MAX_SIZE         512
#define CHECKSUM_SIZE           2
#define RESERVED_SIZE           100
#define WIFI_SSID_MAX_SIZE      32
#define WIFI_PASSWORD_MAX_SIZE  64
#define HOST_NAME_MAX_SIZE      32
#define MQTT_USER_MAX_SIZE      32
#define MQTT_PASSWORD_MAX_SIZE  64
#define HOST_NAME_MAX_SIZE      32
#define IP_BYTE_SIZE            4
#define NET_WD_TIMEOUT_SIZE     2
#define NET_WD_RECOVER_SIZE     2
#define MQTT_INTERVAL_SIZE      1
#define LOW_TEMP_SEC_ZONE_SIZE  1
#define SC_PREVENTION_SIZE      1

// EEPROM locations
#define EEPROM_CHECKSUM       0                                                   // 0
#define EEPROM_WIFI_SSID      EEPROM_CHECKSUM + CHECKSUM_SIZE                     // 2
#define EEPROM_WIFI_PASSWORD  EEPROM_WIFI_SSID + WIFI_SSID_MAX_SIZE + 1           // 35
#define EEPROM_IP_ADDR        EEPROM_WIFI_PASSWORD + WIFI_PASSWORD_MAX_SIZE + 1   // 100
#define EEPROM_IP_NETMASK     EEPROM_IP_ADDR + IP_BYTE_SIZE                       // 104
#define EEPROM_MQTT_IP_ADDR   EEPROM_IP_NETMASK + IP_BYTE_SIZE                    // 108
#define EEPROM_MQTT_USER      EEPROM_MQTT_IP_ADDR + IP_BYTE_SIZE                  // 112
#define EEPROM_MQTT_PASSWORD  EEPROM_MQTT_USER + MQTT_USER_MAX_SIZE + 1           // 145
#define EEPROM_HOST_NAME      EEPROM_MQTT_PASSWORD + MQTT_PASSWORD_MAX_SIZE + 1   // 210
#define EEPROM_NET_WD_TIMEOUT EEPROM_HOST_NAME + HOST_NAME_MAX_SIZE + 1           // 275
#define EEPROM_NET_WD_RECOVER EEPROM_NET_WD_TIMEOUT + NET_WD_TIMEOUT_SIZE         // 277
#define EEPROM_MQTT_INTERVAL  EEPROM_NET_WD_RECOVER + NET_WD_RECOVER_SIZE         // 279
#define EEPROM_LOW_TEMP_SEC_ZONE EEPROM_MQTT_INTERVAL + MQTT_INTERVAL_SIZE        // 280
#define EEPROM_SC_PREVENTION  EEPROM_LOW_TEMP_SEC_ZONE + LOW_TEMP_SEC_ZONE_SIZE   // 281
#define EEPROM_RESERVED       EEPROM_SC_PREVENTION + SC_PREVENTION_SIZE           // 282

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
