const int ledPin = 7;
const int bitDelay = 10; // time delay between each bit in milliseconds

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  byte bitsToSend[] = {0b11001100,0b11000000,0b11000100,0b11010100,0b11000000,0b11001100,0b11000000,0b11000100,0b11010100,0b11000000,0b11001100,0b11000000,0b11000100,0b11010100,0b11000000}; // Predefined set of bits to send
  for (int i = 0; i < sizeof(bitsToSend); i++) { // send each set of bits
    byte bits = bitsToSend[i];
    sendBits(bits);
  }
  delay(1000); // delay for 1 second before sending the bits again
}

void sendBits(byte bits) {
  analogWrite(ledPin, 100); // start of message
  delay(bitDelay);
  analogWrite(ledPin, 0);
  delay(bitDelay);
  for (int i = 0; i < 8; i++) { // send each bit
    analogWrite(ledPin, ((bits & (1 << i)) != 0) * 100);
    delay(bitDelay);
  }
  analogWrite(ledPin, 100); // end of message
  delay(bitDelay);
  analogWrite(ledPin, 0);
  delay(bitDelay);
}
