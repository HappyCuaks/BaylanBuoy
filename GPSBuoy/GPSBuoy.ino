// Please select the corresponding model

#define SIM800L_AXP192_VERSION_20200327

// Define the serial console for debug prints, if needed
#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG          SerialMon

#include "utilities.h"

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to the module)
#define SerialAT  Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800          // Modem is SIM800
#define TINY_GSM_RX_BUFFER      1024   // Set RX buffer to 1Kb

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

// Set phone numbers, if you want to test SMS and Calls
#define SMS_TARGET  "+41782174313"
#define CALL_TARGET "+380xxxxxxxxx"


bool waitForResponse(const char* expectedResponse, unsigned long timeoutMillis = 5000);


void setup()
{
    // Set console baud rate
    SerialMon.begin(115200);

    delay(10);

    // Start power management
    if (setupPMU() == false) {
        Serial.println("Setting power error");
    }

    // Some start operations
    setupModem();

    // Set GSM module baud rate and UART pins
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

    delay(6000);
}

void loop()
{
    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.println("Initializing modem...");
    modem.restart();


    delay(10000);

    String imei = modem.getIMEI();
    DBG("IMEI:", imei);

    DBG("------------Enter PIN------------------");
    // Unlock the SIM card with your PIN
    SerialAT.println("AT+CPIN=\"2268\"");
    delay(5000); // Wait for the SIM card to unlock

    DBG("------------Sending SMS------------------");
    // Check if the module has responded with "OK"
    if (waitForResponse("SMS Ready"))
    {
      DBG("Ready to send SMS");
    }
    // Do nothing forevermore
    while (true) {
        /*bool  res = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);
        DBG("SMS:", res ? "OK" : "fail");*/
        modem.maintain();
    }
}

bool waitForResponse(const char* expectedResponse, unsigned long timeoutMillis)
{
  unsigned long startTime = millis();
  
  while (millis() - startTime < timeoutMillis)
  {
    if (SerialAT.find(expectedResponse))
    {
      return true;  // Response found, return true
    }
  }
  
  return false;  // Timeout reached, return false
}