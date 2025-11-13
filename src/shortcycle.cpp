      
#include "shortcycle.h"

void CShortCycle::Reset()
{
  bLastState = false;
  m_iShortCycleTimestamp = 0;
  m_iShortCycleRecoveryCounter = 0;
  for (uint8_t it = 0; it < SHORT_CYCLE_SAMPLES; it++)
  {
    m_iShortCycleTimeStamps[it] = UINT32_MAX / 2;
  }
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

  // Check for on-off state change
  if (bLastState && !bOn)
  {
    uint8_t iCycleCountSmall = 0;
    uint8_t iCycleCountLarge = 0;
    int8_t iTimeStampPos = 0;
    uint32_t iTimeDiffOldest = (m_iShortCycleTimestamp - m_iShortCycleTimeStamps[0]) / 60 / CONTROL_LOOP_TIME;
    for (uint8_t it = 0; it < SHORT_CYCLE_SAMPLES; it++)
    {
      // Note: iTimeDiff already handles wrapping. Note that it's in minutes
      const uint32_t iTimeDiff = (m_iShortCycleTimestamp - m_iShortCycleTimeStamps[it]) / 60 / CONTROL_LOOP_TIME;

      // Check if this is a short cycle in the small window
      if (iTimeDiff <= SHORT_CYCLE_SMALL_WINDOW)
      {
        iCycleCountSmall++;
      }

      // Check if this is a short cycle in the large window
      if (iTimeDiff <= SHORT_CYCLE_LARGE_WINDOW)
      {
        iCycleCountLarge++;
      }

      // Overwrite the oldest timestamp
      if (iTimeDiff < iTimeDiffOldest)
      {
        iTimeStampPos = it;
        iTimeDiffOldest = iTimeDiff;
      }
    }

    if (iCycleCountSmall > SHORT_CYCLE_SMALL_WINDOW_COUNT || iCycleCountLarge > SHORT_CYCLE_LARGE_WINDOW_COUNT)
    {
      m_iShortCycleRecoveryCounter = SHORT_CYCLE_RECOVERY_TIME * 60 * CONTROL_LOOP_TIME;
    }

    m_iShortCycleTimeStamps[iTimeStampPos] = m_iShortCycleTimestamp; // Store current timestamp
  }

  bLastState = bOn;
}