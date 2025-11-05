#ifndef DAIKIN_CTRL_H
#define DAIKIN_CTRL_H

#include <elapsedMillis.h>
#include <PubSubClient.h>

#include "RollingAverage.h"
#include "system.h"

#define MQTT_MAX_SIZE 1024

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

#define MQTT_CTRL4DKN_CTRL_PREFIX                   "ctrl4dkn/c/"
#define MQTT_CTRL4DKN_STATUS_PREFIX                 "ctrl4dkn/s/"

// Control topics
#define MQTT_CONTROLLER_ON_OFF                      "Controller Enable"
#define MQTT_ZONE_PRIMARY_ENABLE                    "Zone Primary Enable"
#define MQTT_ZONE_SECONDARY_ENABLE                  "Zone Secondary Enable"
#define MQTT_ZONE_SECONDARY_FORCE                   "Zone Secondary Force"
#define MQTT_VALVE_PRIMARY_CLOSE_FORCE              "Valve Primary Close Force"
#define MQTT_DAIKIN_DISABLE                         "Daikin Disable"
#define MQTT_DAIKIN_SECONDARY_ENABLE                "Daikin Secondary Enable"
#define MQTT_DAIKIN_SECONDARY_FORCE                 "Daikin Secondary Force"
#define MQTT_ROOM1_ENABLE                           "Room 1 Enable"
#define MQTT_ROOM2_ENABLE                           "Room 2 Enable"
#define MQTT_ROOM3_ENABLE                           "Room 3 Enable"
#define MQTT_ROOM4_ENABLE                           "Room 4 Enable"
#define MQTT_GAS_ONLY                               "Gas Only"
#define MQTT_SHORT_CYCLE_PREVENT                    "Short Cycle Prevention"

// Status topics
#define MQTT_FW_VERSION                             "Firmware Version"
#define MQTT_VALVE_ZONE_PRIMARY_OPEN                "Valve Zone Primary Open"
#define MQTT_VALVE_ROOM1_OPEN                       "Valve Room 1 Open"
#define MQTT_VALVE_ROOM2_OPEN                       "Valve Room 2 Open"
#define MQTT_VALVE_ROOM3_OPEN                       "Valve Room 3 Open"
#define MQTT_VALVE_ROOM4_OPEN                       "Valve Room 4 Open"
#define MQTT_DAIKIN_PRIMARY_ENABLED                 "Daikin Primary Enabled"
#define MQTT_DAIKIN_SECONDARY_ENABLED               "Daikin Secondary Enabled"
#define MQTT_LEAVING_WATER_TOO_HIGH                 "Leaving Water Too High"
#define MQTT_AVG_ROOM_TEMPERATURE                   "Temperature Room Averaged"
#define MQTT_ZONE_PRIMARY_REQUIRES_HEATING          "Zone Primary Requires Heating"
#define MQTT_SHORT_CYCLE_PRIMARY_DETECTED           "Zone Primary Short Cycle Detected"
#define MQTT_SHORT_CYCLE_SECONDARY_DETECTED         "Zone Secondary Short Cycle Detected"

#define DAIKIN_HYSTERESIS                     1.0f
#define LEAVING_WATER_MAX                    45.0f
#define LEAVING_WATER_MAX_MARGIN              3.0f
#define DAIKIN_ZONE_SWITCH_TIME                 5   // Seconds
#define PRIMARY_ZONE_ROOM_TEMP_AVG_TIME      1800   // Seconds = 30 minutes
#define PRIMARY_ZONE_VALVE_DELAY              180   // Seconds = 3 minutes
#define FLOOR_PROTECTION_DELAY                300   // Seconds = 5 minutes
#define DAIKIN_ACTIVE_OFF_TIME                900   // Seconds = 15 minutes
#define DAIKIN_STARTUP_TIME                    30   // Seconds
#define LWT_COOL_DOWN_DELAY_TIME              300   // Seconds = 5 minutes
#define WATCHDOG_TIMEOUT_TIME                 900   // Seconds = 15 minutes
#define WATCHDOG_RECOVERY_TIME                900   // Seconds = 15 minutes

#define CONTROL_LOOP_TIME                       1   // Seconds
#define MQTT_UPDATE_TIME                        1   // Seconds

#define SHORT_CYCLE_SAMPLES                     5
class CDaikinCtrl
{
  public:
    typedef enum ha_config_type_e
    {
      SWITCH,
      TEMP_SENSOR,
      BINARY_SENSOR
    } ha_config_type_t;

    // Statemachine states
    enum sm_state_e
    {
      STATE_IDLE,
      STATE_WAIT_STATE,
      STATE_PRIMARY_VALVE_DELAY,
      STATE_DAIKIN_ZONE_SWITCH_DELAY,
      STATE_LWT_COOL_DOWN_DELAY,
      STATE_DELAY_WAIT
    };

    typedef struct short_cycle_e
    {
      int32_t iShortCycleTimeStamps[SHORT_CYCLE_SAMPLES] = { -1 };
      uint32_t iShortCycleRecoveryCounter = 0;
    } short_cycle_t;
  
