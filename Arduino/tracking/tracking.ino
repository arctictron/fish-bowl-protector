// Mega Pinout
#define trig1 			8
#define echo1 			9
#define trig2 			10
#define echo2 			11
#define trig3 			12
#define echo3			13

// Define constants
#define debug			1
#define maxrange		200

// Include Libaries
#include <Servo.h>


Servo turret;
boolean goingRight = true;
int currentPosition = 90;
int dTheta = 10;
int dTheta2 = 8;
boolean tracking = true;
int i = 0;

void setup () {
	// Serial Port begin
	Serial.begin (9600);
	
	// Set I/O
	pinMode(trig1, OUTPUT);
	pinMode(echo1, INPUT);
	pinMode(trig2, OUTPUT);
	pinMode(echo2, INPUT);
	pinMode(trig3, OUTPUT);
	pinMode(echo3, INPUT);

	// Set up Servo
	turret.attach(5);

}

void loop() { 
	
	while(i < 100) {
		track(); 
		i++;
	}
}

void track() {
	
	// Define Variables
	int difference, duration, distance, distancec, distance1, distance2, distance3;
	
	// Run ultrasonicRoutine for each ping sensor to get distance
	distance1 = ultrasonicRoutine(trig1,echo1,maxrange);
	distance2 = ultrasonicRoutine(trig2,echo2,maxrange);
	distance3 = ultrasonicRoutine(trig3,echo3,maxrange); 
	
	if(debug) {
		Serial.print("distance1 = ");
		Serial.print(distance1);
		Serial.print("\t");
		Serial.print("distance2 = ");
		Serial.print(distance2);
		Serial.print("\t");
		Serial.print("distance3 = ");
		Serial.println(distance3);
	}

	  difference = distance1 - distance2;

	  int difference23;
	  difference23 = distance2 - distance3;
	 
	  int difference31;
	  difference31 = distance1 - distance3;
	  
	// Calculate tolerance values
	float 	tolerance;
	int 	tol12, tol3;
	
	if(distance1 < distance2) {
		distance = distance1;
		distancec = distance2;
	} else {
		distance = distance2;
		distancec = distance1;
	}
	
	tolerance = (float(distance) / 10.0) + .5;
	tol12 = int(tolerance);
	
	tolerance = (float(distance3) / 20.0) + .5;
	tol3 = int(tolerance);
	
	/*float closetolerance;
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
				closetol3 = int (closetolerance3);*/
				
	if(debug) {
		Serial.print("tol12 = ");
		Serial.print(tol12);
		Serial.print("\t");
		Serial.print("tol3 = ");
		Serial.println(tol3);
	}
	
	
   
	/*if (distance1 + closetol1  < distance2 + closetol2 && distance1 - closetol1 < distance2 - closetol2 && currentPosition >=10)
			{
				delay (5);
				currentPosition = currentPosition - dTheta2 ;
				turret.write(currentPosition );
				Serial.println("Smallleft");
				
			}
	if (distance1 + closetol1 > distance2+closetol2  && distance1 - closetol1 > distance2-closetol2 && currentPosition<= 170)
		{
			delay (5);
			currentPosition = currentPosition + dTheta2;
			turret.write(currentPosition );
			Serial.println("smallRight");
		}
	if (distance1 + tol1  < distance2 + tol2 && distance1 - tol1 < distance2 - tol2 && currentPosition >=10)
		{ 
		
   
		 delay (5);
		 currentPosition = currentPosition - dTheta ;
		 turret.write(currentPosition );
		 Serial.print("Bigleft");

	   }
		
	if (distance1 + tol1 > distance2+tol2  && distance1 - tol1 > distance2-tol2 && currentPosition<= 170)
	   {
		 delay (5);
		 currentPosition = currentPosition + dTheta;
		 turret.write(currentPosition );
		 Serial.print("Bigright");

		 }
  */
	if(distance <= (distance3 + tol3) && distance >= (distance3 - tol3) && distancec <= (distance + tol12) && distancec >= (distance - tol12) && distance != maxdistance) {
		Serial.print("Fire!");
	}
	  Serial.println(currentPosition);

	if(debug) {
		delay(1000);
	}
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