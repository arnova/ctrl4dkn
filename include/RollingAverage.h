#ifndef ROLLING_AVERAGE_H
#define ROLLING_AVERAGE_H

#include <inttypes.h>

class CRollingAverage
{
  public:
    CRollingAverage() {}; // Empty constructor
    ~CRollingAverage() {}; // Empty destructor

    void SetAvgCount(uint32_t iCount);
    float GetValue() const { return m_fAccVal / m_iAccCount; };
    bool HasValue() const { return m_iAccCount != 0; };
    uint32_t GetAccCount() const { return m_iAccCount; };
    void UpdateValue(const float fVal);
    void RemoveValue();
    void Reset() { m_fAccVal = 0.0f; m_iAccCount = 0; };

  private:
    float m_fAccVal = 0.0f;
    uint32_t m_iAccCount = 0;
    uint32_t m_iAvgCountSet = 1;
};
#endif // ROLLING_AVERAGE_H