    CDaikinCtrl(PubSubClient& MQTTClient);

    void loop();

    static bool Float2HexStr(const float& fVal, char* strVal);
    static bool Float2Str(const float& fVal, char* strVal);

    void UpdateLeavingWaterTooHigh(const bool bVal);
    void UpdateValveZonePrimaryOpen(const bool bVal);
    void UpdateRoom1ValveOpen(const bool bVal);
    void UpdateRoom2ValveOpen(const bool bVal);
    void UpdateRoom3ValveOpen(const bool bVal);
    void UpdateRoom4ValveOpen(const bool bVal);
    void UpdateDaikinZonePrimaryEnable(const bool bVal);
    void UpdateDaikinZoneSecondaryEnable(const bool bVal);
    void UpdateAveragePrimaryZoneRoomTemp(const float fVal);
    void UpdateZonePrimaryRequiresHeating(const bool bVal);
    bool MQTTPublishValues();

    void TriggerWatchdog() { m_iWatchdogCounter = 0; };

    void SetP1P2AlthermaOn(const bool& bVal) { m_bP1P2AlthermaOn = bVal; };
    void SetP1P2PrimaryZoneRoomTemp(const float& fVal) { m_fP1P2PrimaryZoneRoomTemp = fVal; };
    void SetP1P2PrimaryZoneTargetTemp(const float& fVal) { m_fP1P2PrimaryZoneTargetTemp = fVal; };
    void SetP1P2LeavingWaterTemp(const float& fVal) { m_fP1P2LeavingWaterTemp = fVal; };
    void SetP1P2CirculationPumpOn(const bool& bVal) { m_bP1P2CirculationPumpOn = bVal; };
    void SetP1P2HeatingOn(const bool& bVal) { m_bP1P2HeatingOn = bVal; };
    void SetP1P2CoolingOn(const bool& bVal) { m_bP1P2CoolingOn = bVal; };
    void SetP1P2ValveZoneMain(const bool& bVal) { m_bP1P2ValveZoneMain = bVal; };
    void SetP1P2DefrostActive(const bool& bVal) { m_bP1P2DefrostActive = bVal; };
    void SetP1P2Compressor(const bool& bVal) { m_bP1P2Compressor = bVal; };
    
//    bool IsDaikinHeatingActive() { return m_bP1P2HeatingOn && m_bP1P2CirculationPumpOn; };
//    bool IsDaikinCoolingActive() { return m_bP1P2CoolingOn && m_bP1P2CirculationPumpOn; };
//    bool IsPrimaryHeatingActive() { return IsDaikinHeatingActive() && m_bP1P2ValveZoneMain; };

    void SetCtrlOnOff(const bool& bVal) { m_bCtrlEnable = bVal; m_bUpdateCtrlEnable = true; };
    void SetCtrlZonePriEnable(const bool& bVal) { m_bCtrlZonePriEnable = bVal; m_bUpdateCtrlZonePriEnable = true; };
    void SetCtrlZoneSecEnable(const bool& bVal) { m_bCtrlZoneSecEnable = bVal; m_bUpdateCtrlZoneSecEnable = true; };
    void SetCtrlZoneSecForce(const bool& bVal) { m_bCtrlZoneSecForce = bVal; m_bUpdateCtrlZoneSecForce = true; };
    void SetCtrlValvePriCloseForce(const bool& bVal) { m_bCtrlValvePriCloseForce = bVal; m_bUpdateCtrlValvePriCloseForce = true; };
    void SetCtrlDaikinDisable(const bool& bVal) { m_bCtrlDaikinDisable = bVal; m_bUpdateCtrlDaikinDisable = true; };
    void SetCtrlDaikinSecEnable(const bool& bVal) { m_bCtrlDaikinSecEnable = bVal; m_bUpdateCtrlDaikinSecEnable = true; };
    void SetCtrlDaikinSecForce(const bool& bVal) { m_bCtrlDaikinSecForce = bVal; m_bUpdateCtrlDaikinSecForce = true; };
    void SetCtrlGasOnly(const bool& bVal) { m_bCtrlGasOnly = bVal; m_bUpdateCtrlGasOnly = true; };
    void SetCtrlShortCyclePrevention(const bool& bVal) { m_bCtrlShortCyclePrevention = bVal; m_bUpdateCtrlShortCyclePrevention = true; };
    void SetCtrlRoom1Enable(const bool& bVal) { m_bCtrlRoom1Enable = bVal; m_bUpdateCtrlRoom1Enable = true; };
    void SetCtrlRoom2Enable(const bool& bVal) { m_bCtrlRoom2Enable = bVal; m_bUpdateCtrlRoom2Enable = true; };
    void SetCtrlRoom3Enable(const bool& bVal) { m_bCtrlRoom3Enable = bVal; m_bUpdateCtrlRoom3Enable = true; };
    void SetCtrlRoom4Enable(const bool& bVal) { m_bCtrlRoom4Enable = bVal; m_bUpdateCtrlRoom4Enable = true; };

  private:
    void StateMachine();
    void UpdateRelays();

