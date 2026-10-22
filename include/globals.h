#pragma once

#ifndef GLOBALS_H
#define GLOBALS_H

#define CONTROL_LOOP_TIME                       1000 // ms

// Home Assistant birth/last will topic (HA publishes "online" here after it (re)started)
#define MQTT_HA_STATUS_TOPIC                    "homeassistant/status"

// Control topics
#define MQTT_CONTROLLER_ON_OFF                  "controller_enable"
#define MQTT_ZONE_PRIMARY_ENABLE                "zone_primary_enable"
#define MQTT_ZONE_SECONDARY_ENABLE              "zone_secondary_enable"
#define MQTT_ZONE_SECONDARY_FORCE               "zone_secondary_force"
#define MQTT_VALVE_PRIMARY_CLOSE_FORCE          "valve_primary_close_force"
#define MQTT_DAIKIN_DISABLE                     "daikin_disable"
#define MQTT_ROOM1_ENABLE                       "room_1_enable"
#define MQTT_ROOM2_ENABLE                       "room_2_enable"
#define MQTT_ROOM3_ENABLE                       "room_3_enable"
#define MQTT_ROOM4_ENABLE                       "room_4_enable"
#define MQTT_GAS_ONLY                           "gas_only"
#define MQTT_SHORT_CYCLE_PREVENT                "short_cycle_prevention"

// Status topics
#define MQTT_FW_VERSION                         "firmware_version"
#define MQTT_VALVE_ZONE_PRIMARY_OPEN            "valve_zone_primary_open"
#define MQTT_VALVE_ROOM1_OPEN                   "valve_room_1_open"
#define MQTT_VALVE_ROOM2_OPEN                   "valve_room_2_open"
#define MQTT_VALVE_ROOM3_OPEN                   "valve_room_3_open"
#define MQTT_VALVE_ROOM4_OPEN                   "valve_room_4_open"
#define MQTT_DAIKIN_PRIMARY_ENABLED             "daikin_primary_enabled"
#define MQTT_DAIKIN_SECONDARY_ENABLED           "daikin_secondary_enabled"
#define MQTT_LEAVING_WATER_TOO_HIGH             "leaving_water_too_high"
#define MQTT_AVG_ROOM_TEMPERATURE               "temperature_room_averaged"
#define MQTT_SHORT_CYCLE_PRIMARY_DETECTED       "zone_primary_short_cycle_detected"
#define MQTT_SHORT_CYCLE_SECONDARY_DETECTED     "zone_secondary_short_cycle_detected"

// Diagnostic topic items
#define MQTT_WIFI_SSID                          "wifi_ssid"
#define MQTT_IP_ADDRESS                         "ip_address"
#define MQTT_IP_NETMASK                         "ip_netmask"
#define MQTT_UPDATE_INTERVAL                    "update_interval"
#define MQTT_NET_WD_TIMEOUT                     "network_watchdog_timeout"
#define MQTT_NET_WD_RECOVERY                    "network_watchdog_recovery"
#define MQTT_UP_TIME                            "up_time"
#define MQTT_LWT_MAX                            "leaving_water_max"
#define MQTT_FLOOR_PROTECTION_DELAY             "floor_protection_delay"
#define MQTT_PRIMARY_ZONE_VALVE_DELAY           "valve_zone_primary_delay"
#define MQTT_LWT_COOL_DOWN_DELAY                "leaving_water_cool_down_delay"
#define MQTT_SC_SMALL_WINDOW                    "short_cycle_small_window"
#define MQTT_SC_SMALL_WINDOW_COUNT              "short_cycle_small_window_count"
#define MQTT_SC_LARGE_WINDOW                    "short_cycle_large_window"
#define MQTT_SC_LARGE_WINDOW_COUNT              "short_cycle_large_window_count"
#define MQTT_SC_RECOVERY_TIME                   "short_cycle_recovery_time"

#define MQTT_P1P2_P_PREFIX                          "P1P2/P/" MQTT_P1P2_HOST_ID "/"

