/*
 * Authors: 1. Dimitrios Antoniadis
 *          2. Vasileios Dimitriadis
 *          
 * email:   1. akdimitri@auth.gr          
 *          2. dimvasdim@auth.gr
 *          
 * University:  Aristotle University of Thessaloniki (AUTH)         
 * Semester:    8th 
 * Subject:     Microprocessors and Peripherals  
 * 
 * Parts required:
 * - one Wemos D1 R2 (ESP8266)
 * - one TMP36 temperature sensor
 * - one LCD 16x2 JHD659 M10 1.1
 * - one HC-SR04 proximity sensor
*/

/* include Libraries */
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>
#include "Gsender.h"

/* Global Variable Declaration */
Ticker INTERRUPT;
const char* ssid = "WIFI-NAME";                             // WIFI network name
const char* password = "WIFI-PASSWORD";              // WIFI network password
uint8_t connection_state = 0;                               // Connected to WIFI or not
uint16_t reconnect_interval = 10000;                        // If not connected wait time to try again
const int sensorPin = A0;
int i, isSomeoneClose = 0;
float  sum, temperature[24], averageTemperature;
const float extremeLow = 21.5, extremeHigh = 22.5, extremeFan = 23;
const int trigPin = 12, echoPin = 13;    
unsigned long duration, cm, inches;
bool FAN = false;

// initialize the LCD function with the numbers of the interface pins
// D0 -> GPIO16
// D1 -> GPIO5
// D2 -> GPIO4
// D3 -> GPIO0
// D4 -> GPIO2
// D5 -> GPIO14
// D6 -> GPIO12
// D7 -> GPIO13
LiquidCrystal lcd(16, 5, 4, 0, 2, 14);


/* WiFiConnect: this function is responsible for connecting 
 *              MCU to WiFi.
 */
