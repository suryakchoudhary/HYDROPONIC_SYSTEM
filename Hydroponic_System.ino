#define PH_PIN 2
#define ACID_PIN 4
#define BASE_PIN 5
#define FAN_PIN 18
#define DHT11_PIN 19
#define LCD_SDA 21
#define LCD_SCL 22

#include <WiFi.h>       //include library for wifi
#include "ThingSpeak.h"  //include library for wifi
#include <DHT.h>        //include library for DHT11 sensor
#include <ThingESP.h>
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include "twilio.hpp"

ThingESP32 thing("suryakant", "SmartBin", "ESP32cp-2102");  ///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Enter data

Adafruit_LiquidCrystal lcd(0x27, 16, 2);  //RS,EN,D4,D5,D6,D7

void acid();
void base();

WiFiClient  client;
DHT dht(DHT11_PIN, DHT11);

//FOR ThingSpeak>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>ENTER DATA
unsigned long myChannelNumber = 1652835;           //channal id from thingspeak
const char * myWriteAPIKey = "CGQMNB4RTI07RHQC";   // write api kay from thingspeak
const int PH_VALUE_Field1 =1;                    //channel field 1 for Level
const int TEMPERATURE_Field2 =2;                          //channel field 2 for Temp.
const int HUMIDITY_Field3 = 3;                     //channel field 3 for Humidity

const int analogInPin = A0;
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10], temp = 0;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>TWILIO
// Set these - but DON'T push them to GitHub!
static const char *ssid = "";
static const char *password = "";

// Values from Twilio (find them on the dashboard)
static const char *account_sid = "";
static const char *auth_token = "";
// Phone number should start with "+<countrycode>"
static const char *from_number = "";

// You choose!
// Phone number should start with "+<countrycode>"
static const char *to_number = "";
char *message ;//= "Sent from my ESP32";

Twilio *twilio;
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>TWILIO

void setup() {
  Serial.begin(9600);

  pinMode(PH_PIN, OUTPUT);
  pinMode(ACID_PIN, OUTPUT);       //ACID
  pinMode(BASE_PIN, OUTPUT);       //BASE
  pinMode(DHT11_PIN, INPUT);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("HYDRPONIC");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM");
  delay(2000);
  lcd.clear();
  lcd.print("Measuring PH ");
  lcd.setCursor(0, 1);
  lcd.print("TEMP HUMIDITY");
  delay(2000);
  lcd.clear();

  dht.begin();

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>SET Wi-Fi USING ThingESP API BECZ IT ACCUPIES LESS LINES OF CODE
  //WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  thing.SetWiFi("Ferrari", "pawan@@0433kant");
  //thing.SetWiFi("iQOO", "surya00000kant12");
  Serial.println("\nConnected.");
  thing.initDevice();
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>SET Wi-Fi USING ThingESP API
}

void loop() 
{
  float temperature;
  float humidity;  
  humidity = dht.readHumidity();               //READ HUMIDITY
  temperature = dht.readTemperature();            //READ TEMP

  for (int i = 0; i < 10; i++)  //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(analogInPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++)  //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++) 
    {
      if (buf[i] > buf[j]) 
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)  //take the average value of 6 center sample
    avgValue += buf[i];

  float pHVol = (float)avgValue * 5.0 / 1024 / 4.3;  //convert the analog into millivolt
  float phValue = -5.70 * pHVol + 22.8;
  phValue = 14.2 - phValue;  //convert the millivolt into pH value
  //float phValue = -3.0 * pHVol+17.5;
  Serial.print("PH_VALUE: ");
  Serial.print(phValue);

  //SENDING TEXT MESSAGE THROUGH TWILIO >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>WARNING FOR PH VALUE
  if(phValue >= 10 || phValue <= 4)
  {
    //ASSIGNING MESSAGE TEXT TO BE SENT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>THROUGH Twilio
    if(phValue >= 10)
    {
      message = "THE PH VALUE IS GREATER >= 10";
    }
    else if(phValue <= 4)
    {
      message = "THE PH VALUE IS LESS <= 4";
    }
    //ASSIGNING MESSAGE TEXT TO BE SENT>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>THROUGH Twilio

    twilio = new Twilio(account_sid, auth_token);

  delay(1000);
  String response;
  bool success = twilio->send_message(to_number, from_number, message, response);
  if (success) {
    Serial.println("Sent message successfully!");
  } else {
    Serial.println(response);
  }
  }
 /* lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pH Value");
  lcd.setCursor(3, 1);
  lcd.print(phValue);
  delay(900);*/

  lcd.setCursor(0, 1);
  lcd.print("Temp.: ");
  lcd.print(temperature);

  if(temperature >= 30)                        //FAN
  {
    digitalWrite(FAN_PIN, HIGH);
  }
  else
  {
    digitalWrite(FAN_PIN, LOW);
  }

  if(phValue >=10)
  {
  void acid(); //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Acid on
  }

  if(phValue <=4)
  {
    void base();
  }
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

  ThingSpeak.writeField(myChannelNumber, PH_VALUE_Field1, phValue, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, TEMPERATURE_Field2, temperature, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, HUMIDITY_Field3, humidity, myWriteAPIKey);

  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

  // DEEP SLEEP MODE
  if (phValue >= 10)  // FILLED
  {
    esp_sleep_enable_timer_wakeup(2 * 1000000);  // Microsecond to second conversion factor
    Serial.println("Sleep for 2 seconds");
    esp_deep_sleep_start();
  } 
  else if (4 < phValue && phValue <= 9)  //MEDIUM FILLED
  {
    esp_sleep_enable_timer_wakeup(4 * 1000000);  // Microsecond to second conversion factor
    Serial.println("Sleep for 4 seconds");
    esp_deep_sleep_start();
  } 
  else if (phValue <= 4)  // EMPTY
  {
    esp_sleep_enable_timer_wakeup(2 * 1000000);  // Microsecond to second conversion factor
    Serial.println("Sleep for 8 seconds");
    esp_deep_sleep_start();
  }
}

void acid()                                    //PROVIDES ACID FOR 2 SECONDS
  {
      digitalWrite(ACID_PIN, HIGH);
      digitalWrite(BASE_PIN, LOW);
      lcd.setCursor(0, 0);
      lcd.print("POURING ACID");
      delay(2000);
    
  }

  void base()                                    //PROVIDES BASE FOR 2 SECONDS
  {
      digitalWrite(BASE_PIN, HIGH);
      digitalWrite(ACID_PIN, LOW);
      lcd.setCursor(0, 0);
      lcd.print("POURING BASE");
      delay(2000);
    
  }
