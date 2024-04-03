#ifndef DAIKIN_CTRL_H
#define DAIKIN_CTRL_H

#include <elapsedMillis.h>
#include <PubSubClient.h>

#include "system.h"

#define MQTT_MAX_SIZE 1024

#define MIN_P1P2_WRITE_INTERVAL   120   // Minutes

#define MQTT_P1P2_P_PREFIX                          "P1P2/P/" MQTT_P1P2_HOST_ID "/"
#define MQTT_P1P2_W_TOPIC                           "P1P2/W/" MQTT_P1P2_HOST_ID "/"  // FIXME: HOST_ID ?

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

#define MQTT_CTRL4DKN_CTRL_PREFIX                   "ctrl4dkn/c/"
#define MQTT_CTRL4DKN_STATUS_PREFIX                 "ctrl4dkn/s/"

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
#define FLOOR_PROTECTION_DELAY                5     // Minutes

#define CONTROL_LOOP_TIME                    60     // Seconds
#define MQTT_UPDATE_TIME                      1     // Seconds


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
      STATE_PRIMARY_ZONE_CLOSE,
      STATE_PRIMARY_VALVE_CLOSE,
      STATE_SECONDARY_ZONE_ON,
      STATE_DAIKIN_SEC_ZONE_DISABLE,
      STATE_PRIMARY_VALVE_OPEN
    };

    CDaikinCtrl(PubSubClient* MQTTClient);

    void loop();

    static bool Float2HexStr(const float& fVal, char* strVal);
    static bool Float2Str(const float& fVal, char* strVal);

    void UpdateZonePrimaryRealSetPoint(const float fSetPoint);
    void UpdateZonePrimarySavedSetPoint(const float fSetPoint);
    void UpdateLeavingWaterTooHigh(const bool bVal);
    void UpdateValveZonePrimaryOpen(const bool bVal);
    void UpdateRoom1ValveOpen(const bool bVal);
    void UpdateRoom2ValveOpen(const bool bVal);
    void UpdateRoom3ValveOpen(const bool bVal);
    void UpdateDaikinZonePrimaryEnable(const bool bVal);
    void UpdateDaikinZoneSecondaryEnable(const bool bVal);
    bool MQTTPublishValues();

    bool P1P2SetTargetTemp(const float& fTemp);
    bool UpdateDaikinTargetTemperature(float fNewVal);

    void SetP1P2PrimaryZoneRoomTemp(const float& fVal) { m_fP1P2PrimaryZoneRoomTemp = fVal; };
    void SetP1P2PrimaryZoneTargetTemp(const float& fVal) { m_fP1P2PrimaryZoneTargetTemp = fVal; };
    void SetP1P2LeavingWaterTemp(const float& fVal) { m_fP1P2LeavingWaterTemp = fVal; };
    void SetP1P2CompressorOn(const bool& bVal) { m_bP1P2CompressorOn = bVal; };
    void SetP1P2CirculationPumpOn(const bool& bVal) { m_bP1P2CirculationPumpOn = bVal; };
    void SetP1P2HeatingOn(const bool& bVal) { m_bP1P2HeatingOn = bVal; };
    void SetP1P2ValveZoneMain(const bool& bVal) { m_bP1P2ValveZoneMain = bVal; };

    bool IsHeatingActive() { return m_bP1P2HeatingOn && m_bP1P2CirculationPumpOn; };
    bool IsPrimaryHeatingActive() { return IsHeatingActive() && m_bP1P2ValveZoneMain; };

    void SetCtrlOnOff(const bool& bVal) { m_bCtrlEnable = bVal; m_bUpdateCtrlEnable = true; };
    void SetCtrlPriZoneHeating(const bool& bVal) { m_bCtrlPriZoneHeating = bVal; m_bUpdateCtrlPriZoneHeating = true; };
    void SetCtrlSecZoneHeating(const bool& bVal) { m_bCtrlSecZoneHeating = bVal; m_bUpdateCtrlSecZoneHeating = true; };
    void SetCtrlRoom1Heating(const bool& bVal) { m_bCtrlRoom1Heating = bVal; m_bUpdateCtrlRoom1Heating = true; };
    void SetCtrlRoom2Heating(const bool& bVal) { m_bCtrlRoom2Heating = bVal; m_bUpdateCtrlRoom2Heating = true; };
    void SetCtrlRoom3Heating(const bool& bVal) { m_bCtrlRoom3Heating = bVal; m_bUpdateCtrlRoom3Heating = true; };

  private:
    PubSubClient* m_pMQTTClient;
    bool m_bDaikinPrimaryZoneOn = false;
    bool m_bDaikinSecondaryZoneOn = false;
    bool m_bPrimaryZoneValveClose = false;
    bool m_bFloorProtection = false;
    elapsedMillis m_loopTimer = 0;
    elapsedMillis m_MQTTTimer = 0;
    elapsedMillis m_lastTempUpdateTimer = 999;
    uint16_t m_iSMDelayCounter = 0;
    uint16_t m_iPrimaryZoneDisableCounter = 0;
    uint16_t m_iFloorProtectionCounter = 0;

    bool m_bCtrlEnable = true;
    bool m_bCtrlPriZoneHeating = false;
    bool m_bCtrlSecZoneHeating = false;
    bool m_bCtrlRoom1Heating = false;
    bool m_bCtrlRoom2Heating = false;
    bool m_bCtrlRoom3Heating = false;
    bool m_bCtrlSecZoneForceHeating = false; // FIXME: Implement
    bool m_bUpdateCtrlEnable = true;
    bool m_bUpdateCtrlPriZoneHeating = true;
    bool m_bUpdateCtrlSecZoneHeating = true;
    bool m_bUpdateCtrlRoom1Heating = true;
    bool m_bUpdateCtrlRoom2Heating = true;
    bool m_bUpdateCtrlRoom3Heating = true;

    float m_fZonePrimaryRealSetPoint = 0.0f;
    bool m_bUpdateZonePrimaryRealSetPoint = true;
    float m_fZonePrimarySavedSetPoint = 0.0f;
    bool m_bUpdateZonePrimarySavedSetPoint = true;
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
    bool m_bDaikinZonePrimaryEnable = false;
    bool m_bUpdateDaikinZonePrimaryEnable = true;
    bool m_bDaikinZoneSecondaryEnable = false;
    bool m_bUpdateDaikinZoneSecondaryEnable = true;

    float m_fP1P2LeavingWaterTemp = -1.0f;
    float m_fP1P2PrimaryZoneRoomTemp = -1.0f;       // Room temperature reported by P1P2/thermostat
    float m_fP1P2PrimaryZoneTargetTemp = -1.0f;     // Actual target temperature set on P1P2/thermostat
    float m_fP1P2PrimaryZoneTargetTempSave = -1.0f;   // (Temporary) saved value for target temperature for P1P2/thermostat

    float m_fCtrlPrimaryZoneRealSetPoint = -1.0f; //DEFAULT_SET_POINT; // Default

    bool m_bP1P2CompressorOn = false;
    bool m_bP1P2CirculationPumpOn = false;
    bool m_bP1P2HeatingOn = false;
    bool m_bP1P2ValveZoneMain = false;

    e_sm_state m_iState = STATE_WAIT_STATE;
};
#endif // DAIKIN_CTRL_H