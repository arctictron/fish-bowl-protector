// Defining pins
#define trig1 4
#define echo1 5
#define trig2 6
#define echo2 7
#define trig3 8
#define echo3 9

#include <SoftwareSerial.h>

// Attach the serial display's RX line to digital pin 2
SoftwareSerial lcd(3,2); // pin 2 = TX, pin 3 = RX (unused)

// Initialize variables
int i, reading;

String printstring;

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
	
	// Delay for LCD
	delay(1000); // wait for display to boot up
}

void loop() {
	printstring = "";
	for(i = 0; i < 3; i++) {
		reading = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),200);
		printstring = printstring + String(i+1) + ":" + String(reading) + " ";
	}
	clearLCD();
	lcd.print(printstring);
	Serial.println(printstring);

}