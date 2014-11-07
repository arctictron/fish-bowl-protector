// Defining pins
#define trig1 A0
#define echo1 A1
#define trig2 A2
#define echo2 A3
#define trig3 A4
#define echo3 A5
#define armpin 11
#define firedpin 12

// Defining states
#define armedcheck 0
#define initialscan 1
#define scan 2
#define track 3
#define fire 4

#include <SoftwareSerial.h>

// Attach the serial display's RX line to digital pin 2
SoftwareSerial lcd(3,2); // pin 2 = TX, pin 3 = RX (unused)

// Initialize variables
int i, reading[3], reading2,armed, theta, n;
int armedlast = 1;
int state = armedcheck;

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
	pinMode(armpin, INPUT);
	pinMode(firedpin, OUTPUT);
	
	// Delay for LCD
	delay(1000); // wait for display to boot up
}

void loop() {

	switch(state) {
	
		case armedcheck:
			armed = digitalRead(armpin);
			
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
			}
			
			break;
			
		case initialscan:
			theta = 10;
		
			for(n = 0; n < 20; n++) {
				for(i = 0; i < 3; i++) {
					reading[i] = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),200);
				}
				
				//
				
				// Print Values and theta
				
				
				// Increase position
				theta = theta + 8;
			}
			
			break;
			
		case scan:
			while(digitalRead(armpin) == HIGH) {
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
				
				if((reading[2] + 3) >= reading[1] && (reading[2] - 2) <= reading[1] &&  (reading[2] + 3) >= reading[3] && (reading[2] - 2) <= reading[3]) {
					state = fire;
					break;
				}
			}
			
			// If while statement is not true set state back to armedcheck
			//state = armedcheck;
		
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