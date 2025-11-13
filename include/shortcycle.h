#pragma once
#ifndef SHORT_CYCLE_H
#define SHORT_CYCLE_H

#include <stdint.h>

#include "system.h"

#ifndef SHORT_CYCLE_SAMPLES
#define SHORT_CYCLE_SAMPLES                     5
#endif

#ifndef SHORT_CYCLE_SMALL_WINDOW
#define SHORT_CYCLE_SMALL_WINDOW               15   // minutes
#endif

#ifndef SHORT_CYCLE_SMALL_WINDOW_COUNT
#define SHORT_CYCLE_SMALL_WINDOW_COUNT          2   // Amount of short cycles allowed during SHORT_CYCLE_SMALL_WINDOW
#endif

#ifndef SHORT_CYCLE_LARGE_WINDOW
#define SHORT_CYCLE_LARGE_WINDOW               60   // minutes
#endif

#ifndef SHORT_CYCLE_LARGE_WINDOW_COUNT
#define SHORT_CYCLE_LARGE_WINDOW_COUNT          4   // Amount of short cycles allowed during SHORT_CYCLE_LARGE_WINDOW
#endif

#ifndef SHORT_CYCLE_RECOVERY_TIME
#define SHORT_CYCLE_RECOVERY_TIME              30   // minutes
#endif

class CShortCycle
{
  public:
    CShortCycle() { Reset(); };
    ~CShortCycle() {}; // Empty dtor

    void Loop(const bool bOn);
    void Reset();
    bool IsRecoveryActive() { return m_iShortCycleRecoveryCounter > 0; };
  
  private:
    bool bLastState = false;
    uint16_t m_iShortCycleRecoveryCounter = 0;
    uint32_t m_iShortCycleTimestamp = 0;
    uint32_t m_iShortCycleTimeStamps[SHORT_CYCLE_SAMPLES] = { UINT32_MAX / 2 };
};

#endif // SHORT_CYCLE_H
