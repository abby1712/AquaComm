Algorithm for the transmitter:

Take the input message and encrypt it using AES encryption.
Modulate the encrypted message using Manchester coding.
Transmit the modulated message using the LED.
Wait for an acknowledgement from the receiver.
If an acknowledgement is received, the transmission was successful and the LOS LED glows.
If no acknowledgement is received, retransmit the message after a delay and go to step 4.
Send a dummy signal periodically to check for LOS.

 algorithm for the receiver:

Send a dummy signal to check for LOS.
Receive the modulated message using the photodiode.
Decode the modulated message using Manchester decoding.
Decrypt the decoded message using AES decryption.
Print the decrypted message to the screen.
Send an acknowledgement to the transmitter.
Perform a sweep algorithm to search for LOS if LOS is lost during the transmission.
If LOS is found during the sweep, the LOS LED glows, otherwise it turns off.
repeat from step 1
