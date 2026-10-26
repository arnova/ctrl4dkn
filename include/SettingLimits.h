#pragma once
#ifndef SETTING_LIMITS_H
#define SETTING_LIMITS_H

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

// Default for when (Daikin) secondary zone uses low temperature AWT range that is safe for floor-heated rooms
#define LOW_TEMP_SECONDARY_ZONE_DEFAULT         false

// Default for short cycle prevention (disable zone when short cycling is detected)
#define SHORT_CYCLE_PREVENTION_DEFAULT          true

#endif // SETTING_LIMITS_H
