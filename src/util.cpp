#include <Arduino.h>
#include <limits.h>
#include "util.h"

bool BytesToFloat(const byte* bytes, const unsigned int len, float& fResult)
{
  char strBytes[8] = { 0 };

  if (len >= sizeof(strBytes))
    return false;

  memcpy(strBytes, bytes, len);
//  strPayLoad[length] = '\0';

  char *end;
  fResult = strtof(strBytes, &end);

  // Make sure it's not zero-length and it's a proper number (not suffixed with anything):
  if (*end || end == strBytes)
    return false;

  // Check range
  return (!(fResult == LONG_MIN || fResult == LONG_MAX));
}


bool BytesToInt8(const byte* bytes, const unsigned int len, int8_t& iResult)
{
  char strBytes[8] = { 0 };

  if (len >= sizeof(strBytes))
    return false;

  memcpy(strBytes, bytes, len);
//  strPayLoad[length] = '\0';

  char *end;
  iResult = strtol(strBytes, &end, 0);

  // Make sure it's not zero-length and it's a proper number (not suffixed with anything):
  if (*end || end == strBytes)
    return false;

  // Check range
  return (!(iResult == LONG_MIN || iResult == LONG_MAX));
}
