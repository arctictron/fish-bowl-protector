// File: functions.ino
// Description: This file contains a list of functions that are required to run the main program. Name and description of each function can be found below.

// UltrasonicRoutine
// Sends a ping out to determine the distance between an object and the ping sensor. Function then returns the distance.

int ultrasonicRoutine(int triggerpin, int echopin, int maxdistance) {

	int readstart, readend, duration, distance;

	// Start Reading
	readstart = millis();
	
	// Begin ping
	digitalWrite(triggerpin, LOW);
	delay(1);
	digitalWrite(triggerpin, HIGH);
	delay(1);
	digitalWrite(triggerpin, LOW);
	
	duration = pulseIn(echopin, HIGH);
	
	distance = (duration/2) / 29.1;

	// Storing distance value
	if(distance == 0 || distance > maxdistance) {
		distance = maxdistance;
	} else if(distance < 2 && distance != 0) {
		distance = ultrasonicRoutine(triggerpin,echopin,maxdistance);
	} else if(distance >= 2 && distance <= maxdistance) {
		distance = distance;
	}

	// Checking the time since beginning of last reading (only to reduce time between pings, thus no need to add delay after function)
	readend = millis() - readstart;
	if(readend < 68) {
		delay(68 - readend);
	}
	
	// Return the distance
	return distance;
}

// ValidateReadings
// 

void validateReadings() {
	// Declare variables
	float  	tolfloat;
	int		tol;
	
	// Set tolerance
	tolfloat = reading[0] / 10 + .5;
	tol = int(tolfloat);
	
	
	// Check to see if center data is valid
	if(reading[2] <= (reading[0] + tol) && reading[2] >= (reading[0] - tol) && reading[1] > (reading[0] + tol)) {
		for(i = 0; i < 3; i++) {
			reading[i] = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),200);
		}
		
		validateReadings();
	}
}

// clearLCD
// Clears the LCD

void clearLCD() {
	lcd.write(254);
	lcd.write(128);
	lcd.write("                ");
	lcd.write("                ");
}