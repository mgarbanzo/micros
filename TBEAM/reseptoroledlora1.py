# codigo generado por Jean Carlo Rodriguez Chaves 
#codigo funcional de resepcion de datos de un microcontrolador t-beam
#parametros a tener en cuenta que tienen que relacionarse en ambos equipos
#Frecuencia 915.0 
#Ancho de banda 125 kHz 
#spreading factor 7 
#Coding rate 7 
#SyncWord 0x12 
#PreambleLength 8 
##############################################################################################################################################
import time
import board
import busio
import digitalio
import adafruit_rfm9x
import adafruit_ssd1306
from digitalio import DigitalInOut

# LoRa Parameters
RADIO_FREQ_MHZ = 915.0  # Frequency of the radio in MHz
TX_POWER = 23           # Transmit power in dB

# OLED Parameters
OLED_WIDTH = 128
OLED_HEIGHT = 32


def setup_oled():
    """Initialize and return the OLED display."""
    i2c = busio.I2C(board.SCL, board.SDA)
    reset_pin = DigitalInOut(board.D4)
    display = adafruit_ssd1306.SSD1306_I2C(OLED_WIDTH, OLED_HEIGHT, i2c, reset=reset_pin)
    display.fill(0)
    display.show()
    return display

def setup_lora():
    """Initialize and return the LoRa radio."""
    # Set up SPI bus
    spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
    # Define chip select and reset pins
    cs = digitalio.DigitalInOut(board.CE1)
    reset = digitalio.DigitalInOut(board.D25)
    # Initialize the LoRa radio
    rfm9x = adafruit_rfm9x.RFM9x(spi, cs, reset, RADIO_FREQ_MHZ)
   # rfm9x.setFrequency(915.0);
    rfm9x.tx_power = TX_POWER
    rfm9x.signal_bandwidth = 125000
    rfm9x.spreading_factor = 7
    rfm9x.coding_rate = 7
    rfm9x.enable_crc = True
    rfm9x.preamble_length = 16
    return rfm9x

def display_message(display, message):
    """Display a message on the OLED."""
    display.fill(0)
    display.text(message[:OLED_WIDTH // 6], 0, 0, 1)  # Truncate if too long
    display.show()

def main():
    """Main function to gather LoRa packets, display them, and print them."""
    # Initialize OLED and LoRa
    display = setup_oled()
    rfm9x = setup_lora()

    print("Waiting for packets...")
    display_message(display, "Waiting for packets...")

    while True:
        # Wait for a LoRa packet
        packet = rfm9x.receive(timeout=5.0)
        
        if packet is not None:
            try:
                # Decode packet
                message = packet.decode('utf-8').strip()
                # Display and print the message
                display_message(display, message)
                print(f"Received: {message}")
            except UnicodeDecodeError:
                print("Received invalid packet")
        else:
            print("No packet received")

if __name__ == "__main__":
    main()
