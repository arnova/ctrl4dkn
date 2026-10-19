#include "Ctrl4DknCommandHandler.h"
#include "Uptime.h"
#include "Terminal.h"
#include "DaikinCtrl.h"
#include "util.h"
#include "Network.h"
#include "system.h"

#include <string.h>

const char HELP_STR_P[] PROGMEM = "\r\n"
                                  "help                   : This screen\r\n"
                                  "!                      : Repeat previous command\r\n"
                                  "ver                    : Show device version\r\n"
                                  "reset                  : Reset controller\r\n"
                                  "reboot                 : Reboot controller\r\n"
                                  "info                   : Show device info\r\n"
                                  "status                 : Show device status\r\n"
                                  "uptime                 : Show device uptime\r\n"
                                  "hostname [h]           : Set hostname to [n]\r\n"
                                  "ssid [s]               : Set WiFi SSID to [s]\r\n"
                                  "pass [w]               : Set WiFi password to [w]\r\n"
                                  "ipaddr [ip]            : Set [ip] (\"dhcp\" for DHCP) for device IP address\r\n"
                                  "netmask [mask]         : Set [mask] for device IP netmask\r\n"
                                  "serverip [ip]          : Set MQTT server IP address to [ip]\r\n"
                                  "mqttuser [u]           : Set MQTT server user to [u] (may be empty)\r\n"
                                  "mqttpass [p]           : Set MQTT server password to [p] (may be empty)\r\n"
                                  "mqttinterval [s]       : Set MQTT update interval to [s] seconds\r\n"
                                  "restartnet             : Restart network (after eg. WiFi/IP/hostname settings changes)\r\n"
                                  "netwdt [i]             : Set network watchdog timeout to [i] seconds (0 or \"off\" to disable)\r\n"
                                  "netwdr [i]             : Set network watchdog recovery time to [i] seconds\r\n"
//                                  "exhelp                 : Show expert help\r\n"
                                  "factoryreset           : Reset all (stored) settings to factory defaults (except network)\r\n"
                                  ;

const char EX_HELP_STR_P[] PROGMEM = "\r\n"
                                  ;

