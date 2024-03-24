#ifndef DAIKIN_CTRL_H
#define DAIKIN_CTRL_H

#include <elapsedMillis.h>
#include <PubSubClient.h>

#define MIN_P1P2_WRITE_INTERVAL   120   // Minutes

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
    void UpdateValveZoneRoom1Open(const bool bVal);
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

  private:
    PubSubClient* m_pMQTTClient;
    bool m_bDaikinPrimaryZoneOn = false;
    bool m_bDaikinSecondaryZoneOn = false;
    bool m_bPrimaryZoneValveClose = false;
    bool m_bPrimaryZoneProtection = false;
    elapsedMillis m_loopTimer = 0;
    elapsedMillis m_MQTTTimer = 0;
    elapsedMillis m_lastTempUpdateTimer = 999;
    uint16_t m_iSMDelayCounter = 0;
    uint16_t m_iPrimaryZoneDisableCounter = 0;
    uint16_t m_iPrimaryZoneProtectionCounter = 0;

    bool m_bCtrlEnable = true;
    bool m_bCtrlPriZoneHeating = false; //FIXME: Implement
    bool m_bCtrlSecZoneHeating = false;
    bool m_bCtrlRoom1Heating = false;
    bool m_bCtrlSecZoneForceHeating = false;
    bool m_bUpdateCtrlEnable = true;
    bool m_bUpdateCtrlPriZoneHeating = true;
    bool m_bUpdateCtrlSecZoneHeating = true;
    bool m_bUpdateCtrlRoom1Heating = true;

    float m_fZonePrimaryRealSetPoint = 0.0f;
    bool m_bUpdateZonePrimaryRealSetPoint = true;
    float m_fZonePrimarySavedSetPoint = 0.0f;
    bool m_bUpdateZonePrimarySavedSetPoint = true;
    bool m_bLeavingWaterTooHigh = false;
    bool m_bUpdateLeavingWaterTooHigh = true;
    bool m_bValveZonePrimaryOpen = true;
    bool m_bUpdateValveZonePrimaryOpen = true;
    bool m_bValveRoom1Open = true;
    bool m_bUpdateValveRoom1Open = true;
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