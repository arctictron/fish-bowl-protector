
int trig1 = 8; //trig1 for ping 1
int echo1 = 9; // echo1 pin for ping 1
int trig2 = 10; // trig1 pin for ping 2
int echo2 = 11; // echo1 pin for ping 2
int trig3 = 12;
int echo3 = 13;
int rightLed = 53 ; // Right LED
int leftLed = 52; // NOt used yet 
 #define	speaker 	3 
#include <Servo.h>
Servo turret;
boolean goingRight = true;
int currentPosition = 90;
int dTheta = 10;
int dTheta2 = 8;
boolean tracking = true;
int PIC = 38; 

int maxrange = 200;
boolean armed = true;
boolean locked = true;
void setup ()
{
  pinMode(trig2 , OUTPUT);  
  pinMode(echo2 , INPUT);
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(rightLed, OUTPUT);
  pinMode(leftLed, OUTPUT);
  pinMode(trig3,OUTPUT);
  pinMode(echo3,INPUT);
  Serial.begin(9600);
  pinMode(PIC,INPUT);
  pinMode(speaker, OUTPUT);
  turret.attach(5);
  pinMode(speaker,OUTPUT);
  //Attaches the turret to the correct pin
  //turret.write(currentPosition);
}

void loop() 
{ 
	
	
			
		  track();
	    
}


void track()
{// this gets the distance for the first ping sensor 
		
	  int difference , duration, distance;

	  distance = ultrasonicRoutine(trig1,echo1,200);
	  Serial.print("ping 1");    
	  
	  Serial.print("\t");  
	  Serial.print(distance);
 
	 

	 int duration2 , distance2;

	  distance2 = ultrasonicRoutine(trig2,echo2,200);
	  
	  Serial.print("\t");     
	  Serial.print("ping 2");    
	 
	  Serial.print("\t");  
	  Serial.print(distance2);
	  difference = distance - distance2;

	 

	 int duration3 , distance3, difference2;

	  distance3 = ultrasonicRoutine(trig3,echo3,200);
	  Serial.print("\t");     
	  Serial.print("ping 3");    
	  Serial.print("\t");  
	  Serial.print(distance3);
	  int difference23;
	  difference23 = distance2 - distance3;

	  int difference31;
	  difference31 = distance - distance3;

	 float tolerance;
		tolerance = (float(distance) / 10.0) + .5;
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
		closetolerance = (float(distance) / 7.0) + .5;
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
   
	if (distance  < distance2  && distance  < distance2  && currentPosition >= 10)  

		{ 
		
   
		 delay (5);
		 currentPosition = currentPosition - dTheta ;
		 turret.write(currentPosition );

	   }

	if (distance  > distance2 && distance > distance2 && currentPosition <= 170 )

	   {
		 delay (5);
		 currentPosition = currentPosition + dTheta;
		 turret.write(currentPosition );

		 }
  
		if ( distance3< distance && distance3 < distance  && distance3 < distance2  && distance3 <distance)
		  {
				distance3 = ultrasonicRoutine(trig3,echo3,200);
					if (distance3< distance && distance3 < distance  && distance3 < distance2  && distance3 <distance)
					  {
					  
						speakerloop();

					   digitalWrite(53,HIGH);
					   delay(500);
					   digitalWrite(53,LOW);
					   }
		  }
	  Serial.print("\t");
	  Serial.print("theta");
	  Serial.print("\t");
	  Serial.println(currentPosition); 
}

void speakerloop() {
	for (int i=10000; i<11000; i++) {
		tone (speaker, i);
		delay (2);
		noTone (speaker);
	}	
}
	




int ultrasonicRoutine(int trig1pin, int echo1pin, int maxdistance) {

	int readstart, readend, duration, distance;

	// Start Reading
	readstart = millis();
	
	// Begin ping
	digitalWrite(trig1pin, LOW);
	delay(1);
	digitalWrite(trig1pin, HIGH);
	delay(1);
	digitalWrite(trig1pin, LOW);
	
	duration = pulseIn(echo1pin, HIGH);
	
	distance = (duration/2) / 29.1;

	// Storing distance value
	if(distance == 0 || distance > maxdistance) {
		distance = maxdistance;
	} else if(distance < 2 && distance != 0) {
		distance = ultrasonicRoutine(trig1pin,echo1pin,maxdistance);
	} else if(distance >= 2 && distance <= maxdistance) {
		distance = distance;
	}

	// Checking the time since beginning of last reading (only to reduce time between pings, thus no need to add delay after function)
	readend = millis() - readstart;
	if(readend < 75) {
		delay(75 - readend);
	}
	
	// Return the distance
	return distance;
}