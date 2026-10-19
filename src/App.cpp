#include "Terminal.h"
#include "util.h"
#include "App.h"

CApp::CApp() : m_daikinCtrl(m_network), m_commandHandler(m_daikinCtrl, m_network), m_terminal(m_network)
{
}


void CApp::Init()
{
  m_network.Init();

  m_daikinCtrl.Reset();
}

bool CApp::CommandHandler()
{
  char* strTermCommand = m_terminal.GetCommand();
  if (strTermCommand == nullptr)
    return false;

//  noInterrupts(); // Enter critical section

  // Copy command since it *may* be modified by ProcessCommand
  char strCommand[CMD_BUF_SIZE];
  strcpy(strCommand, strTermCommand);

//  interrupts(); // Leave critical section

  // Parse command and get result
  const result_code_t resultCode = m_commandHandler.ProcessCommand(strCommand);

  // Finally output result-code string (OK or ERROR:)
  if (resultCode.code != ERR_CODE_OK_NULL)
  {
    char strResult[RESULT_BUF_SIZE];
    get_error_string(resultCode, strResult, false);
    CTerminal::print(strResult);
  }

  return true;
}


void CApp::HandleNetwork()
{
  m_network.Loop();

  if (m_network.HandleMqttClient())
  {
    // MQTT (re)connection: publish config (values follow after the republish delay)
    m_daikinCtrl.MqttOnConnect();
    MqttRequestRepublish();
  }

  // Delay republishing: HA subscribes to state topics of (newly) discovered entities only after processing
  // their config, so it would miss our (non-retained) values
  if (m_bMqttRepublish && m_mqttRepublishTimer > MQTT_SETTLE_TIME_MS)
  {
    m_bMqttRepublish = false;
    m_daikinCtrl.MqttRequestRepublish();
    m_network.MqttRequestRepublish();
  }

  if (!m_network.IsConnected() || !m_network.IsMqttConnected())
  {
#ifdef STATUS_LED
    digitalWrite(STATUS_LED, LOW); // Always on: failure
#endif
  }
  else
  {
    // Indicate we're running:
#ifdef STATUS_LED
    if (m_ledTimer > 2000)
    {
      digitalWrite(STATUS_LED, HIGH); // Off
      m_ledTimer = 0;
    }
    else if (m_ledTimer > 1000)
    {
      digitalWrite(STATUS_LED, LOW); // On
    }
#endif
  }
}


void CApp::Loop()
{
  // 1ms loop delay
  delay(1);

  HandleNetwork();

  // Poll serial & ethernet for commands
  m_terminal.Process();

  CommandHandler();

  m_daikinCtrl.Loop();
}
