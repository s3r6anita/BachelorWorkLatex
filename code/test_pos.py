import serial
from datetime import datetime

ser = serial.Serial('COM3', 115200, timeout=1)
with open("stm32_log.txt", "a") as file:
    while True:
        try:
            data = ser.readline().decode().strip()
            if data:
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                file.write(f"[{timestamp}] {data}\n")
                file.flush()
        except KeyboardInterrupt:
            break
ser.close()