uint8_t WiFiConnect(const char* nSSID = nullptr, const char* nPassword = nullptr)
{
    static uint16_t attempt = 0;
    Serial.print("Connecting to ");
    if(nSSID) {
        WiFi.begin(nSSID, nPassword);  
        Serial.println(nSSID);
    } else {
        WiFi.begin(ssid, password);
        Serial.println(ssid);
    }

    uint8_t i = 0;
    while(WiFi.status()!= WL_CONNECTED && i++ < 50)
    {
        delay(200);
        Serial.print(".");
    }
    ++attempt;
    Serial.println("");
    if(i == 51) {
        Serial.print("Connection: TIMEOUT on attempt: ");
        Serial.println(attempt);
        if(attempt % 2 == 0)
            Serial.println("Check if access point available or SSID and Password\r\n");
        return false;
    }
    Serial.println("Connection: ESTABLISHED");
    Serial.print("Got IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}


/* Awaits(): this function tries continuously to connect to WiFi.
 *           It stops when connection has been established.
 */
void Awaits()
{
    uint32_t ts = millis();
    while(!connection_state)
    {
        delay(50);
        if(millis() > (ts + reconnect_interval) && !connection_state){
            connection_state = WiFiConnect();
            ts = millis();
        }
    }
}


/* checkProximity(): this function is responsible for acknowledging wheter 
 *                   someone is close or not.
 */
int checkProximity(){
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  if( duration = pulseIn(echoPin, HIGH)){ 
    // Convert the time into a distance
    cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
    inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
    //Serial.print(inches);
    //Serial.print("in, ");
    //Serial.print(cm);
    //Serial.print("cm");
    //Serial.println();
    if( cm < 10){
      Serial.println("Somebody is close: " + String(cm) + "cm" );
      return 1;
    }
    else{
      return 0;
    }
  }
  else{
    Serial.println("Nobody Nearby");
    return 0;
  } 
}


/* printLCD(): this function is responsible for
 *             printing average temperature to
 *             LCD screen.
 */
void printLCD( String line1, String line2){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}


/* interruptVector(): this function checks periodically every 0.5 seconds
 *                    if someone is nearby. Then it prints average Temperature 
 *                    and the latest temperature measurment.
 */
void interruptVector(){

  // check if someone is close
  isSomeoneClose = checkProximity();
  String line1, line2;
  if( isSomeoneClose == 1){
    line1 = String("AVG:" + String( averageTemperature, 3));
    if(i == 0)
      line2 = String("Last TEMP: " + String(temperature[24], 3));
    else
      line2 = String("Last TEMP: " + String(temperature[i-1], 3));

      printLCD( line1, line2);      
  }
}
/* checkExtremeValues: this function is responsible for cheking wether
 *                     temperature is Greater than a high value or
 *                     Lower than a low value. Accordingly, this function
 *                     turns on/off BLUE/RED LEDS and prints respectively 
 *                     on LCD the suitable message. LCD is used only after
 *                     i > 0 and that's because only then 10 secs have elapsed
 *                     since previous average value print.
 */
void checkExtremeValues( float temperature, int i){
  String line1, line2;
  bool printMade = false;
  
  if( temperature < extremeLow){
        if( i >0 ){      
        line1 = String("Warning: LOW");
        line2 = String( "Temp      " + String(temperature, 3));
        printLCD( line1, line2);
        printMade = true;
      }
      //digitalWrite(BLUE, HIGH);      
   }
   else{
    //digitalWrite(BLUE, LOW);  
   }
   
   if( temperature > extremeHigh){
      if( i > 0){
        line1 = String("Warning: HIGH");
        line2 = String( "Temp      " + String(temperature, 3));
        printLCD( line1, line2);
        printMade = true;
      }
      //digitalWrite(RED, HIGH);         
    }
    else{
      //digitalWrite(RED, LOW);
    }
    
    if( printMade == false && i > 0)     
       printLCD("Calculating", " ");    
}


/* checkExtremeFan(): this function is responsible 
 *                    for checking whether fan is needed
 *                    to be turned on
 */
void checkExtremeFan(float averageTemperature){

  // check fan
  if( averageTemperature > extremeFan){
    if( FAN == false){
      Serial.println("1.FAN is " + String(FAN) + ": TURN it ON");
      //digitalWrite(GREEN, HIGH);
      Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
      String subject = "FAN = ON!";
      String value = String(averageTemperature, 3);
      String message = String("FAN TURNED ON : " + value);
      if(gsender->Subject(subject)->Send("RECEIVER-EMAIL", message)) {
          Serial.println("Message send.");
      } else {
          Serial.print("Error sending message: ");
          Serial.println(gsender->getError());
      }
      FAN = true;
    }
    else{
      Serial.println("2.FAN is " + String(FAN) + ": KEEP it ON");   
    }
  }
  else{
    FAN = false;
    Serial.println("3.FAN is " + String(FAN));
    //digitalWrite(GREEN, LOW);
  }   
}

/* readTemperature(): this function is responsible for reading 
 * temperature sensor and returning temperature value.
 */
float readTemperature(){
  // convert the ADC reading to voltage   
  // convert the voltage to temperature in degrees C
  // the sensor changes 10 mV per degree
  // the datasheet says there's a 500 mV offset
  // ((voltage - 500 mV) times 100)
  int sensorVal = analogRead(sensorPin);    // read TMP36 value in Volts
  float voltage = (sensorVal / 1024.0) * 3.1;         // 3.1 is used after calibration. Nomrally 3.3 should be used
  float temperature = (voltage - .5) * 100;              // since 3.3 V is the Operation Vlotage of MCU.
  String printLine = String(String(i) + ". sensor Value: " + String(sensorVal) + ", Volts: " + String(voltage, 3) + ", degrees C: " + String(temperature, 3));
  Serial.println(printLine);
  return temperature;
}

/* setup(): This function is executed only once at the POWER ON 
 *          or at RESET
 */
void setup()
{
  Serial.begin(115200);
  connection_state = WiFiConnect();
  if(!connection_state)  // if not connected to WIFI
      Awaits();          // constantly trying to connect

    /* Setup LCD */
  lcd.begin(16, 2);  
  lcd.print("ARDUINO");  
  lcd.setCursor(0, 1);  
  lcd.print("PROJECT");


  // HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);   

  /* Setup ticker function */
  INTERRUPT.attach( 0.5, interruptVector);

  /* Setup Serial Communication */
  Serial.begin(115200);
  
}

/* loop(): this function is the main function. It executes
 *         continuously.
 */
void loop(){
  sum = 0;                    // this variable holds the sum of temperatures

  /* 2 minutes Loop */
  Serial.println("\n\n\nNew Measurement");
  for( i = 0; i < 24; i++){
    delay(5*1000);
    temperature[i] = readTemperature();    
    sum = sum + temperature[i];     
    checkExtremeValues( temperature[i], i);        
 }

 /* Calculate Average Temperature */
  averageTemperature = sum/24;
  String line1 = String("Avg. Temp.");
  String line2 = String(averageTemperature, 3);
  printLCD( line1, line2);
  Serial.println("\n\n\n Average Temperature: " + String(averageTemperature, 3));
  
  //check for fan
  checkExtremeFan( averageTemperature);
  
}
