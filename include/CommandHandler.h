#pragma once
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "CommandParser.h"

// Maximum byte size for arguments stored in m_bufArg1
#define ARG_BUF_BYTE_SIZE 6

class CCommandHandler
{
  public:
    CCommandHandler() {};   // Empty ctor
    ~CCommandHandler() {};  // Empty dtor

    static result_code_t FormatIP(uint8_t *data, char *strResult);

  protected:
    static void PrintItemValue(const char* strItem, const char* strValue);
    static void PrintItemValue(const char* strItem, const bool bValue);
    static void PrintItemIp(const char* strItem, const uint8_t* ipAddr);

    result_code_t CmdEchoOnOff(const char *strArgs);
    result_code_t CmdReboot(const char *strArgs);
};

#endif // COMMAND_HANDLER_H
