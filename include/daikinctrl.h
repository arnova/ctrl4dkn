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

    CDaikinCtrl(PubSubClient* MQTTClient);

    void loop();

    static bool Float2HexStr(const float& fVal, char* strVal);
    static bool Float2Str(const float& fVal, char* strVal);

    void UpdateZonePrimaryRealSetPoint(const float fSetPoint);
    void UpdateZonePrimarySavedSetPoint(const float fSetPoint);
    void UpdateLeavingWaterTooHigh(const bool bVal);
    void UpdateValveZonePrimaryOpen(const bool bVal);
    void UpdateValveZoneExtraOpen(const bool bVal);
    void UpdateDaikinZonePrimaryEnable(const bool bVal);
    void UpdateDaikinZoneSecondaryEnable(const bool bVal);
    bool MQTTPublishValues();

    bool P1P2SetTargetTemp(const float& fTemp);
    bool UpdateDaikinTargetTemperature(float fNewVal);

    void SetP1P2PrimaryZoneRoomTemp(const float& fVal) { m_fP1P2PrimaryZoneRoomTemp = fVal; };
    void SetP1P2LeavingWaterTemp(const float& fVal) { m_fP1P2LeavingWaterTemp = fVal; };
    void SetP1P2CompressorOn(const bool& bVal) { m_bP1P2CompressorOn = bVal; };
    void SetP1P2CirculationPumpOn(const bool& bVal) { m_bP1P2CirculationPumpOn = bVal; };
    void SetP1P2HeatingOn(const bool& bVal) { m_bP1P2HeatingOn = bVal; };
    void SetP1P2ValveZoneMain(const bool& bVal) { m_bP1P2ValveZoneMain = bVal; };

    bool IsPrimaryHeatingActive() { return m_bP1P2HeatingOn && m_bP1P2CirculationPumpOn && m_bP1P2ValveZoneMain; }; // FIXME: This needs changing when secondary heating curve is used
    bool IsHeatingActive() { return m_bP1P2HeatingOn && m_bP1P2CirculationPumpOn; }; // FIXME: This needs changing when secondary heating curve is used

    void SetCtrlPriZoneHeating(const bool& bVal) { m_bCtrlPriZoneHeating = bVal; m_bUpdateCtrlPriZoneHeating = true; };
    void SetCtrlSecZoneHeating(const bool& bVal) { m_bCtrlSecZoneHeating = bVal; m_bUpdateCtrlSecZoneHeating = true; };
    void SetCtrlExtraZoneHeating(const bool& bVal) { m_bCtrlExtraZoneHeating = bVal; m_bUpdateCtrlExtraZoneHeating = true; };

  private:
    PubSubClient* m_pMQTTClient;
    bool m_bDaikinPrimaryZoneOn = false;
    bool m_bDaikinSecondaryZoneOn = false;
    bool m_bSecZoneOnly = false;
    bool m_bPrimaryZoneProtection = false;
    elapsedMillis m_loopTimer = 0;
    elapsedMillis m_MQTTTimer = 0;
    elapsedMillis m_lastTempUpdateTimer = 999;
    uint16_t m_iPrimaryZoneDisableCounter = 0;
    uint16_t m_iPrimaryZoneValveCounter = 0;
    uint16_t m_iPrimaryZoneProtectionCounter = 0;

    bool m_bCtrlPriZoneHeating = true;
    bool m_bCtrlSecZoneHeating = false;
    bool m_bCtrlExtraZoneHeating = false;
    bool m_bCtrlSecZoneForceHeating = false; // FIXME: Implement
    bool m_bUpdateCtrlPriZoneHeating = true;
    bool m_bUpdateCtrlSecZoneHeating = true;
    bool m_bUpdateCtrlExtraZoneHeating = true;

    float m_fZonePrimaryRealSetPoint = 0.0f;
    bool m_bUpdateZonePrimaryRealSetPoint = true;
    float m_fZonePrimarySavedSetPoint = 0.0f;
    bool m_bUpdateZonePrimarySavedSetPoint = true;
    bool m_bLeavingWaterTooHigh = false;
    bool m_bUpdateLeavingWaterTooHigh = true;
    bool m_bValveZonePrimaryOpen = true;
    bool m_bUpdateValveZonePrimaryOpen = true;
    bool m_bValveZoneExtraOpen = true;
    bool m_bUpdateValveZoneExtraOpen = true;
    bool m_bDaikinZonePrimaryEnable = false;
    bool m_bUpdateDaikinZonePrimaryEnable = false;
    bool m_bDaikinZoneSecondaryEnable = false;
    bool m_bUpdateDaikinZoneSecondaryEnable = true;

    float m_fP1P2PrimaryZoneRoomTemp = -1.0f;       // (User requested) room temperature from P1P2/thermostat
    float m_fP1P2PrimaryZoneTargetTemp = -1.0f;     // Actual target temperature on Daikin read by P1P2. FIXME: Never updated
    float m_fP1P2PrimaryZoneRoomTempSave = -1.0f;   // (Temporary) saved value for room temperature from P1P2/thermostat
    float m_fP1P2LeavingWaterTemp = -1.0f;
    bool m_bP1P2CompressorOn = false;
    bool m_bP1P2CirculationPumpOn = false;
    bool m_bP1P2HeatingOn = false;
    bool m_bP1P2ValveZoneMain = false;
    float m_fCtrlPrimaryZoneRealSetPoint = -1.0f; //DEFAULT_SET_POINT; // Default
};
#endif // DAIKIN_CTRL_H