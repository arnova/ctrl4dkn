#pragma once
#ifndef MQTT_TOPICS_H
#define MQTT_TOPICS_H

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

// Home Assistant birth/last will topic (HA publishes "online" here after it (re)started)
#define MQTT_HA_STATUS_TOPIC                    "homeassistant/status"

// Select the appropriate one below (depends on P1P2Serial/bridge firmware) 
//#define MQTT_P1P2_HOST_ID "066"               // v0.9.44 or older. NOTE: Need to customize this
#define MQTT_P1P2_HOST_ID "P1P2MQTT/bridge0"    // v0.9.45 or newer. NOTE: *May* need to customize this

// Enable below when a bridge with older firmware (<v0.9.45) is used
//#define MQTT_P1P2_OLD_BRIDGE // FIXME: Setting

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

#endif // MQTT_TOPICS_H
