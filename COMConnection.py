import serial
import time
import pygame
import sys

pygame.init()
pygame.joystick.init()

if pygame.joystick.get_count() == 0:

    print("No controller detected. Please connect a controller and restart.")
    sys.exit(1)

joystick = pygame.joystick.Joystick(0)
joystick.init()
print(f"Detected controller: {joystick.get_name()}")

try: 

    arduino = serial.Serial('COM4', 9600, timeout=1)
    time.sleep(2)  
    print("Connected to Arduino on COM3")

except serial.SerialException:

    print("Failed to connect to Arduino on COM3. Make sure it’s plugged in and not locked by another program.")
    sys.exit(1)



data_packet = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0]
last_packet = data_packet.copy()

def apply_deadzone(value, threshold=0.1):

    return 0.0 if abs(value) < threshold else round(value, 2)

def update_sticks():

    global data_packet

    data_packet[0] = apply_deadzone(joystick.get_axis(0))  
    data_packet[1] = apply_deadzone(joystick.get_axis(1))  
    data_packet[2] = apply_deadzone(joystick.get_axis(2))  
    data_packet[3] = apply_deadzone(joystick.get_axis(3))  
    data_packet[4] = round((joystick.get_axis(4) + 1) / 2, 2)  
    data_packet[5] = round((joystick.get_axis(5) + 1) / 2, 2)  



def update_buttons():
    global data_packet

    data_packet[6] = joystick.get_button(0)  

    data_packet[7] = joystick.get_button(2) 

    data_packet[8] = joystick.get_button(3)

    data_packet[9] = joystick.get_button(1)  

    data_packet[10] = joystick.get_button(9) 

    data_packet[11] = joystick.get_button(10)  


def send_to_arduino():
    global data_packet, last_packet
    if data_packet != last_packet:
        packet = ",".join(str(x) for x in data_packet) + "\n"
        arduino.write(packet.encode())
        print("Sent to Arduino:", packet.strip())
        last_packet = data_packet.copy()



def main():
    
    while True:

        pygame.event.pump()
        update_buttons()
        update_sticks()
        send_to_arduino()
        time.sleep(0.1)

if __name__ == "__main__":
    main()
