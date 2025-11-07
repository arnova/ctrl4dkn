      
#include "shortcycle.h"

void CShortCycle::Reset()
{
  m_iShortCycleTimestamp = 0;
  m_iShortCycleRecoveryCounter = 0;
}


void CShortCycle::Loop(const bool bOn)
{
  // Increment timestamp
  m_iShortCycleTimestamp++;

  if (m_iShortCycleRecoveryCounter > 0)
  {
    m_iShortCycleRecoveryCounter--;
    return;
  }

  if (bOn)
    return; // We're done

  int8_t iPos = -1;
  uint8_t iCycleCount = 0;
  uint32_t iTimeDiffOldest = (m_iShortCycleTimestamp - m_iShortCycleTimeStamps[0]) / (60 / CONTROL_LOOP_TIME);
  for (uint8_t it = 0; it < SHORT_CYCLE_SAMPLES; it++)
  {
    // Note: iTimeDiff already handles wrapping
    const uint32_t iTimeDiff = (m_iShortCycleTimestamp - m_iShortCycleTimeStamps[it]) / (60 / CONTROL_LOOP_TIME);

    // Check if this is a short cycle
    if (iTimeDiff <= SHORT_CYCLE_MIN_TIME)
    {
      iCycleCount++;
    }

    // FIXME: We need to check which timestamp is the oldest, also when within SHORT_CYCLE_MIN_TIME!
    // Overwrite one of timestamps that's no longer interesting
    if (iTimeDiff < iTimeDiffOldest || iPos == -1) // FIXME
    {
      iPos = it;
      iTimeDiffOldest = iTimeDiff;
    }
  }

  if (iCycleCount >= SHORT_CYCLE_MAX_COUNT)
  {
    m_iShortCycleRecoveryCounter = SHORT_CYCLE_RECOVERY_TIME * (60 / CONTROL_LOOP_TIME);
  }

  m_iShortCycleTimeStamps[iPos] = m_iShortCycleTimestamp; // Store current timestamp
}