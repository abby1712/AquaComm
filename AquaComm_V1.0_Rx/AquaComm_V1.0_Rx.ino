#include <SHA256.h>
#include <Manchester.h>
#include <AES.h>
#include <Servo.h>

// Seed value known by both transmitter and receiver
const byte seed[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};

// Pin configurations
const int LED_PIN = 13;
const int PHOTODIODE_PIN = 12;
const int BLINK_LED_PIN = 11;
const int LOS_LED_PIN = 10;
const int MESSAGE_LENGTH = 256;
const int MANCHESTER_DELAY = 500;
const int DUMMY_SIGNAL_DELAY = 1000;

SHA256 sha256;
byte key[32];
AES aes;
Manchester manchester;
Servo servoX, servoY;

void setup() {
  // Generate key from seed using SHA256 library
  sha256.initHmac(seed, sizeof(seed));
  sha256.resultHmac(key, sizeof(key));

  // Set up AES encryption using the generated key
  aes.set_key(key, sizeof(key));

  // Set up Manchester modulation
  manchester.setupManchester();

  // Attach servos to pins
  servoX.attach(9);
  servoY.attach(8);

  // Set pinModes
  pinMode(LED_PIN, OUTPUT);
  pinMode(PHOTODIODE,INPUT);
    pinMode(BLINK_LED_PIN, OUTPUT);
  pinMode(LOS_LED_PIN, OUTPUT);
}

void loop() {
  // Send dummy signal periodically to check for LOS
  sendDummySignal();

  // Receive message
  byte encoded[MESSAGE_LENGTH];
  int index = 0;
  // blink the LED
  digitalWrite(BLINK_LED_PIN, HIGH);
  delay(100);
  digitalWrite(BLINK_LED_PIN, LOW);

  while (digitalRead(PHOTODIODE_PIN) == LOW) {
    encoded[index] = digitalRead(PHOTODIODE_PIN);
    delayMicroseconds(MANCHESTER_DELAY);
    index++;
  }
  // Decode message using Manchester
  byte byte_message[MESSAGE_LENGTH];
  manchester.decode(encoded, byte_message, MESSAGE_LENGTH);

  // Decrypt message using AES
  aes.decrypt(byte_message);

  // Print message to serial
  String message = String((char*) byte_message);
  Serial.println(message);

  // Send acknowledgement to transmitter
  digitalWrite(LED_PIN, HIGH);
  delayMicroseconds(DUMMY_SIGNAL_DELAY);
  digitalWrite(LED_PIN, LOW);

  // Perform sweep algorithm to search for LOS
  for (int i = 0; i < 180; i++) {
    servoX.write(i);
    delay(15);
    if (digitalRead(PHOTODIODE_PIN) == HIGH) {
      break;
    }
  }
  for (int i = 0; i < 180; i++) {
    servoY.write(i);
    delay(15);
    if (digitalRead(PHOTODIODE_PIN) == HIGH) {
      break;
    }
  }
}

void sendDummySignal() {
  // Send dummy signal
  digitalWrite(LED_PIN, HIGH);
  delayMicroseconds(DUMMY_SIGNAL_DELAY);
  digitalWrite(LED_PIN, LOW);

  // Wait for acknowledgement from transmitter
  if (digitalRead(PHOTODIODE_PIN) == HIGH) {
    // LOS LED glows
    digitalWrite(LOS_LED_PIN, HIGH);
    Serial.println("Transmitter and receiver are in LOS");
  } else {
    // LOS LED turns off
    digitalWrite(LOS_LED_PIN, LOW);
    Serial.println("Transmitter and receiver are not in LOS");
  }
}
