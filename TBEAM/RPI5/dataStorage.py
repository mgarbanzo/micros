import os
import json

# Paths
FIFO_PATH = "/tmp/lora_fifo"  # Input FIFO from LoRa receiver
FIFO_MAP_PATH = "/tmp/lora_map_fifo"  # Output FIFO for the map
JSON_FILE = "cow_data.json"

def setup_fifos():
    """Create the necessary FIFO files if they don't exist."""
    for fifo in [FIFO_PATH, FIFO_MAP_PATH]:
        if not os.path.exists(fifo):
            os.mkfifo(fifo)

def read_fifo():
    """Read data from the input FIFO and process it."""
    print(f"Reading from FIFO: {FIFO_PATH}")
    with open(FIFO_PATH, 'r') as fifo:
        while True:
            line = fifo.readline().strip()
            if line:
                print(f"Received: {line}")
                # Parse the data and save to JSON and map FIFO
                parsed_data = parse_data(line)
                append_to_json_and_fifo(parsed_data)

def parse_data(line):
    """Parse a line of data from the FIFO."""
    try:
        timestamp, message = line.split(",", 1)
        parts = message.split(",")
        cow_id = parts[0]
        gps_lock = int(parts[1])
        satellites = int(parts[2]) if gps_lock == 1 else None
        latitude = float(parts[3]) if gps_lock == 1 else None
        longitude = float(parts[4]) if gps_lock == 1 else None

        return {
            "timestamp": timestamp,
            "id": cow_id,
            "gps_lock": gps_lock,
            "satellites": satellites,
            "latitude": latitude,
            "longitude": longitude,
        }
    except Exception as e:
        print(f"Error parsing line: {line} - {e}")
        return None

def append_to_json_and_fifo(data):
    """Append parsed data to the JSON file and send to the map FIFO."""
    if data is None:
        return

    # Ensure the JSON file exists
    if not os.path.exists(JSON_FILE):
        with open(JSON_FILE, 'w') as f:
            json.dump([], f)

    # Append the data to the JSON file
    with open(JSON_FILE, 'r+') as f:
        try:
            records = json.load(f)
        except json.JSONDecodeError:
            records = []  # If the file is empty or corrupted
        records.append(data)
        f.seek(0)
        json.dump(records, f, indent=2)
        f.truncate()

    # Write the data to the map FIFO for real-time updates
    try:
        with open(FIFO_MAP_PATH, 'w') as fifo:
            fifo.write(json.dumps(data) + "\n")
            fifo.flush()
    except BrokenPipeError:
        print("Map FIFO is not being read. Skipping real-time update.")

if __name__ == "__main__":
    setup_fifos()
    read_fifo()