    PubSubClient* m_pMQTTClient;
    bool m_bDaikinPrimaryZoneOn = false;
    bool m_bDaikinSecondaryZoneOn = false;
    bool m_bPrimaryZoneRequiresHeating = true;
    bool m_bPrimaryZoneValveClose = false;
    bool m_bFloorProtection = false;
    bool m_bDaikinActive = false;
    bool m_bRoom1ValveOpenRq = false;
    bool m_bRoom2ValveOpenRq = false;
    bool m_bRoom3ValveOpenRq = false;
    bool m_bRoom4ValveOpenRq = false;
    elapsedMillis m_loopTimer = 0;
    elapsedMillis m_MQTTTimer = 0;
    uint16_t m_iSMDelayCounter = 0;
    uint16_t m_iFloorProtectionCounter = 0;
    uint16_t m_iDaikinActiveOffCounter = 0;
    uint16_t m_iDaikinStartupCounter = 0;
    uint16_t m_iWatchdogCounter = 0;
    uint16_t m_iWatchdogRecoveryCounter = 0;
    CRollingAverage m_roomTempRollingAverager;

    bool m_bCtrlEnable = true;
    bool m_bCtrlZonePriEnable = false;
    bool m_bCtrlZoneSecEnable = false;
    bool m_bCtrlZoneSecForce = false;
    bool m_bCtrlValvePriCloseForce = false;
    bool m_bCtrlDaikinDisable = false;
    bool m_bCtrlDaikinSecEnable = false;
    bool m_bCtrlDaikinSecForce = false;
    bool m_bCtrlGasOnly = false;
    bool m_bCtrlShortCyclePrevention = false;
    bool m_bCtrlRoom1Enable = false;
    bool m_bCtrlRoom2Enable = false;
    bool m_bCtrlRoom3Enable = false;
    bool m_bCtrlRoom4Enable = false;

    bool m_bUpdateCtrlEnable = true;
    bool m_bUpdateCtrlZonePriEnable = true;
    bool m_bUpdateCtrlZoneSecEnable = true;
    bool m_bUpdateCtrlZoneSecForce = true;
    bool m_bUpdateCtrlValvePriCloseForce = true;
    bool m_bUpdateCtrlDaikinDisable = true;
    bool m_bUpdateCtrlDaikinSecEnable = true;
    bool m_bUpdateCtrlDaikinSecForce = true;
    bool m_bUpdateCtrlGasOnly = true;
    bool m_bUpdateCtrlShortCyclePrevention = true;
    bool m_bUpdateCtrlRoom1Enable = true;
    bool m_bUpdateCtrlRoom2Enable = true;
    bool m_bUpdateCtrlRoom3Enable = true;
    bool m_bUpdateCtrlRoom4Enable = true;

    bool m_bLeavingWaterTooHigh = false;
    bool m_bUpdateLeavingWaterTooHigh = true;
    bool m_bValveZonePrimaryOpen = true;
    bool m_bUpdateValveZonePrimaryOpen = true;
    bool m_bRoom1ValveOpen = true;
    bool m_bUpdateRoom1ValveOpen = true;
    bool m_bRoom2ValveOpen = true;
    bool m_bUpdateRoom2ValveOpen = true;
    bool m_bRoom3ValveOpen = true;
    bool m_bUpdateRoom3ValveOpen = true;
    bool m_bRoom4ValveOpen = true;
    bool m_bUpdateRoom4ValveOpen = true;
    bool m_bDaikinZonePrimaryEnable = false;
    bool m_bUpdateDaikinZonePrimaryEnable = true;
    bool m_bDaikinZoneSecondaryEnable = false;
    bool m_bUpdateDaikinZoneSecondaryEnable = true;
    char m_strAveragePrimaryZoneRoomTemp[5] = "";
    bool m_bUpdateAveragePrimaryZoneRoomTemp = true;
    bool m_bZonePrimaryRequiresHeating = false;
    bool m_bUpdateZonePrimaryRequiresHeating = true;

    float m_fP1P2LeavingWaterTemp = -1.0f;
    float m_fP1P2PrimaryZoneRoomTemp = -1.0f;       // Room temperature reported by P1P2/thermostat
    float m_fP1P2PrimaryZoneTargetTemp = -1.0f;     // Actual target temperature set on P1P2/thermostat

    bool m_bP1P2AlthermaOn = false;
    bool m_bP1P2CompressorOn = false;
    bool m_bP1P2CirculationPumpOn = false;
    bool m_bP1P2HeatingOn = false;
    bool m_bP1P2CoolingOn = false;
    bool m_bP1P2ValveZoneMain = false;
    bool m_bP1P2ValveZoneMainLast = false;
    bool m_bAlthermaOn = false;
    bool m_bP1P2DefrostActive = false;
    bool m_bP1P2Compressor = false;

    short_cycle_t m_ShortCyclePrimary = { };
    short_cycle_t m_ShortCycleSecondary = { };

    sm_state_e m_iState = STATE_WAIT_STATE;
};
#endif // DAIKIN_CTRL_H