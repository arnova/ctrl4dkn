#ifndef DAIKIN_CTRL_H
#define DAIKIN_CTRL_H

#include <elapsedMillis.h>
#include <PubSubClient.h>

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
#define MQTT_P1P2_P_PRIMARY_ZONE_ROOM_TEMPERATURE   MQTT_P1P2_P_PREFIX "T/1/Temperature_Room"

#define MQTT_CTRL4DKN_CTRL_PREFIX                   "ctrl4dkn/c/"
#define MQTT_CTRL4DKN_STATUS_PREFIX                 "ctrl4dkn/s/"

// Control topics
#define MQTT_CONTROLLER_ON_OFF                      "Controller Enable"
#define MQTT_ZONE_PRIMARY_ENABLE                    "Zone Primary Enable"
#define MQTT_ZONE_SECONDARY_ENABLE                  "Zone Secondary Enable"
#define MQTT_ZONE_SECONDARY_FORCE                   "Zone Secondary Force"
#define MQTT_DAIKIN_SECONDARY_FORCE                 "Daikin Secondary Force"
#define MQTT_ROOM1_ENABLE                           "Room 1 Enable"
#define MQTT_ROOM2_ENABLE                           "Room 2 Enable"
#define MQTT_ROOM3_ENABLE                           "Room 3 Enable"
#define MQTT_ROOM4_ENABLE                           "Room 4 Enable"
#define MQTT_GAS_ONLY                               "Gas Only"

// Status topics
#define MQTT_FW_VERSION                             "Firmware Version"
#define MQTT_VALVE_ZONE_PRIMARY_OPEN                "Valve Zone Primary Open"
#define MQTT_VALVE_ROOM1_OPEN                       "Valve Room 1 Open"
#define MQTT_VALVE_ROOM2_OPEN                       "Valve Room 2 Open"
#define MQTT_VALVE_ROOM3_OPEN                       "Valve Room 3 Open"
#define MQTT_VALVE_ROOM4_OPEN                       "Valve Room 4 Open"
#define MQTT_DAIKIN_ZONE_PRIMARY_ENABLE             "Daikin Zone Primary Enable"
#define MQTT_DAIKIN_ZONE_SECONDARY_ENABLE           "Daikin Zone Secondary Enable"
#define MQTT_LEAVING_WATER_TOO_HIGH                 "Leaving Water Too High"

#define DAIKIN_HYSTERESIS                     1.0f
#define LEAVING_WATER_MAX                    45.0f
#define LEAVING_WATER_MAX_MARGIN              3.0f
#define DAIKIN_ZONE_SWITCH_TIME                 5   // Seconds
#define PRIMARY_ZONE_DISABLE_TIME             300   // Seconds = 5 minutes
#define PRIMARY_ZONE_VALVE_DELAY              180   // Seconds = 3 minutes
#define FLOOR_PROTECTION_DELAY                300   // Seconds = 5 minutes

#define CONTROL_LOOP_TIME                       1   // Seconds
#define MQTT_UPDATE_TIME                        1   // Seconds


class CDaikinCtrl
{
  public:
    typedef enum HAConfigType_e
    {
      SWITCH,
      SENSOR,
      BINARY_SENSOR
    } HAConfigType_t;

    // Statemachine states
    enum e_sm_state
    {
      STATE_IDLE,
      STATE_WAIT_STATE,
      STATE_PRIMARY_VALVE_CLOSING,
      STATE_SECONDARY_ZONE_ON,
      STATE_DAIKIN_SEC_ZONE_DISABLE
    };

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
    bool MQTTPublishValues();

