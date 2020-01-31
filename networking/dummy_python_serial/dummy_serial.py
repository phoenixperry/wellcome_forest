import serial
import time

if __name__ == "__main__":
    ARD_PORT = "COM11" # COM3 or /dev/ttyACM0
    ser = serial.Serial(ARD_PORT, baudrate=9600, timeout=1)
    time_between_updates = 0.250  # 250ms
    last_update = time.time()

    while True:
        radio_state = ser.readline()
        if radio_state:
            print(radio_state)
        if (time.time()-last_update)>time_between_updates:
            last_update = time.time()
            ser.write('{111211311411511611711811911A11B11}\n'.encode('utf-8'))  # update the full state for devices 1-B. Length is 3 + num_states*num_devices. Currently 36.
            # ser.write("b\n".encode('utf-8'))