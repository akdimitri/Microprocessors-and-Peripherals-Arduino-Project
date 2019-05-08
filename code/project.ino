/*
  Arduino Starter Kit example
  Project 3 - Love-O-Meter

  This sketch is written to accompany Project 3 in the Arduino Starter Kit

  Parts required:
  - one TMP36 temperature sensor
  - three red LEDs
  - three 220 ohm resistors

  created 13 Sep 2012
  by Scott Fitzgerald

  http://www.arduino.cc/starterKit

  This example code is part of the public domain.
*/
// include the library code:
#include <LiquidCrystal.h>
float temperatureFunction();
void printLCD( float averageTemperature);
void checkExtremeValues(float averageTemperature);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// named constant for the pin the sensor is connected to
const int sensorPin = A0;
const int BLUE = 8;
const int RED = 9;
const int GREEN = 13;
const float extremeLow = 21.5;
const float extremeHigh = 22.5;
const float extremeFan = 23;
float temperature[24], averageTemperature;
int i;

void setup() {

  //LEDS
  // set the digital pin as output:
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(GREEN, LOW);
  
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
  
  // open a serial connection to display values
  Serial.begin(9600);
}

void loop() {
  averageTemperature = 0;
  
  for( i = 0; i < 24; i++){
    // delay 5 seconds = 5 * 1000 milliseconds
    delay(5*1000);

    if( i == 1){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CALCULATING...");
    }
    // read temperature in Celsius
    temperature[i] = temperatureFunction();    
  }

  for( i = 0; i < 24; i++){  
    averageTemperature = averageTemperature + temperature[i];
  }
  averageTemperature = averageTemperature/24;
  printLCD( averageTemperature);
  Serial.print("Average Temperature: ");
  Serial.println(averageTemperature);

  //check for fan
  checkExtremeValues( averageTemperature);
    
}

float temperatureFunction(){
  // read the value on AnalogIn pin 0 and store it in a variable
  int sensorVal = analogRead(sensorPin);

  // send the 10-bit sensor value out the serial port
  Serial.print("sensor Value: ");
  Serial.print(sensorVal);

  // convert the ADC reading to voltage
  float voltage = (sensorVal / 1024.0) * 5.0;

  // Send the voltage level out the Serial port
  Serial.print(", Volts: ");
  Serial.print(voltage);

  // convert the voltage to temperature in degrees C
  // the sensor changes 10 mV per degree
  // the datasheet says there's a 500 mV offset
  // ((voltage - 500 mV) times 100)
  Serial.print(", degrees C: ");
  float temperature = (voltage - .5) * 100;
  Serial.println(temperature);

 return temperature;
}

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

void checkExtremeValues(float averageTemperature){

  // check fan
  if( averageTemperature > extremeFan){
     digitalWrite(GREEN, HIGH);
  }
  else{
    digitalWrite(GREEN, LOW);
  }

   // check extreme values
  if( averageTemperature < extremeLow){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WARNING");
    lcd.setCursor(0, 1);
    lcd.print("TEMP < ");
    lcd.print(extremeLow);
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, LOW);
  }
  else if( averageTemperature > extremeHigh){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WARNING");
    lcd.setCursor(0, 1);
    lcd.print("TEMP > ");
    lcd.print(extremeHigh);
    digitalWrite(RED, HIGH); 
    digitalWrite(BLUE, LOW);   
  }
  else{
    // clear LEDS
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    //lcd.print("CALCULATING...");
  }
}
