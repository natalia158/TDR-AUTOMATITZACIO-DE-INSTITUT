const int pin_humitat  = A0;
const int pin_nivellaigua = A1;
const int pin_relay = 7;

const bool relay_on  = HIGH;
const bool relay_off = LOW;

int limit_humitat = 400;
int limit_aigua = 400;

void setup(){
  pinMode(pin_relay, OUTPUT);
  digitalWrite(pin_relay, relay_off);

  Serial.begin(9600);
  Serial.println(F("Sistema iniciat"));
}

void loop(){
  int humitat= analogRead(pin_humitat);
  int aigua= analogRead(pin_nivellaigua);

  if (humitat < limit_humitat && aigua > limit_aigua){
    Serial.println(F("Bomba ON"));
    digitalWrite(pin_relay, relay_on);

    while (true){
      humitat= analogRead(pin_humitat);
      aigua= analogRead(pin_nivellaigua);

      if (humitat >= limit_humitat){
        Serial.println(F("Terra humida → bomba OFF"));
        break;
      }

      if (aigua <= limit_aigua){
        Serial.println(F("Nivell baix → bomba OFF"));
        break;
      }

      delay(20);
    }

    digitalWrite(pin_relay, relay_off);
  }

  delay(200);
}