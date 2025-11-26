#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 9
#define SS_PIN 10
#define ledVerd 4
#define ledVermell 5
#define ledBlau 6


MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
Servo servo1;

byte tag1[4] = {0x63, 0x86, 0xCB, 0xA6};
byte tagActual[4];

void setup() {
  Serial.begin(9600);
   while (!Serial);
   SPI.begin();
   mfrc522.PCD_Init();
   servo1.attach(3);
   pinMode(ledVerd ,OUTPUT);
   pinMode(ledVermell,OUTPUT);
   pinMode(ledBlau, OUTPUT);
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent())
    return;

  if (!mfrc522.PICC_ReadCardSerial())
    return;

 Serial.print(F("Targeta UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
       
servo1.write(90);
digitalWrite(ledVerd ,LOW);
digitalWrite(ledVermell,LOW);
digitalWrite(ledBlau,LOW);
}

void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    tagActual[i] = buffer[i];
  }
  if(comparar(tagActual,tag1)){
    Serial.println("...Accés Permès...");
    digitalWrite(ledVerd, HIGH);
    digitalWrite(ledVermell, LOW);
    digitalWrite(ledBlau, LOW);
    servo1.write(0);
    delay(2000);
  }
  else {
    Serial.println("...Accés denegat...");
    digitalWrite(ledVermell,HIGH);
    digitalWrite(ledVermell, LOW);
    digitalWrite(ledBlau, LOW);
    delay(1000);
  }       
}

boolean comparar(byte array1[], byte array2[]){

  if(array1[0] != array2[0]) return(false);
  if(array1[1] != array2[1]) return(false);
  if(array1[2] != array2[2]) return(false);
  if(array1[3] != array2[3]) return(false);
  return(true);
}