import pygame

pygame.init()
pygame.joystick.init()


joystick = pygame.joystick.Joystick(0)
joystick.init()

print(f"Detected controller: {joystick.get_name()}")

while True:
    pygame.event.pump()

    # left_x = joystick.get_axis(0)  
    # left_y = joystick.get_axis(1)  

    # print(f"Left Stick X: {left_x:.2f} | Y: {left_y:.2f}")

    if joystick.get_button(0):
        print(f"X pressed")
