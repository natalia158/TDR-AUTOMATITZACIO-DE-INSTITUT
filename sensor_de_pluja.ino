#include <ESP32Servo.h>

double Pluja;
Servo servo_13;

void setup()
{
  	servo_13.attach(13);
	pinMode(26, INPUT);

	Serial.begin(115200);
	Serial.flush();
	while(Serial.available()>0)Serial.read();

	Pluja = 0;
	servo_13.write(0);

}


void loop()
{
	yield();

  	Pluja = map(analogRead(26),0,4095,0,100);
  	if ((Pluja < 50)) {
  		servo_13.write(105);
  	}
  	else {
  		servo_13.write(0);
  	}

  	Serial.println(String(""));
  	delay(200);

}