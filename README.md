# Microprocessors-and-Peripherals-Arduino-Project

Authors: 

    1. Dimitrios Antoniadis
    
    2. Vasileios Dimitriadis
    
************************************

SUBJECT:	MICROCONTROLLERS AND PERIPHERALS	

 UNIVERSITY:	ARISTOTLE UNIVERISTY OF THESSALONIKI	

 SEMESTER:	8TH					

 DATE:		26/5/2019				
****************************

*Project: Arduino Thermostat*

*DEV Board:*

    1. Arduino UNO
    2. Wemos D1 R2

					
*****************************************
*****************************************

ARDUINO
-----------------------

File: thermostat/thermostat.ino

*****************************************
*****************************************

1.--- SETUP ---

------------------------------------------

 This function is executed only once at the POWER ON 
 or at RESET
 
---------------------------------
	
	1. Define PINs Mode
	2. Setup LCD
	3. Setup HC-SR04
	4. Enable TIMER2 OVERFLOW INTERRUPT
	5. Setup Serial Communication
	6. Enable Interrupts Globally
	
2.--- LOOP ---

-------------------------

 this function is the main function. It executes
 continuously. This function is interrupted every 0.5 secs
 by TIMER2_OVF_vect.
 
-------------------------------

	1. SUM = 0
	2. FOR i = 0 TO i = 23
		temperature[i] = READ TEMPERATURE
		SUM = SUM + temperature[i]
		CHECK IF temperature[i] IS AN EXTREME VALUE (extremeHigh/extremeLow)
	3. CALCULATE averageTemperature
	4. CHECK IF averageTemperature IS GREATER THAN (extremeFan)
		THEN IF FAN is TURNED OFF, TURN ON THE FAN
		ELSE TURN IT OFF
		
3.--- TIMER2_OVF_vect ---

-------------------------------

 this function checks periodically every 0.5 seconds
 if someone is nearby. Then it prints average Temperature 
 and the latest temperature measurment.
 
----------------------------------------------
	
	1. INCREASE COUNTER
	2. IF COUNTER EQUALS 30, 0.5 SECS HAVE ELAPSED
		CHECK IF SOMEBODY IS CLOSE
		IF YES THEN PRINT LAST AVG TEMP (averageTemperature) AND LAST MEASURED TEMPERATURE (temperature[i-1])
		COUNTER = 0
		
		
4.--- printLCD ---

------------------------
	
 this function is responsible for
 printing average temperature to
 LCD screen.
 
--------------------------------------


5.--- checkExtremeValues ---

------------------------------------------

 this function is responsible for cheking wether
 temperature is Greater than a high value or
 Lower than a low value. Accordingly, this function
 turns on/off BLUE/RED LEDS and prints respectively 
 on LCD the suitable message. LCD is used only after
 i > 0 and that's because only then 10 secs have elapsed
 since previous average value print.
 
---------------------------------------------------------

6.--- checkExtremeFan ---

------------------------------------------------

 this function is responsible 
 for checking whether fan is needed
 to be turned on.
 
--------------------------------------------------

7.--- checkProximity ---

-----------------------------------

 this function is responsible for acknowledging wheter 
 someone is close or not.

-------------------------------------

8.--- readTemperature ---

--------------------------------------

 this function is responsible for reading 
 temperature sensor and returning temperature value.

-------------------------------------------



				
*****************************************
*****************************************

WEMOS D1 R2
-----------------------

File: thermostat-Wifi/thermostat-Wifi.ino
      		     /Gsender.H:   contains INFORMATION about sending e-mail 
		     /Gsender.cpp: implements e-mail sending

*****************************************
*****************************************

1.--- SETUP ---

------------------------------------------

 This function is executed only once at the POWER ON 
 or at RESET
 
---------------------------------
	
	1. Define PINs Mode
	2. Setup LCD
	3. Setup HC-SR04
	4. Enable TIMER2 OVERFLOW INTERRUPT
	5. Setup Serial Communication
	6. Enable Interrupts Globally
	
2.--- LOOP ---

-------------------------

 this function is the main function. It executes
 continuously. This function is interrupted every 0.5 secs
 by TIMER2_OVF_vect.
 
-------------------------------

	1. SUM = 0
	2. FOR i = 0 TO i = 23
		temperature[i] = READ TEMPERATURE
		SUM = SUM + temperature[i]
		CHECK IF temperature[i] IS AN EXTREME VALUE (extremeHigh/extremeLow)
	3. CALCULATE averageTemperature
	4. CHECK IF averageTemperature IS GREATER THAN (extremeFan)
		THEN IF FAN is TURNED OFF, TURN ON THE FAN
		ELSE TURN IT OFF
		
3.--- interruptVector ---

-------------------------------

 this function checks periodically every 0.5 seconds
 if someone is nearby. Then it prints average Temperature 
 and the latest temperature measurment.
 
----------------------------------------------
	
	1. INCREASE COUNTER
	2. IF COUNTER EQU
CHECK IF SOMEBODY IS CLOSE
		IF YES THEN PRINT LAST AVG TEMP (averageTemperature) AND LAST MEASURED TEMPERATURE (temperature[i-1])
		COUNTER = 0
		
		
4.--- printLCD ---

------------------------
	
 this function is responsible for
 printing average temperature to
 LCD screen.
 
--------------------------------------


5.--- checkExtremeValues ---

------------------------------------------

 this function is responsible for cheking wether
 temperature is Greater than a high value or
 Lower than a low value. Accordingly, this function
 turns on/off BLUE/RED LEDS and prints respectively 
 on LCD the suitable message. LCD is used only after
 i > 0 and that's because only then 10 secs have elapsed
 since previous average value print.
 
---------------------------------------------------------

6.--- checkExtremeFan ---

------------------------------------------------

 this function is responsible 
 for checking whether fan is needed
 to be turned on.
 
--------------------------------------------------

7.--- checkProximity ---

-----------------------------------

 this function is responsible for acknowledging wheter 
 someone is close or not.

-------------------------------------

8.--- readTemperature ---

--------------------------------------

 this function is responsible for reading 
 temperature sensor and returning temperature value.

-------------------------------------------

9.--- WiFiConnect ----

--------------------------------

 this function is responsible for connecting 
 MCU to WiFi.
 
----------------------------------

10.--- Awaits ---

------------------------
 this function tries continuously to connect to WiFi.
 It stops when connection has been established.

-----------------------------------------------


		

 
