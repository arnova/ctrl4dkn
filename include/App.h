#pragma once
#ifndef APP_H
#define APP_H

#include "system.h"
#include "Network.h"
#include "DaikinCtrl.h"
#include "Terminal.h"

class CApp
{
  public:
    CApp();

    void Init();
    void Loop();

    bool CommandHandler();
    void HandleNetwork();

    CNetwork& GetNetwork() { return m_network; };
    CDaikinCtrl& GetDaikinCtrl() { return m_daikinCtrl; };

    void MqttRequestRepublish() { m_bMqttRepublish = true; m_mqttRepublishTimer = 0; };

  private:
    CNetwork m_network;
    CDaikinCtrl m_daikinCtrl;
    bool m_bMqttRepublish = false;
    elapsedMillis m_mqttRepublishTimer = 0;
    CCtrl4DknCommandHandler m_commandHandler;
    CTerminal m_terminal;

    elapsedMillis m_ledTimer = 0;
};
#endif // APP_H
