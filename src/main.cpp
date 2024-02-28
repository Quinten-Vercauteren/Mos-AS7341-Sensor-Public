#include <Arduino.h>
#include <WiFiS3.h>
#include <secrets.h>
#include <Wire.h>
#include <Adafruit_AS7341.h>

#define LED_GROEN 4
#define LED_GEEL 3
#define LED_ROOD 5

int status = WL_IDLE_STATUS;

// Millis
unsigned long millisDataSturen = 0;
unsigned long millisTime = 0;

// Function to initialize Led's
void initLed()
{
  pinMode(LED_GROEN, OUTPUT);
  pinMode(LED_GEEL, OUTPUT);
  pinMode(LED_ROOD, OUTPUT);

  digitalWrite(LED_GROEN, LOW); // Initialize LED_GROEN as off
  digitalWrite(LED_GEEL, LOW);  // Initialize LED_GEEL as off
  digitalWrite(LED_ROOD, LOW);  // Initialize LED_ROOD as off
}

void printWifiStatus()
{
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("Signal Strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// sensor code
Adafruit_AS7341 as7341;

// Function that collects data from the sensor and sends it to sheets
void sendDataSheets()
{
  digitalWrite(LED_ROOD, HIGH);

  // Read all channels at the same time and store in as7341 object
  if (!as7341.readAllChannels()){
    Serial.println("Error reading all channels!");
    return;
  }

  WiFiSSLClient client;

  // Get the values from the sensor
  String f1 = String(as7341.getChannel(AS7341_CHANNEL_415nm_F1));
  String f2 = String(as7341.getChannel(AS7341_CHANNEL_445nm_F2));
  String f3 = String(as7341.getChannel(AS7341_CHANNEL_480nm_F3));
  String f4 = String(as7341.getChannel(AS7341_CHANNEL_515nm_F4));
  String f5 = String(as7341.getChannel(AS7341_CHANNEL_555nm_F5));
  String f6 = String(as7341.getChannel(AS7341_CHANNEL_590nm_F6));
  String f7 = String(as7341.getChannel(AS7341_CHANNEL_630nm_F7));
  String f8 = String(as7341.getChannel(AS7341_CHANNEL_680nm_F8));
  String clear = String(as7341.getChannel(AS7341_CHANNEL_CLEAR));
  String nir = String(as7341.getChannel(AS7341_CHANNEL_NIR));


  // Construct the final URL for the Google Sheets script
  String urlFinal = GOOGLE_APPS_SCRIPT_URL + GOOGLE_SCRIPT_ID + "/exec?f1=" + f1 + "&f2=" + f2 + "&f3=" + f3 + "&f4=" + f4 + "&f5=" + f5 + "&f6=" + f6 + "&f7=" + f7 + "&f8=" + f8 + "&clear=" + clear + "&nir=" + nir;
  if (client.connect("script.google.com", 443))
  {
    Serial.println("connected to server");

    // Send HTTP GET request
    client.println("GET " + urlFinal + " HTTP/1.1");
    client.println("Host: script.google.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    client.stop();
  }
  else
  {
    client.stop();
    Serial.println("No client connect");
  }
  digitalWrite(LED_ROOD, LOW);
}

void setup()
{
  initLed();
  Serial.begin(9600);

  digitalWrite(LED_GROEN, HIGH);

  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(1000);
    digitalWrite(LED_GEEL, HIGH);
  }

  printWifiStatus();

  if (!as7341.begin())
  {
    Serial.println("Could not find AS7341");
    while (1)
    {
      delay(10);
    }
  }

  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);

  Serial.println("\nStarting connection to server...");
}

void loop()
{
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(1000);
    digitalWrite(LED_GEEL, HIGH);
  }

  Serial.println("still running 1");

  if (millis() >= millisDataSturen + (60000 * 15))
  {
    Serial.println("still running 2");
    millisDataSturen = millis();
    sendDataSheets();
  }
  Serial.println("still running 3");
}
