#include "Ctrl4DknCommandHandler.h"
#include "Uptime.h"
#include "Terminal.h"
#include "DaikinCtrl.h"
#include "util.h"
#include "Network.h"
#include "system.h"
#include "SettingLimits.h"

#include <string.h>

const char HELP_STR_P[] PROGMEM = "\r\n"
                                  "help                   : This screen\r\n"
                                  "!                      : Repeat previous command\r\n"
                                  "ver                    : Show device version\r\n"
                                  "fw                     : Show firmware version (number only)\r\n"
                                  "reset                  : Reset controller\r\n"
                                  "reboot                 : Reboot controller\r\n"
                                  "info                   : Show device info\r\n"
                                  "status                 : Show device status\r\n"
                                  "uptime                 : Show device uptime\r\n"
                                  "echo [e]               : Set terminal local echo to [e] (\"on\" or \"off\")\r\n"
                                  "hostname [h]           : Set hostname to [h] (requires reboot)\r\n"
                                  "ssid [s]               : Set WiFi SSID to [s]\r\n"
                                  "pass [w]               : Set WiFi password to [w]\r\n"
                                  "ipaddr [ip]            : Set [ip] (\"dhcp\" for DHCP) for device IP address\r\n"
                                  "netmask [mask]         : Set [mask] for device IP netmask\r\n"
                                  "serverip [ip]          : Set MQTT server IP address to [ip]\r\n"
                                  "mqttuser [u]           : Set MQTT server user to [u] (may be empty)\r\n"
                                  "mqttpass [p]           : Set MQTT server password to [p] (may be empty)\r\n"
                                  "mqttinterval [s]       : Set MQTT update interval to [s] seconds\r\n"
                                  "restartnet             : Restart network (after eg. WiFi/IP settings changes)\r\n"
                                  "netwdt [i]             : Set network watchdog timeout to [i] seconds (min. " STRINGIZE(NETWORK_WATCHDOG_TIMEOUT_MIN) ", 0 or \"off\" to disable)\r\n"
                                  "netwdr [i]             : Set network watchdog recovery time to [i] seconds (min. " STRINGIZE(NETWORK_WATCHDOG_RECOVERY_MIN) ")\r\n"
                                  "lowtempsec [0/1]       : Secondary zone uses low temperature (floor heating safe) AWT range: 0 = no, 1 = yes\r\n"
                                  "scprevent [0/1]        : Short cycle prevention (disable zone when short cycling is detected): 0 = off, 1 = on\r\n"
                                  "exhelp                 : Show expert help\r\n"
                                  "factoryreset           : Reset all (stored) settings to factory defaults (except network)\r\n"
                                  ;

const char EX_HELP_STR_P[] PROGMEM = "\r\n"
                                  "lwtmax [t]             : Set leaving water max. temperature (floor protection) to [t] C (" STRINGIZE(LEAVING_WATER_MAX_MIN) "-" STRINGIZE(LEAVING_WATER_MAX_MAX) ", 0 or \"off\" to disable)\r\n"
                                  "floordelay [s]         : Set floor protection release delay to [s] seconds (min. " STRINGIZE(FLOOR_PROTECTION_DELAY_MIN) ")\r\n"
                                  "valvedelay [s]         : Set primary zone valve close delay to [s] seconds (min. " STRINGIZE(PRIMARY_ZONE_VALVE_DELAY_MIN) ")\r\n"
                                  "lwtcooldown [s]        : Set LWT cool down delay (before opening primary zone valve) to [s] seconds (min. " STRINGIZE(LWT_COOL_DOWN_DELAY_TIME_MIN) ")\r\n"
                                  "scsmallwin [m]         : Set short cycle small window to [m] minutes (" STRINGIZE(SHORT_CYCLE_SMALL_WINDOW_MIN) "-" STRINGIZE(SHORT_CYCLE_SMALL_WINDOW_MAX) ")\r\n"
                                  "scsmallcount [n]       : Set short cycles allowed during small window to [n] (" STRINGIZE(SHORT_CYCLE_SMALL_WINDOW_COUNT_MIN) "-" STRINGIZE(SHORT_CYCLE_SMALL_WINDOW_COUNT_MAX) ")\r\n"
                                  "sclargewin [m]         : Set short cycle large window to [m] minutes (" STRINGIZE(SHORT_CYCLE_LARGE_WINDOW_MIN) "-" STRINGIZE(SHORT_CYCLE_LARGE_WINDOW_MAX) ")\r\n"
                                  "sclargecount [n]       : Set short cycles allowed during large window to [n] (" STRINGIZE(SHORT_CYCLE_LARGE_WINDOW_COUNT_MIN) "-" STRINGIZE(SHORT_CYCLE_LARGE_WINDOW_COUNT_MAX) ")\r\n"
                                  "screcovery [m]         : Set short cycle recovery time to [m] minutes (" STRINGIZE(SHORT_CYCLE_RECOVERY_TIME_MIN) "-" STRINGIZE(SHORT_CYCLE_RECOVERY_TIME_MAX) ")\r\n"
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
  const result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

  CTerminal::println(FPSTR(HELP_STR_P));

  return pack_result_code(ERR_CODE_OK);
}


