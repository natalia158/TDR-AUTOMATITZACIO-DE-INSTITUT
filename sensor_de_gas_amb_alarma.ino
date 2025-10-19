const int brunzidor = 11;
const int sensorGas = A0;
int limit = 400;

const unsigned int freqAlarma = 2000;
const unsigned long pitoON   = 200;
const unsigned long pitoOFF  = 200;

void setup() {
  pinMode(brunzidor, OUTPUT);
  noTone(brunzidor);
  Serial.begin(9600);
}

void loop() {
  int gas = analogRead(sensorGas);
  Serial.print("Lectura gas: ");
  Serial.println(gas);

  if (gas > limit) {
    tone(brunzidor, freqAlarma);
    delay(pitoON);
    noTone(brunzidor);
    delay(pitoOFF);
  } else {
    noTone(brunzidor);
    delay(100);
  }
}