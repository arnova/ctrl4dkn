#ifndef DAIKIN_CTRL_H
#define DAIKIN_CTRL_H

#include <elapsedMillis.h>

#include "globals.h"
#include "system.h"
#include "Uptime.h"
#include "Network.h"
#include "Terminal.h"
#include "Ctrl4DknCommandHandler.h"
#include "RollingAverage.h"
#include "ShortCycle.h"

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

    CDaikinCtrl(CNetwork& network); // Ctor

    static bool Float2HexStr(const float& fVal, char* strVal);
    static bool Float2Str(const float& fVal, char* strVal);

    void Loop();
    void Reset();

    void MqttOnConnect();
    void MqttRequestRepublish() { m_bMqttRepublish = true; }; // Publish all values on next loop
    void LoadSettings();
    void FactoryReset();

    void TriggerWatchdog() { m_iNetworkWatchdogCounter = 0; };
    void ShortCycleHandling();

    void UpdateLeavingWaterTooHigh(const bool bVal);
    void UpdateValveZonePrimaryOpen(const bool bVal);
    void UpdateRoom1ValveOpen(const bool bVal);
    void UpdateRoom2ValveOpen(const bool bVal);
    void UpdateRoom3ValveOpen(const bool bVal);
    void UpdateRoom4ValveOpen(const bool bVal);
    void UpdateDaikinZonePrimaryEnable(const bool bVal);
    void UpdateDaikinZoneSecondaryEnable(const bool bVal);
    void UpdateAveragePrimaryZoneRoomTemp(const float fVal);
    void UpdateZonePrimaryShortCycleDetected(const bool bVal);
    void UpdateZoneSecondaryShortCycleDetected(const bool bVal);

    void SetMqttUpdateInterval(const uint8_t iInterval);
    void SetNetWatchDogTimeout(const uint16_t iTimeout);
    void SetNetWatchDogRecovery(const uint16_t iTimeout);
    void SetLowTempSecondaryZone(const bool bVal);
    void SetShortCyclePrevention(const bool bVal);

    // Expert settings
    void SetLeavingWaterMax(const float fVal);
    void SetPrimaryZoneValveDelay(const uint16_t iTime);
    void SetFloorProtectionDelay(const uint16_t iTime);
    void SetLwtCoolDownDelay(const uint16_t iTime);

    void SetP1P2AlthermaOn(const bool& bVal) { m_bP1P2AlthermaOn = bVal; };
    void SetP1P2PrimaryZoneRoomTemp(const float& fVal) { m_fP1P2PrimaryZoneRoomTemp = fVal; };
    void SetP1P2PrimaryZoneTargetTemp(const float& fVal) { m_fP1P2PrimaryZoneTargetTemp = fVal; };
    void SetP1P2LeavingWaterTemp(const float& fVal) { m_fP1P2LeavingWaterTemp = fVal; };
    void SetP1P2CirculationPumpOn(const bool& bVal) { m_bP1P2CirculationPumpOn = bVal; };
    void SetP1P2HeatingOn(const bool& bVal) { m_bP1P2HeatingOn = bVal; };
    void SetP1P2CoolingOn(const bool& bVal) { m_bP1P2CoolingOn = bVal; };
    void SetP1P2ValveZoneMain(const bool& bVal) { m_bP1P2ValveZoneMain = bVal; };
    void SetP1P2DefrostActive(const bool& bVal) { m_bP1P2DefrostActive = bVal; };
    void SetP1P2Compressor(const bool& bVal) { m_bP1P2CompressorOn = bVal; };

    bool GetP1P2AlthermaOn() const { return m_bP1P2AlthermaOn; };
    float GetP1P2PrimaryZoneRoomTemp() const { return m_fP1P2PrimaryZoneRoomTemp; };
    float GetP1P2PrimaryZoneTargetTemp() const { return m_fP1P2PrimaryZoneTargetTemp; };
    float GetP1P2LeavingWaterTemp() const { return m_fP1P2LeavingWaterTemp; };
    bool GetP1P2CirculationPumpOn() const { return m_bP1P2CirculationPumpOn; };
    bool GetP1P2HeatingOn() const { return m_bP1P2HeatingOn; };
    bool GetP1P2CoolingOn() const { return m_bP1P2CoolingOn; };
    bool GetP1P2ValveZoneMain() const { return m_bP1P2ValveZoneMain; };
    bool GetP1P2DefrostActive() const { return m_bP1P2DefrostActive; };
    bool GetP1P2Compressor() const { return m_bP1P2CompressorOn; };
    
