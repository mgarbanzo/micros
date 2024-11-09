from adafruit_circuitplayground import cp

cp.pixels.brightness = 0.1

while True:
    cp.pixels[0] = (0, 0, 255)
