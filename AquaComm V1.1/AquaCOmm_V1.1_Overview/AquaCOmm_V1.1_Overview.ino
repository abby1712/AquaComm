Overview:

Transmitter:

The transmitter has a static LED and a photodiode together.
It receives input from the serial port to transmit messages.
The input message is encrypted using AES encryption algorithm and the AES.h library.
After the encryption, the message is modulated using Manchester coding and the Manchester.h library.
The modulated message is then transmitted using the LED.
The photodiode present in the transmitter is ready to receive the acknowledgement sent back to it from the receiver.
The transmitter sends dummy messages periodically to check if the receiver is in LOS (line of sight).
Receiver:

The receiver has a dynamic LED and photodiode paired with servos in X and Y axis.
The photodiode receives the modulated message and decrypts it using AES decryption algorithm and AES.h library.
The decrypted message is then demodulated using Manchester decoding and the Manchester.h library.
The message is then printed on the screen.
The LED present in the receiver sends back the acknowledgement back to transmitter for successful transmission.
The receiver also sends dummy signals to the transmitter to inform that it is in LOS.
A sweep algorithm is in place in case if LOS is lost, or the receiver starts in a random position to lock the position for perfect transmission.
An ambient light sensor is used to check for errors caused by high ambient light.
A Cyclic Redundancy Check (CRC) algorithm is used to check the integrity of the received message.
In terms of code, the transmitter encrypts the message, modulates it and sends it using LED. The receiver receives the message, 
demodulates it, decrypts it, checks for errors using the ambient light sensor, verify the integrity of the message 
using the CRC algorithm, prints the message on the screen and sends back an acknowledgement to the transmitter. 
The receiver also uses a sweep algorithm to find LOS, 
in case it's lost.
