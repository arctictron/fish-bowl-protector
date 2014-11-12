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
int i, reading[3], reading2,armed, theta, n, intscan[21][3], scanpinState, scanintpinState;
int armedlast = 1;
int state = pincheck;
long interval = 30000;

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
			scanintpinState = digitalRead(scanintpin);
			
			if(armed == HIGH && scanpinState == LOW && scanintpinState == LOW) {
				Serial.println("System Armed");
				
				// Check to see if initial scan array is set
				state = scan;
			} else if (scanpinState == HIGH) {
				state = initialscan; 
			} else if (scanintpinState == HIGH) {
				state = updateinterval;
			}
			
			break;
			
		case updateinterval:
			digitalWrite(scannedpin, HIGH);
			while (digitalRead(scanintpin) == HIGH) {
			}
			interval = pulseIn(scanintpin, HIGH);
			digitalWrite(scannedpin, LOW);
			
			interval = (interval - 100) / 100 * 1000;
			
			Serial.print("Interval: ");
			Serial.println(interval);
			state = pincheck;
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
			
			
			while(digitalRead(armedpin) == HIGH && digitalRead(scanpin) == LOW && digitalRead(scanintpin) == LOW) {
				Serial.println("Starting new scan interval.");
				
			
				theta = 10;
				
				for(n = 0; n < 21; n++) {
					Serial.print("Theta: ");
					Serial.println(theta);
				
					for(i = 0; i < 3; i++) {
						reading[i] = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),200);
					}
					
					Serial.print("    L: ");
					Serial.print(reading[0]);
					Serial.print("    C: ");
					Serial.print(reading[1]);
					Serial.print("    R: ");
					Serial.println(reading[2]);
					
					// Check if armed scan pin is high
					if(digitalRead(armedpin) == LOW || digitalRead(scanpin) == HIGH) {
						break;
					}
					
					// Increase theta
					theta = theta + 8;
				}
			
				// Wait for next scan interval
				if(digitalRead(armedpin) == HIGH && digitalRead(scanpin) == LOW && digitalRead(scanintpin) == LOW) {
					Serial.println("Begin delay interval.");
					delay(interval);
				}
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