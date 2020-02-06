from pyfirmata import Arduino, util
import msvcrt

speed = 5
initial_angle = 90
board = Arduino('COM11')

print ("press left or right arros to move the servo:")
print ("press e to terminate the program:")
data = initial_angle
while True:
    c = ord(msvcrt.getch())
    if c != 224 :
        if c== 101:
            print ("program terminated.")
            break
            print()
        if c == 75 :
            data -=speed
            if data < 0 :
                data = 0 
        if c == 77 :
            data +=speed
            if data > 180 :
                data = 180
        board.send_sysex(0x71, util.str_to_two_byte_iter(f"{data}\0"))