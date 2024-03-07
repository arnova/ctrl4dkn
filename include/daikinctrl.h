#ifndef DAIKIN_CTRL_H
#define DAIKIN_CTRL_H

#include <elapsedMillis.h>
#include <PubSubClient.h>

#define MIN_P1P2_WRITE_INTERVAL   120   // Minutes

class CDaikinCtrl
{
  public:
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
    bool PublishMQTTValues();
    void SetP1P2PrimaryZoneRoomTemp(const float& fVal) { m_fP1P2PrimaryZoneRoomTemp = fVal; };
    void SetCtrlPriZoneHeating(const uint8_t& iVal) { m_iCtrlPriZoneHeating = iVal; };
    void SetCtrlSecZoneHeating(const uint8_t& iVal) { m_iCtrlSecZoneHeating = iVal; };
    void SetP1P2LeavingWaterTemp(const float& fVal) { m_fP1P2LeavingWaterTemp = fVal; };
    void SetP1P2CompressorOn(const bool& bVal) { m_bP1P2CompressorOn = bVal; };
    void SetCtrlHysteresisHack(const bool& bVal) { m_bCtrlHysteresisHackOn = bVal; };
    void SetCtrlExtraZoneHeating(const uint8_t& iVal) { m_iCtrlExtraZoneHeating = iVal; };
    void SetP1P2CirculationPumpOn(const bool& bVal) { m_bP1P2CirculationPumpOn = bVal; };
    void SetP1P2HeatingOn(const bool& bVal) { m_bP1P2HeatingOn = bVal; };
    void SetP1P2ValveZoneMain(const bool& bVal) { m_bP1P2ValveZoneMain = bVal; };

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
    uint8_t m_iCtrlPriZoneHeating = 0;
    uint8_t m_iCtrlSecZoneHeating = 0;
    uint8_t m_iCtrlExtraZoneHeating = 0;
    bool m_bCtrlHysteresisHackOn = false;
    float m_fCtrlPrimaryZoneRealSetPoint = -1.0f; //DEFAULT_SET_POINT; // Default
};
#endif // DAIKIN_CTRL_H