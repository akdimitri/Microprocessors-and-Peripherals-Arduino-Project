#include <ESP8266WiFi.h>
#include <LiquidCrystal.h>
#include "Gsender.h"

#pragma region Globals
const char* ssid = "Dimitris2";                             // WIFI network name
const char* password = "69451070306945558718";              // WIFI network password
uint8_t connection_state = 0;                               // Connected to WIFI or not
uint16_t reconnect_interval = 10000;                        // If not connected wait time to try again
const int sensorPin = A0;
int sensorVal, i;
float  sum, temperature[24], averageTemperature;
const float extremeLow = 21.5;
const float extremeHigh = 22.5;
const float extremeFan = 23;
const int trigPin = 12;    // Trigger
const int echoPin = 13;    // Echo
unsigned long duration, cm, inches;
int isSomeoneClose = 0;
#pragma endregion Globals

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


/* printLCD(): this function is responsible for
 *             printing average temperature to
 *             LCD screen.
 */
void printLCD( float averageTemperature){
  // clean up the screen before printing a new reply
  lcd.clear();
  // set the cursor to column 0, line 0
  lcd.setCursor(0, 0);
  // print some text
  lcd.print("AVG. TEMP.");
  // move the cursor to the second line
  lcd.setCursor(0, 1);
  lcd.print(averageTemperature);
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
  
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();
    if( cm < 10){
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


/* checkExtremeFan(): this function is responsible 
 *                    for checking whether fan is needed
 *                    to be turned on
 */
void checkExtremeFan(float averageTemperature){

  // check fan
  if( averageTemperature > extremeFan){
    //digitalWrite(GREEN, HIGH);
    Gsender *gsender = Gsender::Instance();    // Getting pointer to class instance
    String subject = "FAN = ON!";
    String value = String(averageTemperature, DEC);
    String message = String("FAN TURNED ON : " + value);
    if(gsender->Subject(subject)->Send("mitsos1996@yahoo.com", message)) {
        Serial.println("Message send.");
    } else {
        Serial.print("Error sending message: ");
        Serial.println(gsender->getError());
    }     
    
  }
  else{
    
    //digitalWrite(GREEN, LOW);
  }   
}




void setup()
{
    Serial.begin(115200);
    connection_state = WiFiConnect();
    if(!connection_state)  // if not connected to WIFI
        Awaits();          // constantly trying to connect


    //LCD
    // set up the number of columns and rows on the LCD
    lcd.begin(16, 2);
    // Print a message to the LCD.
    lcd.print("ARDUINO");
    // set the cursor to column 0, line 1
    // line 1 is the second row, since counting begins with 0
    lcd.setCursor(0, 1);
    // print to the second line
    lcd.print("PROJECT");


    // HC-SR04
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);   
}

void loop(){
  sum = 0;
  for( i = 0; i < 24; i++){

    // if 10 seconds elapsed since previous average temperature print clear LCD
    if( i == 1){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CALCULATING...");
    }
    
    // read the value on AnalogIn pin 0 and store it in a variable
    sensorVal= analogRead(sensorPin);

    // send the 10-bit sensor value out the serial port
    Serial.print("sensor Value: ");
    Serial.print(sensorVal);
  
    // convert the ADC reading to voltage
    float voltage = (sensorVal / 1024.0) * 3.1;
  
    // Send the voltage level out the Serial port
    Serial.print(", Volts: ");
    Serial.print(voltage);
  
    // convert the voltage to temperature in degrees C
    // the sensor changes 10 mV per degree
    // the datasheet says there's a 500 mV offset
    // ((voltage - 500 mV) times 100)
    Serial.print(", degrees C: ");
    temperature[i] = (voltage - .5) * 100;
    Serial.println(temperature[i]);

    // check extreme values
  if( temperature[i] < extremeLow && i > 0){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WARNING");
    lcd.setCursor(0, 1);
    lcd.print("TEMP < ");
    lcd.print(extremeLow);    
  }
  else if( temperature[i] > extremeHigh && i > 0){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WARNING");
    lcd.setCursor(0, 1);
    lcd.print("TEMP > ");
    lcd.print(extremeHigh);
    //digitalWrite(RED, HIGH); 
    //digitalWrite(BLUE, LOW);   
  }
  else if( i > 0){
    lcd.clear();   
  }
  

  // check if someone is close
  isSomeoneClose = checkProximity();
  
  if( isSomeoneClose == 1){
      // clean up the screen before printing a new reply
      lcd.clear();
      // set the cursor to column 0, line 0
      lcd.setCursor(0, 0);
      // print some text
      lcd.print("AVG. TEMP.:");
      lcd.print(averageTemperature);
      // move the cursor to the second line
      lcd.setCursor(0, 1);
      lcd.print("LAST TEMP.:");
      lcd.print(temperature[i]);
  }
  
  sum = sum + temperature[i];

  delay(5*1000);
      
 }

  averageTemperature = sum/24;
  
  checkExtremeFan( averageTemperature);
  
  // print average temperature
  printLCD( averageTemperature);
  Serial.print("Average Temperature: ");
  Serial.println(averageTemperature);
  
}
