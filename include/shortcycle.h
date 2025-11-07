#pragma once
#ifndef SHORT_CYCLE_H
#define SHORT_CYCLE_H

#include <stdint.h>

#include "system.h"

#ifndef SHORT_CYCLE_SAMPLES
#define SHORT_CYCLE_SAMPLES                     5
#endif

#ifndef SHORT_CYCLE_MIN_TIME
#define SHORT_CYCLE_MIN_TIME                   15   // minutes
#endif

#ifndef SHORT_CYCLE_MAX_COUNT
#define SHORT_CYCLE_MAX_COUNT                   2   // Amount of short cycles allowed during SHORT_CYCLE_MIN_TIME
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
    uint16_t m_iShortCycleRecoveryCounter = 0;
    uint32_t m_iShortCycleTimestamp = 0;
    uint32_t m_iShortCycleTimeStamps[SHORT_CYCLE_SAMPLES] = { UINT32_MAX / 2 };
    bool bLastOn = false;
};

#endif // SHORT_CYCLE_H
