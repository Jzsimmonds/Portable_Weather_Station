#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include <ESPWiFi.h>
#include <ESPHTTPClient.h>
#include <JsonListener.h>
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
int rainPin = A0;
int thresholdValue = 500;
#include "DHTesp.h" // Library for DHT22 Sensor. Click here to get the library: http://librarymanager/All#DHTesp
#ifdef ESP32 //Model board
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)  //only for ESP8266 Board
#error Select ESP8266 board.  //Model of ESP
#endif //
DHTesp dht;

const char* ssid = "MARS";                        //SSID of the router.
const char* password = "123JASG6"; 

unsigned long channelID = 1363791;                //ID of the channel.
const char* WriteAPIKey = ""; 

WiFiClient Client;

void setup() {
  Serial.begin(115200);
  
WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi conectado!");

  ThingSpeak.begin(Client);
  
  String thisBoard= ARDUINO_BOARD;
  Serial.println(thisBoard);

  dht.setup(0, DHTesp::DHT22); // Connect DHT sensor to GPIO 0, that´s mean D3 in ESP8266
  
  if (!bmp.begin()) {
       Serial.println("No BMP180 / BMP085");// we dont wait for this
    //   while (1) {}
  }
  pinMode(rainPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  learnbmp();

  delay(2000);
  learnrain();

  delay(2000);
  learnlight();

  delay(2000);
  learndht();

  ThingSpeak.writeFields(channelID,WriteAPIKey);
  Serial.println("\nDatos enviados a ThingSpeak!");
  delay(14000);
  
}

void learnbmp(){
  String P = String(bmp.readPressure()) + "Pa";
  String A = String(bmp.readAltitude(275000)) + " msnm";// insert pressure at sea level

Serial.print("Pressure: ");
Serial.print(P);
Serial.print(" Pascal ");
Serial.print("\nAltitude: ");
Serial.print(A);
Serial.println(" meters above sea level");

  ThingSpeak.setField (3,P);
  ThingSpeak.setField (5,A);
  
  }

void learnrain(){
  int sensorValue = analogRead(rainPin);
  Serial.print(sensorValue);

  if(sensorValue < thresholdValue){
    Serial.println(" It´s Raining ");
    Serial.print("---------------------------------");
    }else {
      Serial.println(" Not Raining ");
      Serial.print("--------------------------------- \n");
      }
        ThingSpeak.setField (6,sensorValue);
  }

void learnlight(){
float volts = analogRead(A0) * 5.0 / 1024.0;
float amps = volts / 10000.0; // across 10,000 Ohms
float microamps = amps * 1000000;
float lux = microamps * 2.0;
Serial.print(lux);
Serial.print(" lux \n");

    ThingSpeak.setField (4,lux);
  
  }

void learndht(){

  delay(dht.getMinimumSamplingPeriod());

  float h = dht.getHumidity();
  float t = dht.getTemperature();
  float hic = dht.computeHeatIndex(t, h, false);

  float gamma = log(h/100) + ((17.62*t) / (243.5+t));
  float dp = 243.5*gamma / (17.62-gamma);
  
  Serial.print("Temperature:  ");
  Serial.print(t);
  Serial.print(" ºC.");
  Serial.print("\nHumidity:  ");
  Serial.print(h);
  Serial.print(" %.");
  Serial.print("\nHeat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print("\nDew point: ");
  Serial.print(dp);
  Serial.print(" ºC.");
  Serial.println();

    ThingSpeak.setField (1,t);
    ThingSpeak.setField (2,h);
    ThingSpeak.setField (7,hic);
    ThingSpeak.setField (8,dp);
  
  }