    void SetP1P2PrimaryZoneRoomTemp(const float& fVal) { m_fP1P2PrimaryZoneRoomTemp = fVal; };
    void SetP1P2PrimaryZoneTargetTemp(const float& fVal) { m_fP1P2PrimaryZoneTargetTemp = fVal; };
    void SetP1P2LeavingWaterTemp(const float& fVal) { m_fP1P2LeavingWaterTemp = fVal; };
    void SetP1P2CirculationPumpOn(const bool& bVal) { m_bP1P2CirculationPumpOn = bVal; };
    void SetP1P2HeatingOn(const bool& bVal) { m_bP1P2HeatingOn = bVal; };
    void SetP1P2CoolingOn(const bool& bVal) { m_bP1P2CoolingOn = bVal; };
    void SetP1P2ValveZoneMain(const bool& bVal) { m_bP1P2ValveZoneMain = bVal; };

//    bool IsDaikinHeatingActive() { return m_bP1P2HeatingOn && m_bP1P2CirculationPumpOn; };
//    bool IsDaikinCoolingActive() { return m_bP1P2CoolingOn && m_bP1P2CirculationPumpOn; };
    bool IsDaikinActive() { return m_bP1P2CirculationPumpOn; };
//    bool IsPrimaryHeatingActive() { return IsDaikinHeatingActive() && m_bP1P2ValveZoneMain; };

    void SetCtrlOnOff(const bool& bVal) { m_bCtrlEnable = bVal; m_bUpdateCtrlEnable = true; };
    void SetCtrlZonePriEnable(const bool& bVal) { m_bCtrlZonePriEnable = bVal; m_bUpdateCtrlZonePriEnable = true; };
    void SetCtrlZoneSecEnable(const bool& bVal) { m_bCtrlZoneSecEnable = bVal; m_bUpdateCtrlZoneSecEnable = true; };
    void SetCtrlZoneSecForce(const bool& bVal) { m_bCtrlZoneSecForce = bVal; m_bUpdateCtrlZoneSecForce = true; };
    void SetCtrlDaikinSecForce(const bool& bVal) { m_bCtrlDaikinSecForce = bVal; m_bUpdateCtrlDaikinSecForce = true; };
    void SetCtrlGasOnly(const bool& bVal) { m_bCtrlGasOnly = bVal; m_bUpdateCtrlGasOnly = true; };
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
    bool m_bPrimaryZoneRequiresHeating = false;
    bool m_bPrimaryZoneValveClose = false;
    bool m_bFloorProtection = false;
    bool m_bRoom1ValveOpenRq = false;
    bool m_bRoom2ValveOpenRq = false;
    bool m_bRoom3ValveOpenRq = false;
    bool m_bRoom4ValveOpenRq = false;
    elapsedMillis m_loopTimer = 0;
    elapsedMillis m_MQTTTimer = 0;
    uint16_t m_iSMDelayCounter = 0;
    uint16_t m_iPrimaryZoneNoHeatCounter = 0;
    uint16_t m_iFloorProtectionCounter = 0;

    bool m_bCtrlEnable = true;
    bool m_bCtrlZonePriEnable = false;
    bool m_bCtrlZoneSecEnable = false;
    bool m_bCtrlZoneSecForce = false;
    bool m_bCtrlDaikinSecForce = false;
    bool m_bCtrlGasOnly = false;
    bool m_bCtrlRoom1Enable = false;
    bool m_bCtrlRoom2Enable = false;
    bool m_bCtrlRoom3Enable = false;
    bool m_bCtrlRoom4Enable = false;

    bool m_bUpdateCtrlEnable = true;
    bool m_bUpdateCtrlZonePriEnable = true;
    bool m_bUpdateCtrlZoneSecEnable = true;
    bool m_bUpdateCtrlZoneSecForce = true;
    bool m_bUpdateCtrlDaikinSecForce = true;
    bool m_bUpdateCtrlGasOnly = true;
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

    float m_fP1P2LeavingWaterTemp = -1.0f;
    float m_fP1P2PrimaryZoneRoomTemp = -1.0f;       // Room temperature reported by P1P2/thermostat
    float m_fP1P2PrimaryZoneTargetTemp = -1.0f;     // Actual target temperature set on P1P2/thermostat

    bool m_bP1P2CompressorOn = false;
    bool m_bP1P2CirculationPumpOn = false;
    bool m_bP1P2HeatingOn = false;
    bool m_bP1P2CoolingOn = false;
    bool m_bP1P2ValveZoneMain = false;

    e_sm_state m_iState = STATE_WAIT_STATE;
};
#endif // DAIKIN_CTRL_H