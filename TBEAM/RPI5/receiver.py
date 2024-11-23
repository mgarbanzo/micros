import time
import board
import busio
import digitalio
import adafruit_rfm9x
import adafruit_ssd1306
from digitalio import DigitalInOut
import os

# LoRa Parameters
RADIO_FREQ_MHZ = 915.0  # Frequency of the radio in MHz
TX_POWER = 23           # Transmit power in dB

# OLED Parameters
OLED_WIDTH = 128
OLED_HEIGHT = 32

# FIFO Path
FIFO_PATH = "/tmp/lora_fifo"

def setup_fifo():
    """Create a FIFO file if it doesn't exist."""
    if not os.path.exists(FIFO_PATH):
        os.mkfifo(FIFO_PATH)

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
    cs = digitalio.DigitalInOut(board.CE1)
    reset = digitalio.DigitalInOut(board.D25)
    rfm9x = adafruit_rfm9x.RFM9x(spi, cs, reset, RADIO_FREQ_MHZ)
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
    """Main function to gather LoRa packets, timestamp them, and send to FIFO."""
    # Setup OLED, LoRa, and FIFO
    setup_fifo()
    display = setup_oled()
    rfm9x = setup_lora()

    print("Waiting for packets...")
    display_message(display, "Waiting for packets...")

    # Open FIFO for writing
    with open(FIFO_PATH, 'w') as fifo:
        while True:
            # Wait for a LoRa packet
            packet = rfm9x.receive(timeout=5.0)
            if packet is not None:
                try:
                    # Decode packet and add timestamp
                    message = packet.decode('utf-8').strip()
                    timestamp = time.strftime("%Y-%m-%dT%H:%M:%S")
                    formatted_message = f"{timestamp},{message}"

                    # Display and print the message
                    display_message(display, message)
                    print(formatted_message)

                    # Write to FIFO
                    fifo.write(f"{formatted_message}\n")
                    fifo.flush()
                except UnicodeDecodeError:
                    print("Received invalid packet")
            else:
                print("No packet received")

if __name__ == "__main__":
    main()
