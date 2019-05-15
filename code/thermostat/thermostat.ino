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
 * - one Arduino UNO
 * - one TMP36 temperature sensor
 * - one LCD 16x2 JHD659 M10 1.1
 * - one HC-SR04 proximity sensor
*/

// include the library code:
#include <LiquidCrystal.h>

// functions declaration
void checkExtremeValues( float temperature, int i);
void printLCD( String line1, String line2);
void checkExtremeFan(float averageTemperature);
int checkProximity();
float readTemperature();

// initialize the LCD function with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* Global Variables Declaration */
const int sensorPin = A0, BLUE = 8, RED = 9;
const int GREEN = 13, trigPin = 7, echoPin = 6;
const float extremeLow = 21.5, extremeHigh = 22.5, extremeFan = 23;
float temperature[24], averageTemperature, sum = 0;
int i, isSomeoneClose = 0, counter = 0;
unsigned long duration, cm, inches;
bool FAN = false;



/* setup(): This function is executed only once at the POWER ON 
 *          or at RESET
 */
void setup() {

  cli();
  /* Define PINs Mode */
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(GREEN, LOW);
  
  /* Setup LCD */
  lcd.begin(16, 2);  
  lcd.print("ARDUINO");  
  lcd.setCursor(0, 1);  
  lcd.print("PROJECT");

  /* Setup HC-SR04 */
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  /* Enable TIMER2 OVERFLOW INTERRUPT */
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  TIMSK2 = (TIMSK2 & B11111110) | 0x01;       // Arduino UNO has an ATmega328P CPU
  TCCR2B = (TCCR2B & B11111000) | 0x07;       // Thus, ENABLE bit TIMSK:0 and SET prescaler to clk/1024
  Serial.println(String(TIMSK2, BIN));
  Serial.println(String(TCCR2B, BIN));
   
  /* Setup Serial Communication */
  Serial.begin(9600);

  /* Enable Interrupts Globally */
  sei();
}

/* TIMER2 OVERFLOW INTERRUPT VECTOR: this function checks periodically every 0.5 seconds
 *                                   if someone is nearby. Then it prints average Temperature 
 *                                   and the latest temperature measurment.
 */
ISR(TIMER2_OVF_vect){
  counter++;
  //Serial.println("TIMER OVERFLOW: " + counter);
  if( counter == 30){
    String line1, line2;
    //Serial.print("INTERRUPT: ");
    // check if someone is close
    isSomeoneClose = checkProximity();
    counter = 0;
    if( isSomeoneClose == 1){
        line1 = String("AVG:" + String( averageTemperature, 3));
        if(i == 0)
          line2 = String("Last TEMP: " + String(temperature[24], 3));
        else
          line2 = String("Last TEMP: " + String(temperature[i-1], 3));

        printLCD( line1, line2);        
    }
  }  
}

/* loop(): this function is the main function. It executes
 *         continuously.
 */
void loop() {
  
  sum = 0;                    // this variable holds the sum of temperatures
  
  Serial.println("New Measurement");
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

/***************
* Sub-routines *
***************/


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
      digitalWrite(BLUE, HIGH);      
   }
   else{
    digitalWrite(BLUE, LOW);  
   }
   
   if( temperature > extremeHigh){
      if( i > 0){
        line1 = String("Warning: HIGH");
        line2 = String( "Temp      " + String(temperature, 3));
        printLCD( line1, line2);
        printMade = true;
      }
      digitalWrite(RED, HIGH);         
    }
    else{
      digitalWrite(RED, LOW);
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
      Serial.println("FAN is OFF: TURN it ON");
      digitalWrite(GREEN, HIGH);
      FAN = true;
    }
    else{
      Serial.println("2.FAN is " + String(FAN) + ": KEEP it ON");   
    }
  }
  else{
    digitalWrite(GREEN, LOW);
    FAN = false;
    Serial.println("3.FAN is " + String(FAN));
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
  float voltage = (sensorVal / 1024.0) * 5;         // 3.1 is used after calibration. Nomrally 3.3 should be used
  float temperature = (voltage - .5) * 100;              // since 3.3 V is the Operation Vlotage of MCU.
  String printLine = String(String(i) + ". sensor Value: " + String(sensorVal) + ", Volts: " + String(voltage, 3) + ", degrees C: " + String(temperature, 3));
  Serial.println(printLine);
  return temperature;
}
