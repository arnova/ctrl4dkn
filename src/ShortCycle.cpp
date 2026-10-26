#include "ShortCycle.h"

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


void CShortCycle::SetConfig(const uint16_t iSmallWindow, const uint8_t iSmallWindowCount, const uint16_t iLargeWindow, const uint8_t iLargeWindowCount, const uint16_t iRecoveryTime)
{
  m_iSmallWindow = iSmallWindow;
  m_iSmallWindowCount = iSmallWindowCount;
  m_iLargeWindow = iLargeWindow;
  m_iLargeWindowCount = iLargeWindowCount;
  m_iRecoveryTime = iRecoveryTime;
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
    // Include the current stop in the counts
    uint8_t iCycleCountSmall = 1;
    uint8_t iCycleCountLarge = 1;
    int8_t iTimeStampPos = 0;
    uint32_t iTimeDiffOldest = (uint64_t)(m_iShortCycleTimestamp - m_iShortCycleTimeStamps[0]) * m_iLoopTime / 60000;
    for (uint8_t it = 0; it < SHORT_CYCLE_SAMPLES; it++)
    {
      // Note: iTimeDiff already handles wrapping. Note that it's in minutes
      const uint32_t iTimeDiff = (uint64_t)(m_iShortCycleTimestamp - m_iShortCycleTimeStamps[it]) * m_iLoopTime / 60000;

      // Check if this is a short cycle in the small window
      if (iTimeDiff <= m_iSmallWindow)
      {
        iCycleCountSmall++;
      }

      // Check if this is a short cycle in the large window
      if (iTimeDiff <= m_iLargeWindow)
      {
        iCycleCountLarge++;
      }

      // Overwrite the oldest timestamp
      if (iTimeDiff > iTimeDiffOldest)
      {
        iTimeStampPos = it;
        iTimeDiffOldest = iTimeDiff;
      }
    }

    if (iCycleCountSmall > m_iSmallWindowCount || iCycleCountLarge > m_iLargeWindowCount)
    {
      m_iShortCycleRecoveryCounter = (uint32_t)m_iRecoveryTime * 60000 / m_iLoopTime;
    }

    m_iShortCycleTimeStamps[iTimeStampPos] = m_iShortCycleTimestamp; // Store current timestamp
  }

  bLastState = bOn;
}