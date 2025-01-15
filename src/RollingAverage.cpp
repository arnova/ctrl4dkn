/*
  Rolling average library
  (C) Copyright 2010-2024, Arno van Amersfoort

  Written by       : Arno van Amersfoort
  Language         : C++11
  Target compiler  : AVR
  Dependencies     : (none)
  Initial date     : March 23, 2018
  Last modified    : October 6, 2024
*/

#include "RollingAverage.h"

void CRollingAverage::UpdateValue(const float& fNewVal)
{
  float fAccVal = (m_iAccCount == 0 ? 0.0f : m_fAccVal);

  if (m_iAccCount == m_iAvgCountSet)
  {
    fAccVal -= (fAccVal / m_iAvgCountSet);
  }
  else
  {
    m_iAccCount++;
  }

  m_fAccVal = fAccVal + fNewVal; // Update value
}


void CRollingAverage::RemoveValue()
{
  if (m_iAccCount > 0)
  {
    m_fAccVal -= (m_fAccVal / m_iAvgCountSet);
    m_iAccCount--;
  }
}
