// File: fish_bowl_protector_V1_0.ino
// Description: Main program flow for the arduino.

// Define pins
#define trig1 			4
#define echo1 			5
#define trig2 			6
#define echo2 			7
#define trig3 			8
#define echo3			9
#define servo			10
#define firedpin 		A1
#define armedpin 		A2
#define scanpin 		A3
#define scannedpin	 	A4
#define scanintpin 		A5

// Define states
#define pincheck		0
#define updateinterval	1
#define initialscan 	2
#define scan 			3
#define track 			4
#define fire 			5

// Include libraries
#include <SoftwareSerial.h>

// Attach the serial display's RX line to digital pin 2
SoftwareSerial lcd(3,2); // pin 2 = TX, pin 3 = RX (unused)

// Initialize variables
int i, reading[3], reading2,armed, theta, n, intscan[21][3], scanpinState;
int armedlast = 1;
int state = pincheck;

void setup() {
	//Serial Port begin
	Serial.begin (9600);
	lcd.begin(9600);
	
	//Set I/O
	pinMode(trig1, OUTPUT);
	pinMode(echo1, INPUT);
	pinMode(trig2, OUTPUT);
	pinMode(echo2, INPUT);
	pinMode(trig3, OUTPUT);
	pinMode(echo3, INPUT);
	pinMode(armedpin, INPUT);
	pinMode(firedpin, OUTPUT);
	pinMode(scanpin, INPUT);
	pinMode(scannedpin, OUTPUT);
	pinMode(scanintpin, INPUT);
}

void loop() {

	switch(state) {
	
		case pincheck:
			armed = digitalRead(armedpin);
			scanpinState = digitalRead(scanpin);
			//scanintpinState = digitalRead(scanintpin);
			
			if(armedlast != armed && armed == LOW) {
				clearLCD();
				lcd.print("     System     ");
				lcd.print("   Not  Armed   ");
				Serial.println("System Not Armed");
				armedlast = armed;
			} else if(armed == HIGH) {
				clearLCD();
				lcd.print("     System     ");
				lcd.print("     Armed      ");
				Serial.println("System Armed");
				
				// Check to see if initial scan array is set
				state = scan;
				armedlast = 1;
				delay(1000);
				clearLCD();
			} else if (scanpinState == HIGH) {
				state = initialscan; 
			} //else if (scanintpinState == HIGH) {
			//}
			
			break;
			
		case updateinterval:
			break;
			
		case initialscan:
			theta = 10;
		
			for(n = 0; n < 21; n++) {
				// Run ultrasonicRoutine for each ping sensor
				for(i = 0; i < 3; i++) {
					reading[i] = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),200);
				}
				
				// Validate the center ultrasonic data
				validateReadings();
				
				// Update intscan array
				for(i = 0; i < 3; i++) {
					intscan[n][i] = reading[i];
				}
				
				// Print Values and theta
				Serial.print("Theta: ");
				Serial.print(theta);
				Serial.print("    L: ");
				Serial.print(reading[0]);
				Serial.print("    C: ");
				Serial.print(reading[1]);
				Serial.print("    R: ");
				Serial.println(reading[2]);
				
				// Increase position
				theta = theta + 8;
			}
			
			// Tell pic microcontrollor scan is complete
			Serial.println("Scan Complete");
			digitalWrite(scannedpin, HIGH);
			while (digitalRead(scanpin) == HIGH) {
				delay(50);
			}
			digitalWrite(scannedpin, LOW);
			
			// Update state
			state = pincheck;
			
			break;
			
		case scan:
			theta = 10;
		
			while(digitalRead(armedpin) == HIGH && (digitalRead(scanpin) == HIGH || digitalRead(scanintpin) == HIGH)) {
				lcd.write(254);
				lcd.write(128);
				lcd.print("     ");
				lcd.write(254);
				lcd.write(128);
				reading[1] = ultrasonicRoutine(trig1,echo1,200);
				lcd.print(reading[1]);
				Serial.print(reading[1]);
				Serial.print("    ");
				lcd.write(254);
				lcd.write(133);
				lcd.print("     ");
				lcd.write(254);
				lcd.write(133);
				reading[2] = ultrasonicRoutine(trig2,echo2,200);
				lcd.print(reading[2]);
				Serial.print(reading[2]);
				Serial.print("    ");
				lcd.write(254);
				lcd.write(138);
				lcd.print("     ");
				lcd.write(254);
				lcd.write(138);
				reading[3] = ultrasonicRoutine(trig3,echo3,200);
				lcd.print(reading[3]);
				Serial.println(reading[3]);
				
			}
			
			// Update state if while statement is broken and state does not equal track.
			if (state != track) {
				if (digitalRead(armedpin) == HIGH && (digitalRead(scanpin) == LOW || digitalRead(scanintpin) == LOW)) {
					state = pincheck;
				} else if(digitalRead(scanpin) == HIGH) {
					state = initialscan;
				} else if (digitalRead(scanintpin) == HIGH) {
					state = updateinterval;
				} else {
					state = pincheck;
				}
			}
		
			break;
			
		case track:
			break;
			
		case fire:
			clearLCD();
			lcd.print("Fired!");
			digitalWrite(firedpin, HIGH);
			delay(100);
			digitalWrite(firedpin, LOW);
			delay(2000);
			state = scan;
			break;
			
	}
	
}