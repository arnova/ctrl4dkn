#pragma once
#ifndef CTRL4DKN_COMMAND_HANDLER_H
#define CTRL4DKN_COMMAND_HANDLER_H

#include "CommandHandler.h"
#include "CommandParser.h"

#ifdef ESP8266
  #include <ESP8266WiFi.h>
  using NetClient = WiFiClient;
#elif defined(ESP32)
  #include <WiFi.h>
  using NetClient = WiFiClient;
#elif defined(ARDUINO_TEENSY41)
  #include <NativeEthernet.h>
  using NetClient = EthernetClient;
#endif

// Forward declare
class CDaikinCtrl;
class CNetwork;

class CCtrl4DknCommandHandler : public CCommandHandler
{
  public:
    CCtrl4DknCommandHandler(CDaikinCtrl& daikinCtrl, CNetwork& network) : m_daikinCtrl(daikinCtrl), m_network(network) {};

    result_code_t ProcessCommand(char *strCommand);

  private:
    CDaikinCtrl& m_daikinCtrl;
    CNetwork& m_network;

    result_code_t CmdShowVersion(const char *strArgs);
    result_code_t CmdShowFWVersion(const char *strArgs);
    result_code_t CmdShowHelp(const char *strArgs);
    result_code_t CmdShowExpertHelp(const char *strArgs);

    result_code_t CmdInfo(const char *strArgs);
    result_code_t CmdStatus(const char *strArgs);
    result_code_t CmdUpTime(const char *strArgs);
    result_code_t CmdReset(const char *strArgs);
    result_code_t CmdFactoryReset(const char *strArgs);

    result_code_t CmdSetHostName(const char *strArgs);
    result_code_t CmdSetIpAddress(const char *strArgs);
    result_code_t CmdSetNetMask(const char *strArgs);
    result_code_t CmdSetWifiSsid(const char *strArgs);
    result_code_t CmdSetWifiPassword(const char *strArgs);

    result_code_t CmdSetMqttIpAddress(const char *strArgs);
    result_code_t CmdSetMqttUser(const char *strArgs);
    result_code_t CmdSetMqttPassword(const char *strArgs);
    result_code_t CmdSetMqttUpdateInterval(const char *strArgs);

    result_code_t CmdNetWatchdogTimeout(const char *strArgs);
    result_code_t CmdNetWatchdogRecovery(const char *strArgs);
    result_code_t CmdLowTempSecondaryZone(const char *strArgs);
    result_code_t CmdShortCyclePrevention(const char *strArgs);

    result_code_t CmdLeavingWaterMax(const char *strArgs);
    result_code_t CmdFloorProtectionDelay(const char *strArgs);
    result_code_t CmdPrimaryZoneValveDelay(const char *strArgs);
    result_code_t CmdLwtCoolDownDelay(const char *strArgs);
    result_code_t CmdRestartNet(const char *strArgs);
};
#endif // CTRL4DKN_COMMAND_HANDLER_H
