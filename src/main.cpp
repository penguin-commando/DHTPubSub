// Definiciones y librerias
#include <UbidotsEsp32Mqtt.h>

#include "DHT.h"
#include <TFT_eSPI.h>
#include <SPI.h>

#define DHTPIN 27
#define DHTTYPE DHT11

#define LEDPin 26

DHT dht(DHTPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();

// Define Constants
const char *UBIDOTS_TOKEN = "BBFF-qzKGCY0knHsGaELFUHuM0D16zyOLB6"; // Put here your Ubidots TOKEN
const char *WIFI_SSID = "UPBWiFi";                                 // Put here your Wi-Fi SSID
const char *WIFI_PASS = "";                                        // Put here your Wi-Fi password
const char *DEVICE_LABEL = "TESP32";                               // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL1 = "temp";                              // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL2 = "hmdd";                              // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL3 = "SW1";
const char *VARIABLE_LABEL4 = "SW2";
const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds
unsigned long timer;

int bsw1 = 0;
int bsw2 = 0;
char str1[] = "/v2.0/devices/tesp32/sw1/lv";
char *buff = "this is a test string";
Ubidots ubidots(UBIDOTS_TOKEN);

// Auxiliar Functions
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);

    if ((char)payload[0] == '1')
    {
      // digitalWrite(LED, HIGH);
      if (strcmp(str1, topic) == 0)
      {
        bsw1 = 1;
      }
      else
      {
        bsw2 = 1;
      }
    }
    else
    {
      // digitalWrite(LED, LOW);
      if (strcmp(str1, topic) == 0)
      {
        bsw1 = 0;
      }
      else
      {
        bsw2 = 0;
      }
    }
  }

  Serial.println();
}

// Setup del programa
void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  timer = millis();

  pinMode(LEDPin, OUTPUT);

  Serial.println(F("DHTxx test!"));
  dht.begin();

  tft.init();
  tft.fillScreen(0x0000);

  ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL3);
  ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL4);
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(5000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  if (!ubidots.connected())
  {
    ubidots.reconnect();
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL3);
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL4);
  }
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    ubidots.add(VARIABLE_LABEL1, t); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);

    ubidots.add(VARIABLE_LABEL2, h); // Insert your variable Labels and the value to be sent
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();

  tft.drawString("Temp. (°C):", 0, 30, 4);
  tft.drawString(String(t), 0, 60, 7);

  tft.drawString("HMDD (%):", 0, 130, 4);
  tft.drawString(String(h), 0, 160, 7);

  Serial.print(F("% Humedad: "));
  Serial.println(h);
  Serial.print(F("Temperatura: "));
  Serial.print(t);
  Serial.println(F("°C "));
  Serial.println();

  // Switch 1
  if (bsw1 == 1)
  {
    tft.fillCircle(10, 10, 10, TFT_RED); //(X,Y,radio,color)
    digitalWrite(LEDPin, HIGH);
  }
  else
  {
    tft.fillCircle(10, 10, 10, TFT_DARKGREY); //(X,Y,radio,color)
    digitalWrite(LEDPin, LOW);
  }

  // Switch 2
  if (bsw2 == 1)
  {
    tft.fillCircle(120, 10, 10, TFT_GREEN); //(X,Y,radio,color)
  }
  else
  {
    tft.fillCircle(120, 10, 10, TFT_DARKGREY); //(X,Y,radio,color)
  }
}