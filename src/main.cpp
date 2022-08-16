#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <../lib/credentials.h>

#define button_0 0
#define button_1 35
#define width 240
#define height 135
#define pinRead(pin) \
  !digitalRead(pin)

#define dualPrintf(...)       \
  Serial.printf(__VA_ARGS__); \
  tft.printf(__VA_ARGS__);

String locOne = "Huize Plessers";
String locTwo = "Casa Loiacono";
String location;

bool toggle = false;

int setupStep = 0;
String header;

TFT_eSPI tft = TFT_eSPI();
WiFiServer server(80);

Auth auth;

void connectWiFi(Authentication creds);
String IPAddresstoString(const IPAddress &address);

void setup()
{
  Serial.begin(115200);
  // Declaration of the two buttons, button_0 immediatly connected to the ground, button_1 connected to Vcc
  pinMode(button_0, INPUT_PULLUP);
  pinMode(button_1, INPUT);
  // TFT Setup
  tft.init();
  tft.setRotation(3); // Rotate screen 270 degrees
  tft.setCursor(0, 0, 2);
  // tft.setTextColor(TFT_WHITE); //No background but text does not get overwritten.
  tft.setTextColor(TFT_WHITE, TFT_TRANSPARENT);
  tft.fillScreen(TFT_BLACK);
  tft.fillRectHGradient(0, 0, width, height, TFT_BLUE, TFT_GREEN);

  int setupStep = 0; // Increment in which step the program is present

  // WiFi Setup
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop()
{
  Serial.println("Hello Jarni");
  // First Setup step, WiFi SSID and PWD. button_1 -> submit, button_0 -> toggle
  if (pinRead(button_1) == 0 && setupStep == 0)
  {
    delay(250);
    if (pinRead(button_0) == 1)
    {
      toggle = !toggle;
    }

    tft.setCursor(0, 0, 2);
    tft.println("Selecteer de locatie:");

    if (toggle)
    {
      tft.println(locOne);
      location = locOne;
    }
    else if (!toggle)
    {
      tft.println(locTwo);
      location = locTwo;
    }
  }
  else if (pinRead(button_1) == 1 && setupStep == 0)
  {
    if (location.equals(locOne))
    {
      connectWiFi(auth.getPlessers());
    }
    else if (location.equals(locTwo))
    {
      connectWiFi(auth.getLoiacono());
    }
    setupStep++;
  }
  delay(1000);
}

/**
 * @brief Connect to WiFi using credentials obtained above in class Auth
 * 
 * @param creds Authentication
 */
void connectWiFi(Authentication creds)
{
  dualPrintf("De gegevens voor deze locatie zijn: %s en %s\n\r", creds.SSID, creds.PWD);
  WiFi.begin(creds.SSID, creds.PWD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    dualPrintf(".");
  }
  dualPrintf("\nWiFi Connected Succesfully\n\r");
  dualPrintf("IP Address: %s\n\r", IPAddresstoString(WiFi.localIP()));
}

/**
 * @brief format type IPAddress to String
 * 
 * @param address IPAddress
 * @return String 
 */
String IPAddresstoString(const IPAddress &address)
{
  return String() + address[0] + "." + address[1] + "." + address[2] + "." + address[3];
}