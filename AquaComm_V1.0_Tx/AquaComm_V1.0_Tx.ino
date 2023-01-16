#include <SHA256.h>
#include <Manchester.h>
#include <AES.h>

// Seed value known by both transmitter and receiver
const byte seed[] = {Value Comes Here};

// Pin configurations
const int LED_PIN = 13;
const int PHOTODIODE_PIN = 12;
const int BLINK_LED_PIN = 11;
const int LOS_LED_PIN = 10;
const int MESSAGE_LENGTH = 256;
const int MANCHESTER_DELAY = 500;
const int DUMMY_SIGNAL_DELAY = 1000;
const int RETRANSMISSION_DELAY = 500;

SHA256 sha256;
byte key[32];
AES aes;
Manchester manchester;

void setup() {
  // Generate key from seed using SHA256 library
  sha256.initHmac(seed, sizeof(seed));
  sha256.resultHmac(key, sizeof(key));

  // Set up AES encryption using the generated key
  aes.set_key(key, sizeof(key));

  // Set up Manchester modulation
  manchester.setupManchester();

  // Set pinModes
  pinMode(LED_PIN, OUTPUT);
  pinMode(PHOTODIODE_PIN, INPUT);
  pinMode(BLINK_LED_PIN, OUTPUT);
  pinMode(LOS_LED_PIN, OUTPUT);
}

void loop() {
  // Send dummy message periodically to check for LOS
  sendDummySignal();

  // Check for input from serial
  if (Serial.available() > 0) {
    String message = Serial.readString();
    byte byte_message[MESSAGE_LENGTH];
    message.getBytes(byte_message, MESSAGE_LENGTH);

    // blink the LED
    digitalWrite(BLINK_LED_PIN, HIGH);
    delay(100);
    digitalWrite(BLINK_LED_PIN, LOW);

    // Encrypt message using AES
    aes.encrypt(byte_message);

    // Modulate message using Manchester
    byte encoded[MESSAGE_LENGTH];
    manchester.encode(byte_message, encoded, MESSAGE_LENGTH);

    // Transmit message
    for (int i = 0; i < MESSAGE_LENGTH; i++) {
      digitalWrite(LED_PIN, encoded[i]);
      delayMicroseconds(MANCHESTER_DELAY);
    }

    // Wait for acknowledgement from receiver
    if (digitalRead(PHOTODIODE_PIN) == HIGH) {
      // LOS LED glows
      digitalWrite(LOS_LED_PIN, HIGH);
      Serial.println("Message transmitted successfully");
    } else {
      // LOS LED turns off
      digitalWrite(LOS_LED_PIN, LOW);
      Serial.println("Message not transmitted successfully, retransmitting");
      delay(RETRANSMISSION_DELAY);
      // retransmit the message
      for (int i = 0; i < MESSAGE_LENGTH; i++) {
        digitalWrite(LED_PIN, encoded[i]);
        delayMicroseconds(MANCHESTER_DELAY);
      }
    }
  }
}

void sendDummySignal() {
  // Send dummy signal
  digitalWrite(LED_PIN, HIGH);
  delayMicroseconds(DUMMY_SIGNAL_DELAY);
  digitalWrite(LED_PIN, LOW);

  // Wait for acknowledgement from receiver
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
