#define trig 4
#define echo 5

void setup() {
	Serial.begin(9600);
	
	pinMode(trig, OUTPUT);
	pinMode(echo, INPUT);
}

void loop() {
	ultrasonicRoutine(trig,echo,200);
	//delay(2500);
}

int ultrasonicRoutine(int triggerpin, int echopin, int maxdistance) {

	int readstart, readend, duration, distance;
	long timeout = 68000;

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
	
	Serial.println(distance);
	Serial.println(readend);
	
	
	// Return the distance
	return distance;
}	