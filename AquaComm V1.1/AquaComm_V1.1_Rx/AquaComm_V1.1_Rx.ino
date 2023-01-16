#include <CRC32.h>
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
const int SERVO_X_PIN = 9;
const int SERVO_Y_PIN = 8;
const int AMBIENT_LIGHT_PIN = A0;

SHA256 sha256;
byte key[32];
AES aes;
Manchester manchester;
CRC32 crc;
Servo servoX, servoY;

void setup() {
  Serial.begin(9600);

  // Generate AES key
  sha256.resetHMAC(seed, sizeof(seed));
  sha256.finalizeHMAC(key, sizeof(key));
  aes.set_key(key, sizeof(key));

  // Configure pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(PHOTODIODE_PIN, INPUT);
  pinMode(BLINK_LED_PIN, OUTPUT);
  pinMode(LOS_LED_PIN, OUTPUT);
  pinMode(SERVO_X_PIN, OUTPUT);
  pinMode(SERVO_Y_PIN, OUTPUT);
  pinMode(AMBIENT_LIGHT_PIN, INPUT);

  // Attach servos to pins
  servoX.attach(SERVO_X_PIN);
  servoY.attach(SERVO_Y_PIN);
}

void loop() {
  // Check for LOS
  if (checkForLos()) {
    digitalWrite(LOS_LED_PIN, HIGH);
  } else {
    digitalWrite(LOS_LED_PIN, LOW);
    // Perform sweep algorithm if LOS is lost
    performSweep();
    return;
  }

  // Receive message
  byte encoded[MESSAGE_LENGTH];
  for (int i = 0; i < MESSAGE_LENGTH; i++) {
    encoded[i] = receiveByte();
  }

  // Receive the crc value
  uint32_t crcValue = receiveCRC();
  // Demodulate message using Manchester
  byte message[MESSAGE_LENGTH];
  manchester.decode(encoded, message, MESSAGE_LENGTH);

  // Decrypt message using AES
  aes.decrypt(message);

  // Calculate the crc value for the received message
  uint32_t receivedCRC = crc.calculate(message, MESSAGE_LENGTH);

  // Compare the received crc value with the calculated value
  if (crcValue != receivedCRC) {
    Serial.println("CRC check failed!");
    return;
  }

  // Print message to screen
  Serial.println(message);

  // send acknowledgement
  digitalWrite(LED_PIN, HIGH);
  delay(DUMMY_SIGNAL_DELAY);
  digitalWrite(LED_PIN, LOW);

  // Check the ambient light level
  int ambientLight = analogRead(AMBIENT_LIGHT_PIN);
  if (ambientLight > 500) {
    Serial.println("High ambient light detected!");
  }
}

bool checkForLos() {
  digitalWrite(LED_PIN, HIGH);
  delay(DUMMY_SIGNAL_DELAY);
  digitalWrite(LED_PIN, LOW);
  delay(DUMMY_SIGNAL_DELAY);
  return digitalRead(PHOTODIODE_PIN) == HIGH;
}

void performSweep() {
  for (int i = 0; i < 180; i++) {
    servoX.write(i);
    delay(15);
    if (checkForLos()) {
      return;
    }
  }
  for (int i = 180; i > 0; i--) {
    servoX.write(i);
    delay(15);
    if (checkForLos()) {
      return;
    }
  }
  for (int i = 0; i < 180; i++) {
    servoY.write(i);
    delay(15);
    if (checkForLos()) {
      return;
    }
  }
  for (int i = 180; i > 0; i--) {
    servoY.write(i);
    delay(15);
    if (checkForLos()) {
      return;
    }
  }
}

byte receiveByte() {
  byte data = 0;
  for (int i = 0; i < 8; i++) {
    data |= digitalRead(PHOTODIODE_PIN) << i;
    delay(MANCHESTER_DELAY);
  }
  return data;
}

uint32_t receiveCRC() {
  uint32_t crcValue = 0;
  for (int i = 0; i < 32; i++) {
    crcValue |= digitalRead(PHOTODIODE_PIN) << i;
    delay(MANCHESTER_DELAY);
  }
  return crcValue;
}
