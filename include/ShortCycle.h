#pragma once
#ifndef SHORT_CYCLE_H
#define SHORT_CYCLE_H

#include <stdint.h>

#include "system.h"

#ifndef SHORT_CYCLE_SAMPLES
#define SHORT_CYCLE_SAMPLES                     5
#endif

class CShortCycle
{
  public:
    CShortCycle() { Reset(); };
    ~CShortCycle() {}; // Empty dtor

    void Loop(const bool bOn);
    void Reset();
    bool IsRecoveryActive() { return m_iShortCycleRecoveryCounter > 0; };

    // Windows & recovery time are in minutes
    void SetConfig(const uint16_t iSmallWindow, const uint8_t iSmallWindowCount, const uint16_t iLargeWindow, const uint8_t iLargeWindowCount, const uint16_t iRecoveryTime);

  private:
    bool bLastState = false;
    uint16_t m_iShortCycleRecoveryCounter = 0;
    uint32_t m_iShortCycleTimestamp = 0;
    uint32_t m_iShortCycleTimeStamps[SHORT_CYCLE_SAMPLES] = { UINT32_MAX / 2 };

    uint16_t m_iSmallWindow = SHORT_CYCLE_SMALL_WINDOW_DEFAULT;
    uint8_t m_iSmallWindowCount = SHORT_CYCLE_SMALL_WINDOW_COUNT_DEFAULT; // Amount of short cycles allowed during small window
    uint16_t m_iLargeWindow = SHORT_CYCLE_LARGE_WINDOW_DEFAULT;
    uint8_t m_iLargeWindowCount = SHORT_CYCLE_LARGE_WINDOW_COUNT_DEFAULT; // Amount of short cycles allowed during large window
    uint16_t m_iRecoveryTime = SHORT_CYCLE_RECOVERY_TIME_DEFAULT;
};

#endif // SHORT_CYCLE_H
