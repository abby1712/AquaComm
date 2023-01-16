#include <CRC32.h>
#include <SHA256.h>
#include <Manchester.h>
#include <AES.h>

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
CRC32 crc;

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
}

void loop() {
  // Send dummy signal periodically to check for LOS
  sendDummySignal();

  // Take input message from serial
  String input;
  input = Serial.readString();
  input.trim();

  // Convert input message to byte array
  byte message[MESSAGE_LENGTH];
  input.getBytes(message, MESSAGE_LENGTH);
  
  // Calculate the crc value for the message
  uint32_t crcValue = crc.calculate(message, MESSAGE_LENGTH);

  // Encrypt message using AES
  aes.encrypt(message);

  // Modulate message using Manchester
  byte encoded[MESSAGE_LENGTH];
  manchester.encode(message, encoded, MESSAGE_LENGTH);

  // Transmit message
    for (int i = 0; i < MESSAGE_LENGTH; i++) {
    digitalWrite(BLINK_LED_PIN, HIGH);
    transmitByte(encoded[i]);
    digitalWrite(BLINK_LED_PIN, LOW);
    delay(MANCHESTER_DELAY);
  }
  // Transmit the crc value
  transmitCRC(crcValue);
  
  // Wait for acknowledgement from receiver
  if (waitForAck()) {
    digitalWrite(LOS_LED_PIN, HIGH);
  } else {
    digitalWrite(LOS_LED_PIN, LOW);
    // Retransmit message if no acknowledgement is received
    delay(1000);
    loop();
  }
}

void sendDummySignal() {
  digitalWrite(LED_PIN, HIGH);
  delay(DUMMY_SIGNAL_DELAY);
  digitalWrite(LED_PIN, LOW);
  delay(DUMMY_SIGNAL_DELAY);
}

void transmitByte(byte data) {
  for (int i = 0; i < 8; i++) {
    if (data & 0x01) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
    data >>= 1;
    delay(MANCHESTER_DELAY);
  }
}

void transmitCRC(uint32_t crcValue) {
  for (int i = 0; i < 32; i++) {
    if (crcValue & 0x01) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
    crcValue >>= 1;
    delay(MANCHESTER_DELAY);
  }
}

bool waitForAck() {
  unsigned long start = millis();
  while (millis() - start < 1000) {
    if (digitalRead(PHOTODIODE_PIN) == HIGH) {
      return true;
    }
  }
  return false;
}
