// Defining pins
#define trig1 6
#define echo1 7
#define trig2 8
#define echo2 9
#define trig3 10
#define echo3 11

int reading[3];

void setup() {
	//Serial Port begin
	Serial.begin (9600);
	//lcd.begin(9600);
	
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

	for(int n = 0; n < 3; n++) {
		reading[n] = ultrasonicRoutine((trig1 + 2*n),(echo1 + 2*n),200);
	}
	

	Serial.print(reading[1]);
	Serial.print("    ");
	Serial.print(reading[2]);
	Serial.print("    ");
	Serial.println(reading[3]);
}		

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