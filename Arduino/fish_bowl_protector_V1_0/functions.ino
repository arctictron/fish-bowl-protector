// File: functions.ino
// Description: This file contains a list of functions that are required to run the main program. Name and description of each function can be found below.

// UltrasonicRoutine
// Sends a ping out to determine the distance between an object and the ping sensor. Function then returns the distance.

int ultrasonicRoutine(int triggerpin, int echopin, int maxdistance) {

	int readstart, readend, duration, distance[3], average, a, samples, distanceCalc;
	long timeout = 6800;
	float xbar, xiArray[3], ssum, s;
	

	for(a = 0; a < 3; a++) {
		// Start Reading
		readstart = millis();
		
		// Begin ping
		digitalWrite(triggerpin, LOW);
		delay(1);
		digitalWrite(triggerpin, HIGH);
		delay(1);
		digitalWrite(triggerpin, LOW);
		
		duration = pulseIn(echopin, HIGH, timeout);
		
		distance[a] = (duration/2) / 29.1;

		// Storing distance value
		if(distance[a] == 0 || distance[a] > maxdistance) {
			distance[a] = maxdistance;
		} else if(distance[a] < 2 && distance[a] != 0) {
			distance[a] = ultrasonicRoutine(triggerpin,echopin,maxdistance);
		} else if(distance[a] >= 2 && distance[a] <= maxdistance) {
			distance[a] = distance[a];
		}

		// Checking the time since beginning of last reading (only to reduce time between pings, thus no need to add delay after function)
		readend = millis() - readstart;
		if(readend < 68) {
			delay(68 - readend);
		}
	}
	
	// Find standard deviation
	
	if(0) {
		Serial.println("Distances:");
		Serial.print("\t");
		Serial.print("1: ");
		Serial.print(distance[0]);
		Serial.print("\t");
		Serial.print("2: ");
		Serial.print(distance[1]);
		Serial.print("\t");
		Serial.print("3: ");
		Serial.println(distance[2]);
	}
	
	xbar = float(distance[0] + distance[1] + distance[2]) / 3.0;
	
	for(a = 0; a < 3; a++) {
		xiArray[a] = (float(distance[a]) - xbar);
	}
	
	ssum = 0;
	
	for (a = 0; a < 3; a++) {
		ssum += (xiArray[i] * xiArray[i]) / 2;
	}
	
	s = sqrt(ssum);
	
	samples = 0;
	distanceCalc = 0;
	
	// Find outliers and calculate average
	for(a = 0; a < 3; a++) {
		if(s != 0) {
			if(abs(xiArray[a]) < s) {
				distanceCalc += distance[a];
				samples++;
			}
		} else {
			distanceCalc += distance[a];
			samples++;
		}
	}
	
	distanceCalc = distanceCalc / samples;
	
	// Return the distance
	return distanceCalc;
}

// UltrasonicRoutineTrack
// Sends a ping out to determine the distance between an object and the ping sensor. Function then returns the distance.

int ultrasonicRoutineTrack(int triggerpin, int echopin, int maxdistance) {

	int readstart, readend, duration, distance;
	long timeout = 6800;
	
	// Start Reading
	readstart = millis();
	
	// Begin ping
	digitalWrite(triggerpin, LOW);
	delay(1);
	digitalWrite(triggerpin, HIGH);
	delay(1);
	digitalWrite(triggerpin, LOW);
	
	duration = pulseIn(echopin, HIGH, timeout);
	
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
	tolfloat = float(reading[0]) / 10.0 + .5;
	tol = int(tolfloat);
	
	
	// Check to see if center data is valid
	if(reading[1] <= (reading[0] + tol) && reading[1] >= (reading[0] - tol) && reading[2] > (reading[0] + tol)) {
		for(i = 0; i < 3; i++) {
			reading[i] = ultrasonicRoutine((trig1 + 2*i),(echo1 + 2*i),200);
		}
		
		validateReadings();
	}
}