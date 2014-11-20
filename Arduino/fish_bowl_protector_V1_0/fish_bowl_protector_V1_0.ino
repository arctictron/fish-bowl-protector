// File: fish_bowl_protector_V1_0.ino
// Description: Main program flow for the arduino.

// Mega Pinout
#define servo			5
#define	speaker			3
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
#define pump			50

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
int i, reading[3], reading2,armed, theta, n, baseScan[21][3], scanpinState, scanintpinState, time, checkreading, gotoTracking[3];
int lastarmed = 0;
int state = pincheck;
long interval = 30000;
int rightLed = 53 ; // Right LED
int leftLed = 52; // NOt used yet 
Servo turret;
boolean goingRight = true;
int dTheta = 10;
int currentPosition = 90;
boolean tracking = true;

boolean locked = true;

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
	pinMode(pump, OUTPUT);
	
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
				if(debug) {
					Serial.println("System Armed");
				}
				
				// Check to see if initial scan array is set
				if(baseScan[0][0] == 0 && baseScan[0][1] == 0 && baseScan[0][2] == 0) {
					state = initialscan;
				} else {
					state = scan;
				}
			} else if (armed == LOW && lastarmed != 1) {
				if(debug) {
					Serial.println("System not armed.");
				}
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
			
			delay(200);
			
			// Return to pincheck
			state = pincheck;
			
			break;
			
		case initialscan:
			if(debug) {
				Serial.println("Starting base scan.");
			}
			
			theta = 10;
			
			turret.write(theta);
			
			delay(5);
		
			for(n = 0; n < 21; n++) {
				// Run ultrasonicRoutine for each ping sensor and set baseScan array
				for(i = 0; i < 3; i++) {
					baseScan[n][i] = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),200);
				}
				
				// Print Values and theta
				if(debug) {
					Serial.print("Theta ");
					Serial.print(theta);
					Serial.println(":");
					Serial.print("\t");
					Serial.print("L: ");
					Serial.print(baseScan[n][0]);
					Serial.print("\t");
					Serial.print("C: ");
					Serial.print(baseScan[n][2]);
					Serial.print("\t");
					Serial.print("R: ");
					Serial.println(baseScan[n][1]);
				}
				
				// Increase position
				if(theta <= 170) {
					theta = theta + dtheta;
					turret.write(theta);
					delay(5);
				}
				
			}
			
			turret.write(10);
			
			// Tell pic microcontrollor scan is complete
			if(debug) {
				Serial.println("Scan Complete");
			}
			
			digitalWrite(scannedpin, HIGH);
			
			while (digitalRead(scanpin) == HIGH) {
				delay(5);
			}
			
			digitalWrite(scannedpin, LOW);
			
			delay(1000);
			
			// Update state
			state = pincheck;
			
			break;
			
		case scan:
			
			while(digitalRead(armedpin) == HIGH && digitalRead(scanpin) == LOW && digitalRead(scanintpin) == LOW && state != track) {
				if(debug) {
					Serial.println("Starting new scan interval.");
				}
				
				theta = 10;
				
				turret.write(theta);
				
				delay(5);
				
				for(n = 0; n < 21; n++) {
				
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
						Serial.print("\t");
						Serial.print("L: ");
						Serial.print(baseScan[n][0]);
						Serial.print("\t");
						Serial.print("C: ");
						Serial.print(baseScan[n][2]);
						Serial.print("\t");
						Serial.print("R: ");
						Serial.println(baseScan[n][1]);
					}
					
					// Check if armed scan pin is high
					if(digitalRead(armedpin) == LOW || digitalRead(scanpin) == HIGH) {
						break;
					} else {
						// Check to see if something moved
						for(i = 0; i < 3; i++) {
							if(reading[i] <= (baseScan[n][i] - 10)) {
								Serial.println(reading[i]);
								Serial.println(baseScan[n][i] - 5);
								checkreading = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),maxrange);
								
								if(checkreading <= (baseScan[n][i] - 2)) {
									gotoTracking[i] = 1;
								} else {
									gotoTracking[i] = 0;
								}
							} else {
								Serial.println(reading[1]);
								Serial.println(baseScan[n][i] - 5);
								gotoTracking[i] = 0;
							}
						}
						
						if(gotoTracking[0] == 1 || gotoTracking [1] == 1|| gotoTracking[2] == 1) {
							state = track;
							
							if(debug) {
								Serial.println("Going to track.");
							}
							break;
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
					
					time = 0;
					
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
				
				while(state != fire) {
					int difference, distance1, distance2, distance3, difference2;
					distance1 = ultrasonicRoutineTrack(trig1,echo1,200);
					distance2 = ultrasonicRoutineTrack(trig2,echo2,200);  
					distance3 = ultrasonicRoutineTrack(trig3,echo3,200);  
					  Serial.print("ping 1");    
					  
					  Serial.print("\t");  
					  Serial.print(distance1);
				 
					  Serial.print("\t");     
					  Serial.print("ping 2");    
					 
					  Serial.print("\t");  
					  Serial.print(distance2);
					  difference = distance1 - distance2;
					  
					  Serial.print("\t");     
					  Serial.print("ping 3");    
					  Serial.print("\t");  
					  Serial.print(distance3);
					  int difference23;
					  difference23 = distance2 - distance3;

					  int difference31;
					  difference31 = distance1 - distance3;

					 float tolerance;
						tolerance = (float(distance1) / 10.0) + .5;
						int tol1;
						tol1 = int(tolerance);
						//Serial.print(tol1);
					 float tolerance2;
						tolerance2 = (float(distance2) / 10.0) + .5;
							int tol2;
								tol2 = int (tolerance2);
					  float tolerance3;
						tolerance3 = (float(distance3) / 20.0) + .5;
							int tol3;
								tol3 = int (tolerance3);
					float closetolerance;
						closetolerance = (float(distance1) / 7.0) + .5;
						int closetol1;
						closetol1 = int(closetolerance);
						//Serial.print(tol1);
					 float closetolerance2;
						closetolerance2 = (float(distance2) / 7.0) + .5;
							int closetol2;
								closetol2 = int (closetolerance2);
					  float closetolerance3;
						closetolerance3 = (float(distance3) / 15.0) + .5;
							int closetol3;
								closetol3 = int (closetolerance3);
				   
					if(distance1 < distance2 && distance1 < distance2 && theta > 10) { 
						theta = theta - dTheta ;
						turret.write(theta);
						delay(10);
					} else if(distance1 > distance2 && distance1 > distance2 && theta < 170) {
						theta = theta + dTheta;
						turret.write(theta);
						delay(10);
					}
				  
					if(distance3 < distance1 && distance3 < distance2 && distance3 < distance1) {
								distance3 = ultrasonicRoutine(trig3,echo3,200);
									if (distance3 < distance1 && distance3 < distance1 && distance3 < distance2 && distance3 < distance1) {
									  
							// Change state to fire
							state = fire;
							delay(500); 
						}
					}
					  Serial.print("\t");
					  Serial.print("theta");
					  Serial.print("\t");
					  Serial.println(theta); 
				}
			}
			break;
			
		case fire:
			// Update fire count
			digitalWrite(firedpin, HIGH);
			delay(1);
			digitalWrite(firedpin, LOW);
			
			// Shoot stream of water
			digitalWrite(pump, HIGH);
			delay(200);
			digitalWrite(pump, LOW);
			
			// Return back to scanning
			time = 0;
			
			while(time <= 5000) {
				delay(5);
				time = time + 5;
			}
			state = scan;
			
			break;
		
	}
	
}