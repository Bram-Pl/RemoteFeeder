#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <WiFi.h>
#include <../lib/credentials.h>
#include <ESP32Servo.h>

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
IPAddress ip(192, 168, 1, 5);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);

bool toggle = false;
int posServo = 0;
int stepDeg = 5;

int setupStep = 0;
String header;

TFT_eSPI tft = TFT_eSPI();
WiFiServer server(80);

Auth auth;
Servo servo;

void connectWiFi(Authentication creds);
String IPAddresstoString(const IPAddress &address);
void servoToStartPos();
void displayHomePage(WiFiClient client);
void displayPerformedTask(WiFiClient client);

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
  // Connect servo
  servo.attach(2);

  // WiFi Setup
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  servoToStartPos();
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
  else if(setupStep == 1)
  {
    //Check which requist is done to webserver
    WiFiClient client = server.available();
    Serial.print(client);
    if (!client){delay(1000);return;}
    Serial.println("New Client! Welcome!");
    while (!client.available())
    {
      delay(1);
    }
    String request = client.readStringUntil('\r');
    Serial.println(request);
    if (request.indexOf("deposit") != -1)
    {
      displayPerformedTask(client);
      stepDeg = 1;
      for (posServo = 180; posServo >= 105; posServo -= stepDeg) { // goes from 180 degrees to 0 degrees
        servo.write(posServo);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
      setupStep++;
      client.stop();
    }
    else if (request.indexOf("") != -1)
    {
      displayHomePage(client);
      client.stop();
    }    
  }
  else if(setupStep == 2) //For testing purposes
  {
    //Check which requist is done to webserver
    WiFiClient client = server.available();
    Serial.print(client);
    if (!client){delay(1000);return;}
    Serial.println("New Client! Welcome!");
    while (!client.available())
    {
      delay(1);
    }
    String request = client.readStringUntil('\r');
    Serial.println(request);
    if (request.indexOf("close") != -1)
    {
      stepDeg = 1;
      for (posServo = 105; posServo <= 180; posServo += stepDeg) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        servo.write(posServo);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
      setupStep--;
      client.stop();
    }
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
  // WiFi.config(ip, dns, gateway, subnet);
  WiFi.begin(creds.SSID, creds.PWD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    dualPrintf(".");
  }
  dualPrintf("\nWiFi Connected Succesfully\n\r");
  dualPrintf("IP Address: %s\n\r", IPAddresstoString(WiFi.localIP()));
  server.begin();
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

/**
 * @brief Put Servo back in start position
 * 
 */
void servoToStartPos()
{
  //Put servo position in start before WiFi initialization
  for (posServo = 105; posServo <= 180; posServo += stepDeg) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo.write(posServo);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

/**
 * @brief HTML code to send to client and display the homepage
 * 
 * @param client 
 */
void displayHomePage(WiFiClient client){
  // Display the HTML web page
    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    client.println("<title>Food Dispenser</title>");
    // Feel free to change the background-color and font-size attributes to fit your preferences
    client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
    client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
    client.println(".button2 {background-color: #555555;}img {border:1px solid #ddd;border-radius:4px;padding:5px;width:300px;}</style></head>");
    client.println("<body><h1>Frey's Food Dispenser</h1>");
    client.println("<p><a href=\"/deposit\"><button class=\"button\">DEPOSIT FOOD</button></a></p>");
    client.println("<img src=\"https://i.ibb.co/6Bg7BFz/IMG-20220813-135342.jpg\">");
    client.println("</body></html>");
}

/**
 * @brief HTML code to send to client and display the performed task, dispensing
 * 
 * @param client 
 */
void displayPerformedTask(WiFiClient client){
  // Display the HTML web page
    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    client.println("<title>Food Dispenser</title>");
    // Feel free to change the background-color and font-size attributes to fit your preferences
    client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client.println("img {border:1px solid #ddd;border-radius:4px;padding:5px;width:300px;}");
    client.println("#btndisclaimer{position:fixed;left:10px;bottom:60px;}</style></head>");
    client.println("<body><h1>Frey haar eten zit in haar kom!</h1>");
    client.println("<h2>Frey is nu blij!</h2>");
    client.println("<img src=\"https://i.ibb.co/xH0HMS4/IMG-20220601-143249.jpg\">");
    client.println("<div class=\"form-group\" style=\"margin-top: 12%; float:right; position: relative; \"><div class=\"col-xs-6 col-sm-6 col-md-6 col-lg-6\">");
    client.println("<a href=\"/close\"><button class=\"btn btn-primary btn-md\" id=\"btndisclaimer\">Close</button></a></div></div>\"");
    client.println("</body></html>");
}