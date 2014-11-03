//	File: functions.ino
//	Description: This file contains a list of functions that are required to run the main program. Name and description of each function can be found below.

// UltrasonicRoutine
// Sends a ping out to determine the distance between an object and the ping sensor. Function then returns the distance.

int readstart, readend, duration, distance;

int ultrasonicRoutine(int triggerpin, int echopin, int maxdistance) {
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

// clearLCD
// Clears the LCD

void clearLCD() {
	lcd.write(254);
	lcd.write(128);
	lcd.write("                ");
	lcd.write("                ");
}