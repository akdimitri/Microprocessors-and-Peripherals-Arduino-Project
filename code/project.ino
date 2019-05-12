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
 * - one TMP36 temperature sensor
 * - one LCD 16x2 JHD659 M10 1.1
 * - one HC-SR04 proximity sensor
*/

// include the library code:
#include <LiquidCrystal.h>

// functions declaration
float temperatureFunction();
void printLCD( float averageTemperature);
void checkExtremeFan(float averageTemperature);
int checkProximity();

// initialize the LCD function with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// global variables declaration
const int sensorPin = A0;
const int BLUE = 8;
const int RED = 9;
const int GREEN = 13;
const float extremeLow = 21.5;
const float extremeHigh = 22.5;
const float extremeFan = 23;
float temperature[24], averageTemperature;
float lastAverageTemperature = 0;
int i;
const int trigPin = 7;    // Trigger
const int echoPin = 6;    // Echo
unsigned long duration, cm, inches;
int isSomeoneClose = 0;

// setup function
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

  // HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // open a serial connection to display values
  Serial.begin(9600);
}

void loop() {
  
  averageTemperature = 0;

  // 24*5 secs = 2 minutes
  for( i = 0; i < 24; i++){
    // delay 5 seconds = 5 * 1000 milliseconds
    delay(5*1000);

    // if 10 seconds elapsed since previous average temperature print cleaer LCD
    if( i == 1){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CALCULATING...");
    }
    
    // read temperature in Celsius
    temperature[i] = temperatureFunction();

    // check if someone is close
    isSomeoneClose = checkProximity();
    
    if( isSomeoneClose == 1){
        // clean up the screen before printing a new reply
        lcd.clear();
        // set the cursor to column 0, line 0
        lcd.setCursor(0, 0);
        // print some text
        lcd.print("AVG. TEMP.:");
        lcd.print(lastAverageTemperature);
        // move the cursor to the second line
        lcd.setCursor(0, 1);
        lcd.print("LAST TEMP.:");
        lcd.print(temperature[i]);
    }
  }

  // calculate average temperature
  for( i = 0; i < 24; i++){  
    averageTemperature = averageTemperature + temperature[i];
  }
  averageTemperature = averageTemperature/24;

  // store average temperature
  lastAverageTemperature = averageTemperature;

  // print average temperature
  printLCD( averageTemperature);
  Serial.print("Average Temperature: ");
  Serial.println(averageTemperature);

  //check for fan
  checkExtremeFan( averageTemperature);
  
}

/***************
* Sub-routines *
***************/


/* temperatureFunction: this function is responsible for reading 
 *                      the analog value of temperature function, 
 *                      also is responsible for checking extreme
 *                      temperature values.
 */
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

  // check extreme values
  if( temperature < extremeLow){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WARNING");
    lcd.setCursor(0, 1);
    lcd.print("TEMP < ");
    lcd.print(extremeLow);
    digitalWrite(BLUE, HIGH);
    digitalWrite(RED, LOW);
  }
  else if( temperature > extremeHigh){
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
  }

 return temperature;
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

/* checkExtremeFan(): this function is responsible 
 *                    for checking whether fan is needed
 *                    to be turned on
 */
void checkExtremeFan(float averageTemperature){

  // check fan
  if( averageTemperature > extremeFan){
     digitalWrite(GREEN, HIGH);
  }
  else{
    digitalWrite(GREEN, LOW);
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
