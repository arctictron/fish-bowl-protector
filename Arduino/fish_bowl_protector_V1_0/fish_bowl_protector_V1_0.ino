// File: fish_bowl_protector_V1_0.ino
// Description: Main program flow for the arduino.

// Mega Pinout
#define servo
#define	speaker			
#define trig1 			8
#define echo1 			9
#define trig2 			10
#define echo2 			11
#define trig3 			12
#define echo3			13
#define firedpin 		22
#define armedpin 		23
#define scanpin 		24
#define scannedpin	 	25
#define scanintpin 		26

// Define Case States
#define pincheck		0
#define updateinterval	1
#define initialscan 	2
#define scan 			3
#define track 			4
#define fire 			5

// Define other constants
#define debug			1
#define dtheta 			8
#define maxrange		200

// Include libraries
#include <Servo.h>

// Initialize variables
int i, reading[3], reading2,armed, theta, n, baseScan[21][3], scanpinState, scanintpinState, time, checkreading;
int lastarmed = 0;
int state = pincheck;
long interval = 30000;
Servo turret;

void setup() {
	//Serial Port begin
	Serial.begin (9600);
	
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
	
	// Set servo
	turret.attach(5);
	turret.write(0);
	delay(5);
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
				if(baseScan[0][0] == 0 && baseScan[0][1] == 0 && baseScan[0][2] == 0) {
					state = initialscan;
				} else {
					state = scan;
				}
			} else if (armed == LOW && lastarmed != 1) {
				Serial.println("System not armed.");
				lastarmed = 1;
			} else if (scanpinState == HIGH) {
				state = initialscan; 
			} else if (scanintpinState == HIGH) {
				state = updateinterval;
			}
			
			break;
			
		case updateinterval:
			digitalWrite(scannedpin, HIGH);
			
			// Wait for scanintpin to go low
			while (digitalRead(scanintpin) == HIGH) {
			}
			
			interval = pulseIn(scanintpin, HIGH);
			digitalWrite(scannedpin, LOW);
			
			// Calculate interval
			interval = (interval - 100) / 100 * 1000;
			
			if(debug) {
				Serial.print("Interval: ");
				Serial.println(interval);
			}
			
			// Return to pincheck
			state = pincheck;
			
			break;
			
		case initialscan:
			Serial.println("Starting base scan.");
			
			theta = 10;
			
			turret.write(theta);
			
			delay(5);
		
			for(n = 0; n < 21; n++) {
				// Run ultrasonicRoutine for each ping sensor
				for(i = 0; i < 3; i++) {
					reading[i] = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),200);
				}
				
				// Validate the center ultrasonic data
				validateReadings();
				
				// Update baseScan array
				for(i = 0; i < 3; i++) {
					baseScan[n][i] = reading[i];
				}
				
				// Print Values and theta
				Serial.print("Theta: ");
				Serial.print(theta);
				Serial.print("    L: ");
				Serial.print(reading[0]);
				Serial.print("    C: ");
				Serial.print(reading[2]);
				Serial.print("    R: ");
				Serial.println(reading[1]);
				
				// Increase position
				if(theta <= 170) {
					theta = theta + dtheta;
					turret.write(theta);
					delay(5);
				}
				
			}
			
			turret.write(10);
			
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
				
				turret.write(theta);
				
				delay(100);
				
				for(n = 0; n < 21; n++) {
					Serial.print("Theta: ");
					Serial.println(theta);
				
					for(i = 0; i < 3; i++) {
						reading[i] = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),200);
					}
					
					if(debug) {
						Serial.print("Theta: ");
						Serial.println(theta);
						Serial.print("\t");
						Serial.print("L: ");
						Serial.print(reading[0]);
						Serial.print("\t");
						Serial.print("C: ");
						Serial.print(reading[2]);
						Serial.print("\t");
						Serial.print("R: ");
						Serial.println(reading[1]);
					}
					
					// Check if armed scan pin is high
					if(digitalRead(armedpin) == LOW || digitalRead(scanpin) == HIGH) {
						break;
					} else {
						// Check to see if something moved
						for(i = 0; i < 3; i++) {
							if(reading[i] <= (baseScan[n][i] - 2)) {
								checkreading = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),maxrange);
								
								if(checkreading <= (baseScan[n][i] - 2)) {
									state = track;
									break;
								}
							}
						}
						
						if(theta <= 170 && state != track) {
							theta = theta + dtheta;
							turret.write(theta);
							delay(10);
						}
					}
				}
				
				if(state != track) {
					turret.write(10);
				}
			
				// Wait for next scan interval
				if(digitalRead(armedpin) == HIGH && digitalRead(scanpin) == LOW && digitalRead(scanintpin) == LOW && state != track) {
					if(debug) {
						Serial.println("Begin delay interval.");
					}
					
					while(digitalRead(armedpin) == HIGH && digitalRead(scanpin) == LOW && digitalRead(scanintpin) == LOW && time <= interval) {
						delay(5);
						time = time + 5;
					}
				}
			}
			
			// Update state if while statement is broken and state does not equal track.
			if (state != track) {
				if (digitalRead(armedpin) == LOW && (digitalRead(scanpin) == LOW || digitalRead(scanintpin) == LOW)) {
					state = pincheck;
					lastarmed = 0;
				} else if(digitalRead(scanpin) == HIGH) {
					state = initialscan;
				} else if (digitalRead(scanintpin) == HIGH) {
					state = updateinterval;
				} else {
					state = pincheck;
					Serial.println("Error in case scan.");
				}
			}
		
			break;
			
		case track:
			
			if(digitalRead(armedpin) == HIGH) {
				if(debug) {
					Serial.print("Begin tracking.");
				}
				
				
			}
			break;
			
		case fire:
			// Update fire count
			digitalWrite(firedpin, HIGH);
			delay(1);
			digitalWrite(firedpin, LOW);
			
			// Shoot stream of water
			
			
			// Return back to scanning
			state = scan;
			delay(5000);
			
			break;
		
	}
	
}