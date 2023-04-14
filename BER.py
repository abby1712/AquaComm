import serial
import csv

# Open the serial port
ser = serial.Serial('/dev/cu.usbmodem14101', 115200)

# Wait for the Arduino to reset
ser.setDTR(False)
ser.setRTS(False)
ser.setDTR(True)
ser.flushInput()

with open('output.csv', 'a') as file:
    writer = csv.writer(file)
    while True:
        # Read a line of serial input from the Arduino
        line = ser.readline().decode('utf-8').strip()
        
        # Parse the input and write it to the CSV file
        values = line.split(',')
        if len(values) >= 4:
            received_bits = values[0].split(':')[1].strip()
            bit_error_rate = values[1].split(':')[1].strip()
            success_rate = values[2].split(':')[1].strip()
            quality_factor = values[3].split(':')[1].strip()
            writer.writerow([received_bits, bit_error_rate, success_rate, quality_factor])