// Define P1P2Serial / bridge topics
#ifndef MQTT_P1P2_OLD_BRIDGE
#define MQTT_P1P2_P_HEATING_ON                      MQTT_P1P2_P_PREFIX "S/0/Heating_OnOff"
#define MQTT_P1P2_P_COOLING_ON                      MQTT_P1P2_P_PREFIX "S/0/Cooling_OnOff"
#define MQTT_P1P2_P_CIRCULATION_PUMP_ON             MQTT_P1P2_P_PREFIX "S/1/Circulation_Pump_OnOff"
#define MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE MQTT_P1P2_P_PREFIX "S/1/Target_Temperature_Room"
#define MQTT_P1P2_P_LEAVING_WATER_TEMP              MQTT_P1P2_P_PREFIX "T/1/Temperature_Leaving_Water"
#else
#define MQTT_P1P2_P_HEATING_ON                      MQTT_P1P2_P_PREFIX "S/1/Climate_Heating"
#define MQTT_P1P2_P_COOLING_ON                      MQTT_P1P2_P_PREFIX "S/1/Climate_Cooling"
#define MQTT_P1P2_P_CIRCULATION_PUMP_ON             MQTT_P1P2_P_PREFIX "S/1/Circulation_Pump"
#define MQTT_P1P2_P_PRIMARY_ZONE_TARGET_TEMPERATURE MQTT_P1P2_P_PREFIX "S/0/Room_Heating_Setpoint"
#define MQTT_P1P2_P_LEAVING_WATER_TEMP              MQTT_P1P2_P_PREFIX "T/1/Temperature_R2T_Leaving_Water"
#endif
#define MQTT_P1P2_P_VALVE_ZONE_MAIN                 MQTT_P1P2_P_PREFIX "S/1/Valve_Zone_Main"
#define MQTT_P1P2_P_DEFROST_ACTIVE                  MQTT_P1P2_P_PREFIX "S/1/Defrost_Active"
#define MQTT_P1P2_P_COMPRESSOR                      MQTT_P1P2_P_PREFIX "S/1/Compressor"
#define MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE   MQTT_P1P2_P_PREFIX "T/1/Temperature_Room"
#define MQTT_P1P2_P_ALTHERMA_ON                     MQTT_P1P2_P_PREFIX "S/0/Altherma_On"

#define MQTT_P1P2_P_DATE_TIME                       MQTT_P1P2_P_PREFIX "M/9/Date_Time_Daikin"

#define DAIKIN_HYSTERESIS                     1.0f
#define LEAVING_WATER_MAX_MARGIN              3.0f
#define DAIKIN_ZONE_SWITCH_TIME                 5   // Seconds
#define PRIMARY_ZONE_ROOM_TEMP_AVG_TIME      1800   // Seconds = 30 minutes
#define DAIKIN_ACTIVE_OFF_TIME                900   // Seconds = 15 minutes
#define DAIKIN_STARTUP_TIME                    30   // Seconds

// Expert settings (stored in EEPROM): Defaults and allowed ranges
// NOTE: Float min/max without "f" suffix, since they're also STRINGIZE'd for the help text
#define LEAVING_WATER_MAX_DEFAULT                45.0f
#define LEAVING_WATER_MAX_MIN                    25.0
#define LEAVING_WATER_MAX_MAX                    65.0

// NOTE: Times are in seconds. Delays/counters that count down require a minimum of 1
#define PRIMARY_ZONE_VALVE_DELAY_DEFAULT          180   // 3 minutes
#define PRIMARY_ZONE_VALVE_DELAY_MIN                1
#define PRIMARY_ZONE_VALVE_DELAY_MAX            65000

#define FLOOR_PROTECTION_DELAY_DEFAULT            300   // 5 minutes
#define FLOOR_PROTECTION_DELAY_MIN                  1
#define FLOOR_PROTECTION_DELAY_MAX              65000

#define LWT_COOL_DOWN_DELAY_TIME_DEFAULT          300   // 5 minutes
#define LWT_COOL_DOWN_DELAY_TIME_MIN                1
#define LWT_COOL_DOWN_DELAY_TIME_MAX            65000

// Short cycle detection: Windows/recovery are in minutes
// NOTE: Counts max. is SHORT_CYCLE_SAMPLES (larger counts can never be exceeded)
// NOTE: Recovery max. is limited by its 16 bit (seconds) counter
#define SHORT_CYCLE_SMALL_WINDOW_DEFAULT           15
#define SHORT_CYCLE_SMALL_WINDOW_MIN                1
#define SHORT_CYCLE_SMALL_WINDOW_MAX             1440   // 24 hours

#define SHORT_CYCLE_SMALL_WINDOW_COUNT_DEFAULT      2   // Amount of short cycles allowed during small window
#define SHORT_CYCLE_SMALL_WINDOW_COUNT_MIN          1
#define SHORT_CYCLE_SMALL_WINDOW_COUNT_MAX          SHORT_CYCLE_SAMPLES

#define SHORT_CYCLE_LARGE_WINDOW_DEFAULT           60
#define SHORT_CYCLE_LARGE_WINDOW_MIN                1
#define SHORT_CYCLE_LARGE_WINDOW_MAX             1440   // 24 hours

#define SHORT_CYCLE_LARGE_WINDOW_COUNT_DEFAULT      4   // Amount of short cycles allowed during large window
#define SHORT_CYCLE_LARGE_WINDOW_COUNT_MIN          1
#define SHORT_CYCLE_LARGE_WINDOW_COUNT_MAX          SHORT_CYCLE_SAMPLES

#define SHORT_CYCLE_RECOVERY_TIME_DEFAULT          30
#define SHORT_CYCLE_RECOVERY_TIME_MIN               1
#define SHORT_CYCLE_RECOVERY_TIME_MAX            1000

#endif // GLOBALS_H