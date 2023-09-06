
/**********************************************************************************
 *  Download Board ESP32 : https://github.com/espressif/arduino-esp32
 *
 *  Blynk Library (1.1.0):  https://github.com/blynkkk/blynk-library
 **********************************************************************************/
/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID "TMPL2xBQQNHfq"
#define BLYNK_TEMPLATE_NAME "Smart Home"
#define BLYNK_AUTH_TOKEN "e2MT2g-ZkJiJtiwwV8WgrpfJQP8RgX3E"

//WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "MTN_4G_457AA7";
char pass[] = "mifiiPass";

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display);
#define BLYNK_PRINT Serial
#include "EmonLib.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h> 
#include <Preferences.h>

Preferences pref;
EnergyMonitor emon;
BlynkTimer timer;
 

float kWh = 0;
unsigned long lastmillis = millis();
int wifiFlag = 0;
char auth[] = BLYNK_AUTH_TOKEN;


void myTimerEvent()
{
  emon.calcVI(20, 2000);
  kWh = kWh + emon.apparentPower * (millis() - lastmillis) / 3600000000.0;
  yield();
  Serial.print("Vrms: ");
  Serial.print(emon.Vrms, 2);
  Serial.print("V");
 
  Serial.print("\tIrms: ");
  Serial.print(emon.Irms, 2);
  Serial.print("A");
 
  Serial.print("\tApparent Power: ");
  Serial.print(emon.apparentPower, 2);
  Serial.print("W");
 
  Serial.print("\tkWh: ");
  Serial.print(kWh, 3);
  Serial.println("kWh");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vrms:");
  lcd.print(emon.Vrms, 2);
  lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("Irms:");
  lcd.print(emon.Irms, 2);
  lcd.print("A");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Power:");
  lcd.print(emon.apparentPower, 3);
  lcd.print("W");
  lcd.setCursor(0, 1);
  lcd.print("kWh:");
  lcd.print(kWh, 3);
  lcd.print("W");
  delay(2000);
 
  lastmillis = millis();
 
  Blynk.virtualWrite(V5, emon.Vrms);
  Blynk.virtualWrite(V6, emon.Irms);
  Blynk.virtualWrite(V7, emon.apparentPower);
  Blynk.virtualWrite(V8, kWh);
}
 
// define the GPIO connected with Relays and switches
#define RelayPin1 33  //D33
#define RelayPin2 25  //D25
#define RelayPin3 26  //D26
#define RelayPin4 27  //D27
#define wifiLed   14   //D14
//Voltage = PIN 12
//Current = PIN 13


//Change the virtual pins according the rooms
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2
#define VPIN_BUTTON_3    V3
#define VPIN_BUTTON_4    V4


// Relay State
bool toggleState_1 = LOW; //Define integer to remember the toggle state for relays
bool toggleState_2 = LOW; 
bool toggleState_3 = LOW; 
bool toggleState_4 = LOW; 


// When App button is pushed - switch the state

BLYNK_WRITE(VPIN_BUTTON_1) {
  toggleState_1 = param.asInt();
  digitalWrite(RelayPin1, !toggleState_1);
  pref.putBool("Relay1", toggleState_1);
  Serial.println("Bulb 1 On");  //For debugging
}

BLYNK_WRITE(VPIN_BUTTON_2) {
  toggleState_2 = param.asInt();
  digitalWrite(RelayPin2, !toggleState_2);
  pref.putBool("Relay2", toggleState_2);
  Serial.println("Bulb 2 On");
}

BLYNK_WRITE(VPIN_BUTTON_3) {
  toggleState_3 = param.asInt();
  digitalWrite(RelayPin3, !toggleState_3);
  pref.putBool("Relay3", toggleState_3);
  Serial.println("Bulb 3 On");
}

BLYNK_WRITE(VPIN_BUTTON_4) {
  toggleState_4 = param.asInt();
  digitalWrite(RelayPin4, !toggleState_4);
  pref.putBool("Relay4", toggleState_4);
  Serial.println("Fan On");
}

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    wifiFlag = 1;
    digitalWrite(wifiLed, LOW);
    Serial.println("Blynk Not Connected");
  }
  if (isconnected == true) {
    wifiFlag = 0;
    digitalWrite(wifiLed, HIGH);
     //Serial.println("Blynk Connected");
  }
}

BLYNK_CONNECTED() {
  // update the latest state to the server
  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
  Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
  Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
}


void getRelayState()
{
  //Serial.println("reading data from NVS");
  toggleState_1 = pref.getBool("Relay1", 0);
  digitalWrite(RelayPin1, !toggleState_1); 
  Blynk.virtualWrite(VPIN_BUTTON_1, toggleState_1);
  delay(200);
  toggleState_2 = pref.getBool("Relay2", 0);
  digitalWrite(RelayPin2, !toggleState_2); 
  Blynk.virtualWrite(VPIN_BUTTON_2, toggleState_2);
  delay(200);
  toggleState_3 = pref.getBool("Relay3", 0);
  digitalWrite(RelayPin3, !toggleState_3); 
  Blynk.virtualWrite(VPIN_BUTTON_3, toggleState_3);
  delay(200);
  toggleState_4 = pref.getBool("Relay4", 0);
  digitalWrite(RelayPin4, !toggleState_4); 
  Blynk.virtualWrite(VPIN_BUTTON_4, toggleState_4);
  delay(200);
}

void setup()
{
  Serial.begin(115200);
  pref.begin("Relay_State", false);
  
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  pinMode(RelayPin4, OUTPUT);

  pinMode(wifiLed, OUTPUT);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, !toggleState_1);
  digitalWrite(RelayPin2, !toggleState_2);
  digitalWrite(RelayPin3, !toggleState_3);
  digitalWrite(RelayPin4, !toggleState_4);


  Blynk.begin(auth, ssid, pass);    //MAKE SURE THE BLYNK LIBRARY IS V1.1.0
  lcd.init();  //SDA--GPIO 21, SCL--GPIO 22
  lcd.backlight();

  timer.setInterval(1000, checkBlynkStatus); // check if Blynk server is connected every 1 seconds


  getRelayState(); //fetch data from NVS Flash Memory
//  delay(500);
 
  emon.voltage(12, 83.3, 1.7); // Voltage: input pin, calibration = , 234.26, phase_shift
  emon.current(13, 0.50); // Current: input pin, calibration = 0.50,  111.1
 
  timer.setInterval(500, myTimerEvent);
  lcd.setCursor(3, 0);
  lcd.print("IoT Energy");
  lcd.setCursor(5, 1);
  lcd.print("Meter");
  delay(2000);
  lcd.clear();

}

void loop()
{  
  Blynk.run();
  timer.run(); // Initiates SimpleTimer

}
