const int photodiodePin = 7;
const int bitDelay = 10; // time delay between each bit in milliseconds
byte bitsToReceive[] = {0b11001100,0b11000000,0b11000100,0b11010100,0b11000000,0b11001100,0b11000000,0b11000100,0b11010100,0b11000000,0b11001100,0b11000000,0b11000100,0b11010100,0b11000000}; // Predefined set of bits to receive

void setup() {
  Serial.begin(9600);
} 

void loop() {
  int lightLevel = analogRead(photodiodePin);
  if (lightLevel > 700) { // message detected
    delay(bitDelay); // wait for the message to start
    byte receivedBits = 0;
    for (int i = 0; i < 8; i++) { // read each bit
      delay(bitDelay);
      int bitValue = analogRead(photodiodePin) > 512;
      receivedBits |= (bitValue << i);
    }
    delay(bitDelay); // wait for the end of message
    int ber = calculateBitErrorRate(bitsToReceive, receivedBits); // calculate the bit error rate
    float successRate = (1.0 - ((float)ber / 8.0)) * 100.0; // calculate the percentage of successful transmission
    float qualityFactor = calculateQualityFactor(successRate); // calculate the quality factor
    Serial.println();
    Serial.print("Received bits: ");
    Serial.print(receivedBits, BIN);
    Serial.print(", Bit Error Rate: ");
    Serial.print(ber);
    Serial.print(", Success Rate: ");
    Serial.print(successRate);
    Serial.print("%, Quality Factor: ");
    Serial.print(qualityFactor);

  }
}

int calculateBitErrorRate(byte* expectedBits, byte receivedBits) {
  int errors = 0;
  for (int i = 0; i < 8; i++) {
    if (((expectedBits[0] >> i) & 1) != ((receivedBits >> i) & 1)) {
      errors++;
    }
  }
  return errors;
}
float calculateQualityFactor(float successRate) {
  float q = 1.0 / (2.0 * sqrt(2.0) * pow(successRate / 100.0, 2.0)); // calculate the quality factor
  return q;
}