//    bool IsDaikinHeatingActive() { return m_bP1P2HeatingOn && m_bP1P2CirculationPumpOn; };
//    bool IsDaikinCoolingActive() { return m_bP1P2CoolingOn && m_bP1P2CirculationPumpOn; };
//    bool IsPrimaryHeatingActive() { return IsDaikinHeatingActive() && m_bP1P2ValveZoneMain; };

    void SetCtrlOnOff(const bool& bVal) { m_bCtrlEnable = bVal; m_bUpdateCtrlEnable = true; };
    void SetCtrlZonePriEnable(const bool& bVal) { m_bCtrlZonePriEnable = bVal; m_bUpdateCtrlZonePriEnable = true; };
    void SetCtrlZoneSecEnable(const bool& bVal) { m_bCtrlZoneSecEnable = bVal; m_bUpdateCtrlZoneSecEnable = true; };
    void SetCtrlZoneSecForce(const bool& bVal) { m_bCtrlZoneSecForce = bVal; m_bUpdateCtrlZoneSecForce = true; };
    void SetCtrlValvePriCloseForce(const bool& bVal) { m_bCtrlValvePriCloseForce = bVal; m_bUpdateCtrlValvePriCloseForce = true; };
    void SetCtrlDaikinDisable(const bool& bVal) { m_bCtrlDaikinDisable = bVal; m_bUpdateCtrlDaikinDisable = true; };
    void SetCtrlGasOnly(const bool& bVal) { m_bCtrlGasOnly = bVal; m_bUpdateCtrlGasOnly = true; };
    void SetCtrlRoom1Enable(const bool& bVal) { m_bCtrlRoom1Enable = bVal; m_bUpdateCtrlRoom1Enable = true; };
    void SetCtrlRoom2Enable(const bool& bVal) { m_bCtrlRoom2Enable = bVal; m_bUpdateCtrlRoom2Enable = true; };
    void SetCtrlRoom3Enable(const bool& bVal) { m_bCtrlRoom3Enable = bVal; m_bUpdateCtrlRoom3Enable = true; };
    void SetCtrlRoom4Enable(const bool& bVal) { m_bCtrlRoom4Enable = bVal; m_bUpdateCtrlRoom4Enable = true; };

    bool GetCtrlOnOff() const { return m_bCtrlEnable; };
    bool GetCtrlZonePriEnable() const { return m_bCtrlZonePriEnable; };
    bool GetCtrlZoneSecEnable() const { return m_bCtrlZoneSecEnable; };
    bool GetCtrlZoneSecForce() const { return m_bCtrlZoneSecForce; };
    bool GetCtrlValvePriCloseForce() const { return m_bCtrlValvePriCloseForce; };
    bool GetCtrlDaikinDisable() const { return m_bCtrlDaikinDisable; };
    bool GetCtrlGasOnly() const { return m_bCtrlGasOnly; };
    bool GetCtrlRoom1Enable() const { return m_bCtrlRoom1Enable; };
    bool GetCtrlRoom2Enable() const { return m_bCtrlRoom2Enable; };
    bool GetCtrlRoom3Enable() const { return m_bCtrlRoom3Enable; };
    bool GetCtrlRoom4Enable() const { return m_bCtrlRoom4Enable; };

    bool GetLeavingWaterTooHigh() const { return m_bLeavingWaterTooHigh; };
    bool GetValveZonePrimaryOpen() const { return m_bValveZonePrimaryOpen; };
    bool GetRoom1ValveOpen() const { return m_bRoom1ValveOpen; };
    bool GetRoom2ValveOpen() const { return m_bRoom2ValveOpen; };
    bool GetRoom3ValveOpen() const { return m_bRoom3ValveOpen; };
    bool GetRoom4ValveOpen() const { return m_bRoom4ValveOpen; };
    bool GetDaikinZonePrimaryEnable() const { return m_bDaikinZonePrimaryEnable; };
    bool GetDaikinZoneSecondaryEnable() const { return m_bDaikinZoneSecondaryEnable; };
    const char* GetAveragePrimaryZoneRoomTemp() const { return m_strAveragePrimaryZoneRoomTemp; };
    bool GetZonePrimaryShortCycleDetected() const { return m_bZonePrimaryShortCycleDetected; };
    bool GetZoneSecondaryShortCycleDetected() const { return m_bZoneSecondaryShortCycleDetected; };

    CUptime::uptime_t GetUpTime() const { return m_upTime.GetBreakdown(); };
    uint8_t GetMqttUpdateInterval() const { return m_iMqttUpdateInterval; };
    uint16_t GetNetWatchDogTimeout() const { return m_iNetWatchDogTimeout; };
    uint16_t GetNetWatchDogRecovery() const { return m_iNetWatchDogRecovery; };
    bool GetLowTempSecondaryZone() const { return m_bLowTempSecondaryZone; };
    bool GetShortCyclePrevention() const { return m_bShortCyclePrevention; };

    float GetLeavingWaterMax() const { return m_fLeavingWaterMax; };
    uint16_t GetPrimaryZoneValveDelay() const { return m_iPrimaryZoneValveDelay; };
    uint16_t GetFloorProtectionDelay() const { return m_iFloorProtectionDelay; };
    uint16_t GetLwtCoolDownDelay() const { return m_iLwtCoolDownDelay; };

  private:
    void MqttPublishDiscovery();
    bool MqttPublishValues();
    void MqttP1P2Subscribe();

    void Update();
    void UpdateRelays();

    CNetwork& m_network;

    elapsedMillis m_loopTimer = 0;
    elapsedMillis m_mqttPublishTimer = 0;
    bool m_bMqttRepublish = false; // Publish all values, including the interval timed ones
    uint32_t m_iNetworkWatchdogCounter = 0;
    uint32_t m_iNetworkWatchdogRecoveryCounter = 0;

    // Keep track of up-time
    CUptime m_upTime;

    uint8_t m_iMqttUpdateInterval = 0;
    uint16_t m_iNetWatchDogTimeout = 0;
    uint16_t m_iNetWatchDogRecovery = 0;
    bool m_bLowTempSecondaryZone = LOW_TEMP_SECONDARY_ZONE_DEFAULT; // Secondary zone uses low temp. AWT range (safe for floor heating)
    bool m_bShortCyclePrevention = SHORT_CYCLE_PREVENTION_DEFAULT;   // Disable zone when short cycling is detected

    // Expert settings
    float m_fLeavingWaterMax = LEAVING_WATER_MAX_DEFAULT;                 // Max. LWT (floor protection)
    uint16_t m_iPrimaryZoneValveDelay = PRIMARY_ZONE_VALVE_DELAY_DEFAULT; // Seconds
    uint16_t m_iFloorProtectionDelay = FLOOR_PROTECTION_DELAY_DEFAULT;    // Seconds
    uint16_t m_iLwtCoolDownDelay = LWT_COOL_DOWN_DELAY_TIME_DEFAULT;      // Seconds

    bool m_bPublishSettings = true;

    bool m_bDaikinPrimaryZoneOn = false;
    bool m_bDaikinSecondaryZoneOn = false;
    bool m_bPrimaryZoneValveClose = false;
    bool m_bFloorProtection = false;
    bool m_bDaikinActive = false;
    bool m_bRoom1ValveOpenRq = false;
    bool m_bRoom2ValveOpenRq = false;
    bool m_bRoom3ValveOpenRq = false;
    bool m_bRoom4ValveOpenRq = false;
    uint16_t m_iSMDelayCounter = 0;
    uint16_t m_iFloorProtectionCounter = 0;
    uint16_t m_iDaikinActiveOffCounter = 0;
    uint16_t m_iDaikinStartupCounter = 0;

    CRollingAverage m_roomTempRollingAverager;

    bool m_bCtrlEnable = true;
    bool m_bUpdateCtrlEnable = true;

    bool m_bCtrlZonePriEnable = false;
    bool m_bUpdateCtrlZonePriEnable = true;

    bool m_bCtrlZoneSecEnable = false;
    bool m_bUpdateCtrlZoneSecEnable = true;

    bool m_bCtrlZoneSecForce = false;
    bool m_bUpdateCtrlZoneSecForce = true;

    bool m_bCtrlValvePriCloseForce = false;
    bool m_bUpdateCtrlValvePriCloseForce = true;

    bool m_bCtrlDaikinDisable = false;
    bool m_bUpdateCtrlDaikinDisable = true;

    bool m_bCtrlGasOnly = false;
    bool m_bUpdateCtrlGasOnly = true;

    bool m_bCtrlRoom1Enable = false;
    bool m_bUpdateCtrlRoom1Enable = true;

    bool m_bCtrlRoom2Enable = false;
    bool m_bUpdateCtrlRoom2Enable = true;

    bool m_bCtrlRoom3Enable = false;
    bool m_bUpdateCtrlRoom3Enable = true;

    bool m_bCtrlRoom4Enable = false;
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

    bool m_bZonePrimaryShortCycleDetected = false;
    bool m_bUpdateZonePrimaryShortCycleDetected = true;

    bool m_bZoneSecondaryShortCycleDetected = false;
    bool m_bUpdateZoneSecondaryShortCycleDetected = true;

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

    CShortCycle m_shortCyclePrimary;
    CShortCycle m_shortCycleSecondary;

    sm_state_e m_iState = STATE_WAIT_STATE;
};
#endif // DAIKIN_CTRL_H