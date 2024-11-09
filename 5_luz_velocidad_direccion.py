from adafruit_circuitplayground import cp
import time

# Configuración inicial
cp.pixels.brightness = 0.1  # Brillo de los NeoPixels
color = (0, 255, 0)  # Color inicial: verde
RETRASO_MINIMO = 0.01  # Velocidad máxima (retraso mínimo)
RETRASO_MAXIMO = 1.0   # Velocidad mínima (retraso máximo)
PASO = 0.05            # Paso para aumentar/disminuir la velocidad
retraso = 0.2          # Retraso inicial entre cambios de luces
ultimo_tiempo_boton = 0  # Tiempo de la última presión de botón
INTERVALO_MINIMO_BOTON = 0.2  # Intervalo mínimo entre presiones
pixel_actual = 0  # Índice del píxel inicial para la animación

while True:
    tiempo_actual = time.monotonic()  # Tiempo actual en segundos

    # Detectar presiones de los botones
    if cp.button_a and (tiempo_actual - ultimo_tiempo_boton >= INTERVALO_MINIMO_BOTON):
        # Aumentar el retraso (hacer más lenta la animación)
        if retraso + PASO <= RETRASO_MAXIMO:  # Solo incrementar si no excede el máximo
            retraso += PASO
            print(f"¡Botón A presionado! Retraso aumentado a {retraso:.2f} segundos")
        ultimo_tiempo_boton = tiempo_actual

    if cp.button_b and (tiempo_actual - ultimo_tiempo_boton >= INTERVALO_MINIMO_BOTON):
        # Reducir el retraso (hacer más rápida la animación)
        if retraso - PASO >= RETRASO_MINIMO:  # Solo decrementar si no cae por debajo del mínimo
            retraso -= PASO
            print(f"¡Botón B presionado! Retraso reducido a {retraso:.2f} segundos")
        ultimo_tiempo_boton = tiempo_actual

    # Determinar dirección de giro según el estado del interruptor
    if cp.switch:
        direccion = 1  # Dirección normal (hacia adelante)
    else:
        direccion = -1  # Dirección inversa (hacia atrás)

    # Animación de giro de las luces
    cp.pixels.fill((0, 0, 0))  # Apaga todas las luces
    cp.pixels[pixel_actual] = color  # Enciende el píxel actual con el color definido
    pixel_actual = (pixel_actual + direccion) % 10  # Avanza o retrocede al siguiente píxel
    time.sleep(retraso)  # Usa el retraso actualizado