// Show expert help screen
result_code_t CCtrl4DknCommandHandler::CmdShowExpertHelp(const char *strArgs)
{
  const result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

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

  if (!CNetwork::IsValidHostName(strArgs))
    return pack_result_code(ERR_CODE_ARG_VAL, ARG_INT32_NUM1);

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
  int32_t iInterval;
  const result_code_t result = parse_decimal_arg(strArgs, MQTT_UPDATE_TIME_MIN, MQTT_UPDATE_TIME_MAX, &iInterval);
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
  const result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

  // Output result before restarting, else it's lost on a socket (terminal) connection
  CTerminal::println("OK");

  delay(3000); // Delay to make sure network socket processing is done

  m_network.InitWifi(true);

  return pack_result_code(ERR_CODE_OK_NULL); // Result already printed
}


result_code_t CCtrl4DknCommandHandler::CmdInfo(const char *strArgs)
{
  const result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

  PrintItemValue("host_name", m_network.GetHostName());
  PrintItemValue(" wifi_ssid", m_network.GetWifiSsid());

  PrintItemSecret(" wifi_pass", m_network.GetWifiPassword());

  PrintItemIp(" ip", m_network.GetIpAddr());
  PrintItemIp(" netmask", m_network.GetNetMask());
  PrintItemIp(" mqtt_server", m_network.GetMqttIpAddr());

  const char* strMqttUser = m_network.GetMqttUser();
  PrintItemValue(" mqtt_user", (strlen(strMqttUser) == 0) ? "(empty)" : strMqttUser);

  PrintItemSecret(" mqtt_password", m_network.GetMqttPassword());

  CTerminal::println("");

  PrintItemValue("mqtt_update_interval", m_daikinCtrl.GetMqttUpdateInterval(), "s");
  PrintItemValue(" net_wd_timeout", m_daikinCtrl.GetNetWatchDogTimeout(), "s");
  PrintItemValue(" net_wd_recovery", m_daikinCtrl.GetNetWatchDogRecovery(), "s");

  CTerminal::println("");

  PrintItemValue("lo_temp_sec_zone", m_daikinCtrl.GetLowTemperatureSecondaryZone());

  if (m_daikinCtrl.GetLeavingWaterMax() == 0.0f)
  {
    PrintItemValue(" lwt_max", "off");
  }
  else
  {
    PrintItemValue(" lwt_max", m_daikinCtrl.GetLeavingWaterMax(), 1, "C");
  }

  PrintItemValue(" protect_delay", m_daikinCtrl.GetFloorProtectionDelay(), "s");
  PrintItemValue(" lwt_cool_down", m_daikinCtrl.GetLwtCoolDownDelay(), "s");
  PrintItemValue(" valve_delay", m_daikinCtrl.GetPrimaryZoneValveDelay(), "s");

  CTerminal::println("");

  PrintItemValue("sc_prevent", m_daikinCtrl.GetShortCyclePrevention());
  PrintItemValue(" sc_small_win", m_daikinCtrl.GetShortCycleSmallWindow(), "m");
  PrintItemValue(" sc_small_count", m_daikinCtrl.GetShortCycleSmallWindowCount());
  PrintItemValue(" sc_large_win", m_daikinCtrl.GetShortCycleLargeWindow(), "m");
  PrintItemValue(" sc_large_count", m_daikinCtrl.GetShortCycleLargeWindowCount());
  PrintItemValue(" sc_recovery", m_daikinCtrl.GetShortCycleRecoveryTime(), "m");

  CTerminal::println("");
  CTerminal::println("");

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdStatus(const char *strArgs)
{
  if (strArgs != NULL && *strArgs)
    return pack_result_code(ERR_CODE_TOO_MANY_ARGS, ARG_INT32_NONE);

  PrintItemValue("wifi_conn", m_network.IsConnected());
  PrintItemValue(" mqtt_conn", m_network.IsMqttConnected());

  PrintItemIp(" wifi_ip", WiFi.localIP());
  PrintItemIp(" wifi_subnet", WiFi.subnetMask());

  CTerminal::println("");

  // Controls (MQTT switches)
  PrintItemValue("ctrl_en", m_daikinCtrl.GetCtrlOnOff());
  PrintItemValue(" zone_pri_en", m_daikinCtrl.GetCtrlZonePriEnable());
  PrintItemValue(" zone_sec_en", m_daikinCtrl.GetCtrlZoneSecEnable());
  PrintItemValue(" zone_sec_force", m_daikinCtrl.GetCtrlZoneSecForce());
  PrintItemValue(" valve_pri_close_force", m_daikinCtrl.GetCtrlValvePriCloseForce());
  PrintItemValue(" dkn_dis", m_daikinCtrl.GetCtrlDaikinDisable());
  PrintItemValue(" gas_only", m_daikinCtrl.GetCtrlGasOnly());
  CTerminal::println("");

  PrintItemValue("room1_en", m_daikinCtrl.GetCtrlRoom1Enable());
  PrintItemValue(" room2_en", m_daikinCtrl.GetCtrlRoom2Enable());
  PrintItemValue(" room3_en", m_daikinCtrl.GetCtrlRoom3Enable());
  PrintItemValue(" room4_en", m_daikinCtrl.GetCtrlRoom4Enable());
  CTerminal::println("");

  // States (MQTT sensors)
  PrintItemValue("valve_zone_pri", m_daikinCtrl.GetValveZonePrimaryOpen());
  PrintItemValue(" valve_room1", m_daikinCtrl.GetRoom1ValveOpen());
  PrintItemValue(" valve_room2", m_daikinCtrl.GetRoom2ValveOpen());
  PrintItemValue(" valve_room3", m_daikinCtrl.GetRoom3ValveOpen());
  PrintItemValue(" valve_room4", m_daikinCtrl.GetRoom4ValveOpen());
  CTerminal::println("");

  PrintItemValue("dkn_pri_en", m_daikinCtrl.GetDaikinZonePrimaryEnable());
  PrintItemValue(" dkn_sec_en", m_daikinCtrl.GetDaikinZoneSecondaryEnable());
  PrintItemValue(" lwt_too_hi", m_daikinCtrl.GetLeavingWaterTooHigh());
  PrintItemValue(" sc_pri", m_daikinCtrl.GetZonePrimaryShortCycleDetected());
  PrintItemValue(" sc_sec", m_daikinCtrl.GetZoneSecondaryShortCycleDetected());
  const char* strAvgRoomTemp = m_daikinCtrl.GetAveragePrimaryZoneRoomTemp();
  if (strlen(strAvgRoomTemp) == 0)
  {
    PrintItemValue(" room_temp_avg", "(none)");
  }
  else
  {
    PrintItemValue(" room_temp_avg", strAvgRoomTemp, "C");
  }
  CTerminal::println("");

  // P1P2 (received) values
  PrintItemValue("p1p2_on", m_daikinCtrl.GetP1P2AlthermaOn());
  PrintItemValue(" p1p2_comp", m_daikinCtrl.GetP1P2Compressor());
  PrintItemValue(" p1p2_pump", m_daikinCtrl.GetP1P2CirculationPumpOn());
  PrintItemValue(" p1p2_heat", m_daikinCtrl.GetP1P2HeatingOn());
  PrintItemValue(" p1p2_cool", m_daikinCtrl.GetP1P2CoolingOn());
  PrintItemValue(" p1p2_valve_main", m_daikinCtrl.GetP1P2ValveZoneMain());
  PrintItemValue(" p1p2_defrost", m_daikinCtrl.GetP1P2DefrostActive());
  CTerminal::println("");

  PrintItemValue("p1p2_room_temp", m_daikinCtrl.GetP1P2PrimaryZoneRoomTemp(), 1, "C");
  PrintItemValue(" p1p2_target_temp", m_daikinCtrl.GetP1P2PrimaryZoneTargetTemp(), 1, "C");
  PrintItemValue(" p1p2_lwt", m_daikinCtrl.GetP1P2LeavingWaterTemp(), 1, "C");
  CTerminal::println("");
  CTerminal::println("");

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdUpTime(const char *strArgs)
{
  const result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

  // Publish uptime
  const CUptime::uptime_t upTime = m_daikinCtrl.GetUpTime();
  char strTemp[24];
  snprintf(strTemp, sizeof(strTemp), "%ud %02u:%02u:%02u", upTime.iDays, upTime.iHours, upTime.iMinutes, upTime.iSeconds);
  CTerminal::println(strTemp);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdReset(const char *strArgs)
{
  const result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.Reset();

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdReboot(const char *strArgs)
{
  const result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

  // Clean disconnect so we report offline
  m_network.GetMqttClient().ServerDisconnect();

  CTerminal::println("OK");

  delay(3000); // Delay to make sure network socket processing is done

  ESP.restart();

  // In principle anything from here on is not executed

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdFactoryReset(const char *strArgs)
{
  const result_code_t result = check_arguments(strArgs, ARG_INT32_NONE);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.FactoryReset();

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdNetWatchdogTimeout(const char *strArgs)
{
  result_code_t result = check_arguments(strArgs, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  int32_t iTimeout = 0; // 0 = disabled
  if (!STRIEQUALS(strArgs, "off"))
  {
    result = get_int32_from_string(strArgs, &iTimeout, 0, NETWORK_WATCHDOG_TIMEOUT_MAX, ARG_INT32_NUM1);
    if (result.code != ERR_CODE_OK)
      return result;

    if (iTimeout != 0 && iTimeout < NETWORK_WATCHDOG_TIMEOUT_MIN)
    {
      result_code_t resultCode = pack_result_code(ERR_CODE_ARG_VAL_MIN, ARG_INT32_NUM1);
      return store_arg1_int32(resultCode, NETWORK_WATCHDOG_TIMEOUT_MIN);
    }
  }

  m_daikinCtrl.SetNetWatchDogTimeout(iTimeout);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdNetWatchdogRecovery(const char *strArgs)
{
  result_code_t result = check_arguments(strArgs, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  int32_t iTime;
  result = get_int32_from_string(strArgs, &iTime, NETWORK_WATCHDOG_RECOVERY_MIN, NETWORK_WATCHDOG_RECOVERY_MAX, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetNetWatchDogRecovery(iTime);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdLowTempSecondaryZone(const char *strArgs)
{
  bool bVal;
  const result_code_t result = parse_bool_arg(strArgs, &bVal);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetLowTemperatureSecondaryZone(bVal);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdShortCyclePrevention(const char *strArgs)
{
  bool bVal;
  const result_code_t result = parse_bool_arg(strArgs, &bVal);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetShortCyclePrevention(bVal);

  return pack_result_code(ERR_CODE_OK);
}


// Parse (single) integer argument within range
static result_code_t GetIntArg(const char *strArgs, int32_t *piVal, const int32_t iMin, const int32_t iMax)
{
  const result_code_t result = check_arguments(strArgs, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  return get_int32_from_string(strArgs, piVal, iMin, iMax, ARG_INT32_NUM1);
}


result_code_t CCtrl4DknCommandHandler::CmdLeavingWaterMax(const char *strArgs)
{
  result_code_t result = check_arguments(strArgs, ARG_INT32_NUM1);
  if (result.code != ERR_CODE_OK)
    return result;

  double dblVal = 0.0; // 0 = disabled
  if (!STRIEQUALS(strArgs, "off"))
  {
    result = get_double_from_string(strArgs, &dblVal, 0.0, LEAVING_WATER_MAX_MAX, ARG_INT32_NUM1, 1);
    if (result.code != ERR_CODE_OK)
      return result;

    if (dblVal != 0.0 && dblVal < LEAVING_WATER_MAX_MIN)
    {
      result_code_t resultCode = pack_result_code(ERR_CODE_ARG_VAL_MIN, ARG_INT32_NUM1);
      return store_arg1_dbl(resultCode, LEAVING_WATER_MAX_MIN, 1);
    }
  }

  m_daikinCtrl.SetLeavingWaterMax((float)dblVal);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdFloorProtectionDelay(const char *strArgs)
{
  int32_t iVal;
  const result_code_t result = GetIntArg(strArgs, &iVal, FLOOR_PROTECTION_DELAY_MIN, FLOOR_PROTECTION_DELAY_MAX);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetFloorProtectionDelay(iVal);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdPrimaryZoneValveDelay(const char *strArgs)
{
  int32_t iVal;
  const result_code_t result = GetIntArg(strArgs, &iVal, PRIMARY_ZONE_VALVE_DELAY_MIN, PRIMARY_ZONE_VALVE_DELAY_MAX);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetPrimaryZoneValveDelay(iVal);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdLwtCoolDownDelay(const char *strArgs)
{
  int32_t iVal;
  const result_code_t result = GetIntArg(strArgs, &iVal, LWT_COOL_DOWN_DELAY_TIME_MIN, LWT_COOL_DOWN_DELAY_TIME_MAX);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetLwtCoolDownDelay(iVal);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdShortCycleSmallWindow(const char *strArgs)
{
  int32_t iVal;
  const result_code_t result = GetIntArg(strArgs, &iVal, SHORT_CYCLE_SMALL_WINDOW_MIN, SHORT_CYCLE_SMALL_WINDOW_MAX);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetShortCycleSmallWindow(iVal);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdShortCycleSmallWindowCount(const char *strArgs)
{
  int32_t iVal;
  const result_code_t result = GetIntArg(strArgs, &iVal, SHORT_CYCLE_SMALL_WINDOW_COUNT_MIN, SHORT_CYCLE_SMALL_WINDOW_COUNT_MAX);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetShortCycleSmallWindowCount(iVal);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdShortCycleLargeWindow(const char *strArgs)
{
  int32_t iVal;
  const result_code_t result = GetIntArg(strArgs, &iVal, SHORT_CYCLE_LARGE_WINDOW_MIN, SHORT_CYCLE_LARGE_WINDOW_MAX);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetShortCycleLargeWindow(iVal);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdShortCycleLargeWindowCount(const char *strArgs)
{
  int32_t iVal;
  const result_code_t result = GetIntArg(strArgs, &iVal, SHORT_CYCLE_LARGE_WINDOW_COUNT_MIN, SHORT_CYCLE_LARGE_WINDOW_COUNT_MAX);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetShortCycleLargeWindowCount(iVal);

  return pack_result_code(ERR_CODE_OK);
}


result_code_t CCtrl4DknCommandHandler::CmdShortCycleRecoveryTime(const char *strArgs)
{
  int32_t iVal;
  const result_code_t result = GetIntArg(strArgs, &iVal, SHORT_CYCLE_RECOVERY_TIME_MIN, SHORT_CYCLE_RECOVERY_TIME_MAX);
  if (result.code != ERR_CODE_OK)
    return result;

  m_daikinCtrl.SetShortCycleRecoveryTime(iVal);

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
  else if (STRIEQUALS(strCommand, "echo"))
  {
    result = CmdEchoOnOff(strArgs);
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
    result = CmdNetWatchdogTimeout(strArgs);
  }
  else if (STRIEQUALS(strCommand, "netwdr"))
  {
    result = CmdNetWatchdogRecovery(strArgs);
  }
  else if (STRIEQUALS(strCommand, "lowtempsec"))
  {
    result = CmdLowTempSecondaryZone(strArgs);
  }
  else if (STRIEQUALS(strCommand, "scprevent"))
  {
    result = CmdShortCyclePrevention(strArgs);
  }
  else if (STRIEQUALS(strCommand, "lwtmax"))
  {
    result = CmdLeavingWaterMax(strArgs);
  }
  else if (STRIEQUALS(strCommand, "floordelay"))
  {
    result = CmdFloorProtectionDelay(strArgs);
  }
  else if (STRIEQUALS(strCommand, "valvedelay"))
  {
    result = CmdPrimaryZoneValveDelay(strArgs);
  }
  else if (STRIEQUALS(strCommand, "lwtcooldown"))
  {
    result = CmdLwtCoolDownDelay(strArgs);
  }
  else if (STRIEQUALS(strCommand, "scsmallwin"))
  {
    result = CmdShortCycleSmallWindow(strArgs);
  }
  else if (STRIEQUALS(strCommand, "scsmallcount"))
  {
    result = CmdShortCycleSmallWindowCount(strArgs);
  }
  else if (STRIEQUALS(strCommand, "sclargewin"))
  {
    result = CmdShortCycleLargeWindow(strArgs);
  }
  else if (STRIEQUALS(strCommand, "sclargecount"))
  {
    result = CmdShortCycleLargeWindowCount(strArgs);
  }
  else if (STRIEQUALS(strCommand, "screcovery"))
  {
    result = CmdShortCycleRecoveryTime(strArgs);
  }

  return result;
}
