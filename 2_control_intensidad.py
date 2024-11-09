from adafruit_circuitplayground import cp

cp.pixels.brightness = 0.1

while True:
    cp.pixels[0] = (0, 0, 255) # Puede cambiar el primer 0 de 0 a 9 para mover la luz. Tambien puede cambias los ceros y el 255 para cambiar de color.