// Show copyright + firmware version
result_code_t CCtrl4DknCommandHandler::CmdShowVersion(const char *strArgs)
{
  result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

  CTerminal::println(FPSTR(VER_STR_P));

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdShowFWVersion(const char *strArgs)
{
  result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

  CTerminal::println(MY_VERSION);

  return pack_result_code(ERR_CODE_OK);
}


// Show help screen
result_code_t CCtrl4DknCommandHandler::CmdShowHelp(const char *strArgs)
{
  if (strArgs != NULL && *strArgs)
    return pack_result_code(ERR_CODE_TOO_MANY_ARGS, ARG_INT32_NONE);

  CTerminal::println(FPSTR(HELP_STR_P));

  return pack_result_code(ERR_CODE_OK);
}


// Show expert help screen
result_code_t CCtrl4DknCommandHandler::CmdShowExpertHelp(const char *strArgs)
{
  if (strArgs != NULL && *strArgs)
    return pack_result_code(ERR_CODE_TOO_MANY_ARGS, ARG_INT32_NONE);

  CTerminal::println(FPSTR(EX_HELP_STR_P));

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdSetHostName(const char *strArgs)
{
  if (strArgs == NULL || !*strArgs)
    return pack_result_code(ERR_CODE_ARG_MISSING, ARG_INT32_NUM1);

  if (strlen(strArgs) > HOST_NAME_MAX_SIZE)
  {
    result_code_t resultCode = pack_result_code(ERR_CODE_ARG_STR_MAX, ARG_INT32_NUM1);
    return store_arg1_int32(resultCode, HOST_NAME_MAX_SIZE);
  }

  if (strlen(strArgs) == 0)
  {
    result_code_t resultCode = pack_result_code(ERR_CODE_ARG_STR_MIN, ARG_INT32_NUM1);
    return store_arg1_int32(resultCode, 1);
  }

  m_network.SetHostName(strArgs);

  return pack_result_code(ERR_CODE_OK_AFTER_RESTART);
}


result_code_t CCtrl4DknCommandHandler::CmdSetIpAddress(const char *strArgs)
{
  if (strArgs == NULL || !*strArgs)
    return pack_result_code(ERR_CODE_ARG_MISSING, ARG_INT32_NUM1);

  uint8_t ipAddress[4] = { 0 };

  if (!STRIEQUALS(strArgs, "dhcp"))
  {
    const result_code_t result = parse_ipv4_arg(strArgs, ipAddress);
    if (result.code != ERR_CODE_OK)
      return pack_result_code(ERR_CODE_INVALID_IPV4);
  }

  m_network.SetIpAddr(ipAddress);

  return pack_result_code(ERR_CODE_OK_AFTER_RESTART);
}


result_code_t CCtrl4DknCommandHandler::CmdSetNetMask(const char *strArgs)
{
  if (strArgs == NULL || !*strArgs)
    return pack_result_code(ERR_CODE_ARG_MISSING, ARG_INT32_NUM1);

  uint8_t ipAddress[4];
    const result_code_t result = parse_ipv4_arg(strArgs, ipAddress);
  if (result.code != ERR_CODE_OK)
    return pack_result_code(ERR_CODE_INVALID_IPV4);

  m_network.SetNetMask(ipAddress);

  return pack_result_code(ERR_CODE_OK_AFTER_RESTART);
}


result_code_t CCtrl4DknCommandHandler::CmdSetMqttIpAddress(const char *strArgs)
{
  if (strArgs == NULL || !*strArgs)
    return pack_result_code(ERR_CODE_ARG_MISSING, ARG_INT32_NUM1);

  uint8_t ipAddress[4];
    const result_code_t result = parse_ipv4_arg(strArgs, ipAddress);
  if (result.code != ERR_CODE_OK)
    return pack_result_code(ERR_CODE_INVALID_IPV4);

  m_network.SetMqttServerIp(ipAddress);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdSetMqttUpdateInterval(const char *strArgs)
{
  result_code_t result = check_arguments(strArgs, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  int32_t iInterval;
  result = get_int32_from_string(strArgs, &iInterval, MQTT_UPDATE_TIME_MIN, MQTT_UPDATE_TIME_MAX, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetMqttUpdateInterval(iInterval);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdSetWifiSsid(const char *strArgs)
{
  if (strArgs == NULL || !*strArgs)
    return pack_result_code(ERR_CODE_ARG_MISSING, ARG_INT32_NUM1);

  if (strlen(strArgs) > WIFI_SSID_MAX_SIZE)
  {
    result_code_t resultCode = pack_result_code(ERR_CODE_ARG_STR_MAX, ARG_INT32_NUM1);
    return store_arg1_int32(resultCode, WIFI_SSID_MAX_SIZE);
  }

  m_network.SetWifiSsid(strArgs);

  return pack_result_code(ERR_CODE_OK_AFTER_RESTART);
}


result_code_t CCtrl4DknCommandHandler::CmdSetWifiPassword(const char *strArgs)
{
  if (strArgs && strlen(strArgs) > WIFI_PASSWORD_MAX_SIZE)
  {
    result_code_t resultCode = pack_result_code(ERR_CODE_ARG_STR_MAX, ARG_INT32_NUM1);
    return store_arg1_int32(resultCode, WIFI_PASSWORD_MAX_SIZE);
  }

  m_network.SetWifiPassword((strArgs == NULL) ? "" : strArgs);

  return pack_result_code(ERR_CODE_OK_AFTER_RESTART);
}


result_code_t CCtrl4DknCommandHandler::CmdSetMqttUser(const char *strArgs)
{
  if (strArgs && strlen(strArgs) > MQTT_USER_MAX_SIZE)
  {
    result_code_t resultCode = pack_result_code(ERR_CODE_ARG_STR_MAX, ARG_INT32_NUM1);
    return store_arg1_int32(resultCode, MQTT_USER_MAX_SIZE);
  }

  m_network.SetMqttUser((strArgs == NULL) ? "" : strArgs);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdSetMqttPassword(const char *strArgs)
{
  if (strArgs && strlen(strArgs) > MQTT_PASSWORD_MAX_SIZE)
  {
    result_code_t resultCode = pack_result_code(ERR_CODE_ARG_STR_MAX, ARG_INT32_NUM1);
    return store_arg1_int32(resultCode, MQTT_PASSWORD_MAX_SIZE);
  }

  m_network.SetMqttPassword((strArgs == NULL) ? "" : strArgs);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdRestartNet(const char *strArgs)
{
  if (strArgs != NULL && *strArgs)
    return pack_result_code(ERR_CODE_TOO_MANY_ARGS);

  m_network.InitWifi(true);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdInfo(const char *strArgs)
{
  if (strArgs != NULL && *strArgs)
    return pack_result_code(ERR_CODE_TOO_MANY_ARGS);

  char strBuf[20]; // Enough room for float with 3 decimals

  CTerminal::print("host_name=");
  CTerminal::print(m_network.GetHostName());

  CTerminal::print(" wifi_ssid=");
  CTerminal::print(m_network.GetWifiSsid());

  CTerminal::print(" wifi_pass=");
  const char* strWifiPass = m_network.GetWifiPassword();
  CTerminal::print((strlen(strWifiPass) == 0) ? "(empty)" : strWifiPass);

  CTerminal::print(" ip=");
  CTerminal::print(m_network.GetIpAddr()[0]);
  CTerminal::print(".");
  CTerminal::print(m_network.GetIpAddr()[1]);
  CTerminal::print(".");
  CTerminal::print(m_network.GetIpAddr()[2]);
  CTerminal::print(".");
  CTerminal::print(m_network.GetIpAddr()[3]);

  CTerminal::print(" netmask=");
  CTerminal::print(m_network.GetNetMask()[0]);
  CTerminal::print(".");
  CTerminal::print(m_network.GetNetMask()[1]);
  CTerminal::print(".");
  CTerminal::print(m_network.GetNetMask()[2]);
  CTerminal::print(".");
  CTerminal::print(m_network.GetNetMask()[3]);

  CTerminal::print(" mqtt_server=");
  CTerminal::print(m_network.GetMqttIpAddr()[0]);
  CTerminal::print(".");
  CTerminal::print(m_network.GetMqttIpAddr()[1]);
  CTerminal::print(".");
  CTerminal::print(m_network.GetMqttIpAddr()[2]);
  CTerminal::print(".");
  CTerminal::print(m_network.GetMqttIpAddr()[3]);

  CTerminal::print(" mqtt_user=");
  const char* strMqttUser = m_network.GetMqttUser();
  CTerminal::print((strlen(strMqttUser) == 0) ? "(empty)" : strMqttUser);

  CTerminal::print(" mqtt_password=");
  const char* strMqttPass = m_network.GetMqttPassword();
  CTerminal::print((strlen(strMqttPass) == 0) ? "(empty)" : strMqttPass);

  CTerminal::println("");

  CTerminal::print("mqtt_update_interval=");
  snprintf(strBuf, sizeof(strBuf), "%us", m_daikinCtrl.GetMqttUpdateInterval());
  CTerminal::print(strBuf);

  CTerminal::print(" net_wd_timeout=");
  snprintf(strBuf, sizeof(strBuf), "%us", m_daikinCtrl.GetNetWatchDogTimeout());
  CTerminal::print(strBuf);

  CTerminal::print(" net_wd_recovery=");
  snprintf(strBuf, sizeof(strBuf), "%us", m_daikinCtrl.GetNetWatchDogRecovery());
  CTerminal::print(strBuf);

  CTerminal::println("");
  CTerminal::println("");

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdStatus(const char *strArgs)
{
  if (strArgs != NULL && *strArgs)
    return pack_result_code(ERR_CODE_TOO_MANY_ARGS);

  CTerminal::print("wifi_conn=");
  CTerminal::print(m_network.IsConnected() ? "1" : "0");

  CTerminal::print(" mqtt_conn=");
  CTerminal::print(m_network.IsMqttConnected() ? "1" : "0");

  char strBuf[20]; // Enough room for float with 3 decimals

  CTerminal::print(" wifi_ip=");
  snprintf(strBuf, sizeof(strBuf), "%u.%u.%u.%u", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  CTerminal::print(strBuf);

  CTerminal::println("");

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdUpTime(const char *strArgs)
{
  if (strArgs != NULL && *strArgs)
    return pack_result_code(ERR_CODE_TOO_MANY_ARGS);

  // Publish uptime
  const CUptime::uptime_t upTime = m_daikinCtrl.GetUpTime();
  char strTemp[24];
  snprintf(strTemp, sizeof(strTemp), "%ud %02u:%02u:%02u", upTime.iDays, upTime.iHours, upTime.iMinutes, upTime.iSeconds);
  CTerminal::println(strTemp);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdReset(const char *strArgs)
{
  if (strArgs != NULL && *strArgs)
    return pack_result_code(ERR_CODE_TOO_MANY_ARGS);

  m_daikinCtrl.Reset();

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdFactoryReset(const char *strArgs)
{
  if (strArgs != NULL && *strArgs)
    return pack_result_code(ERR_CODE_TOO_MANY_ARGS);

  m_daikinCtrl.FactoryReset();

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdNetWatchdogTimeout(const char *strArgs)
{
  result_code_t result = check_arguments(strArgs, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  if (STRIEQUALS(strArgs, "off") || STRIEQUALS(strArgs, "0"))
  {
    m_daikinCtrl.SetNetWatchDogTimeout(0);
  }
  else
  {
    int32_t iTimeout;
    result = get_int32_from_string(strArgs, &iTimeout, 0, NETWORK_WATCHDOG_TIMEOUT_MAX, ARG_INT32_NUM1);
    if (result.code != ERR_CODE_OK)
      return result;

    m_daikinCtrl.SetNetWatchDogTimeout(iTimeout);
  }

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdNetWatchdogRecovery(const char *strArgs)
{
  result_code_t result = check_arguments(strArgs, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  int32_t iTime;
  result = get_int32_from_string(strArgs, &iTime, 0, NETWORK_WATCHDOG_RECOVERY_MAX, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetNetWatchDogRecovery(iTime);

  return pack_result_code(ERR_CODE_OK);
}


// Process provided string command
result_code_t CCtrl4DknCommandHandler::ProcessCommand(char *strCommand)
{
  // Trim strCommand to remove leading/trailing space \n \r
  strtrim(strCommand, " \r\n");

  // Preinit return value
  result_code_t result = pack_result_code(ERR_CODE_CMD_UNKNOWN);

  // Split into strCommand & arguments
  char *strArgs = strsplit(strCommand, " "); // strArgs may be NULL, but is handled by the functions called below
  if (strArgs)
  {
    strtrimleft(strArgs, " "); // Trim any leading spaces
  }

  // Our command interpreter:
  if (STRIEQUALS(strCommand, "ver"))
  {
    result = CmdShowVersion(strArgs);
  }
  else if (STRIEQUALS(strCommand, "fw") || STRIEQUALS(strCommand, "fwv"))
  {
    result = CmdShowFWVersion(strArgs);
  }
  else if (STRIEQUALS(strCommand, "help") || STRIEQUALS(strCommand, "?"))
  {
    result = CmdShowHelp(strArgs);
  }
  else if (STRIEQUALS(strCommand, "exhelp"))
  {
    result = CmdShowExpertHelp(strArgs);
  }
  else if (STRIEQUALS(strCommand, "reboot"))
  {
    if (strArgs == NULL || !*strArgs)
      m_network.GetMqttClient().ServerDisconnect();

    result = CmdReboot(strArgs);
  }
  else if (STRIEQUALS(strCommand, "reset"))
  {
    result = CmdReset(strArgs);
  }
  else if (STRIEQUALS(strCommand, "factoryreset"))
  {
    result = CmdFactoryReset(strArgs);
  }
  else if (STRIEQUALS(strCommand, "info"))
  {
    result = CmdInfo(strArgs);
  }
  else if (STRIEQUALS(strCommand, "status"))
  {
    result = CmdStatus(strArgs);
  }
  else if (STRIEQUALS(strCommand, "uptime"))
  {
    result = CmdUpTime(strArgs);
  }
  else if (STRIEQUALS(strCommand, "hostname") || STRIEQUALS(strCommand, "name"))
  {
    result = CmdSetHostName(strArgs);
  }
  else if (STRIEQUALS(strCommand, "ssid") || STRIEQUALS(strCommand, "wssid") || STRIEQUALS(strCommand, "wifissid"))
  {
    result = CmdSetWifiSsid(strArgs);
  }
  else if (STRIEQUALS(strCommand, "pass") || STRIEQUALS(strCommand, "wpass") || STRIEQUALS(strCommand, "wifipass"))
  {
    result = CmdSetWifiPassword(strArgs);
  }
  else if (STRIEQUALS(strCommand, "ipaddr") || STRIEQUALS(strCommand, "ip"))
  {
    result = CmdSetIpAddress(strArgs);
  }
  else if (STRIEQUALS(strCommand, "netmask"))
  {
    result = CmdSetNetMask(strArgs);
  }
  else if (STRIEQUALS(strCommand, "serverip") || STRIEQUALS(strCommand, "mqttip"))
  {
    result = CmdSetMqttIpAddress(strArgs);
  }
  else if (STRIEQUALS(strCommand, "mqttuser") || STRIEQUALS(strCommand, "muser"))
  {
    result = CmdSetMqttUser(strArgs);
  }
  else if (STRIEQUALS(strCommand, "mqttpass") || STRIEQUALS(strCommand, "mpass"))
  {
    result = CmdSetMqttPassword(strArgs);
  }
  else if (STRIEQUALS(strCommand, "mqttint") || STRIEQUALS(strCommand, "mqttinterval") || STRIEQUALS(strCommand, "minterval"))
  {
    result = CmdSetMqttUpdateInterval(strArgs);
  }
  else if (STRIEQUALS(strCommand, "restartnet"))
  {
    result = CmdRestartNet(strArgs);
  }
  else if (STRIEQUALS(strCommand, "netwdt"))
  {
    //result = CmdNetWatchdogTimeout(strArgs); // FIXME
  }
  else if (STRIEQUALS(strCommand, "netwdr"))
  {
    //result = CmdNetWatchdogRecovery(strArgs); // FIXME
  }

  return result;
}
