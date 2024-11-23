import os
import json
import threading
from flask import Flask, jsonify, render_template_string

# Configuration
FIFO_PATH = "/tmp/lora_map_fifo"
DEFAULT_LAT = 9.864  # Cartago, Costa Rica
DEFAULT_LON = -83.919
DEFAULT_ZOOM = 12

# Shared data storage for received data
data_store = {}  # Dictionary to store traces for each ID

# Color map for IDs
COLOR_MAP = {
    "A": "red", "B": "blue", "C": "green", "D": "purple", "E": "orange",
    "F": "yellow", "G": "pink", "H": "brown", "I": "gray", "J": "cyan",
    "K": "lime", "L": "magenta", "M": "navy", "N": "olive", "O": "teal",
    "P": "gold", "Q": "indigo", "R": "coral", "S": "darkgreen", "T": "silver",
    "U": "tan", "V": "salmon", "W": "chocolate", "X": "plum", "Y": "crimson", "Z": "khaki"
}

def assign_color(id):
    """Assign a color to the given ID based on its first letter."""
    first_char = id[0].upper() if id else "A"  # Default to "A" if ID is empty
    return COLOR_MAP.get(first_char, "black")  # Default to "black" if not in COLOR_MAP

def setup_fifo():
    """Ensure the FIFO exists."""
    if not os.path.exists(FIFO_PATH):
        os.mkfifo(FIFO_PATH)

def read_fifo():
    """Continuously read from the FIFO and update the shared data store."""
    setup_fifo()
    print(f"Reading from FIFO: {FIFO_PATH}")
    with open(FIFO_PATH, 'r') as fifo:
        while True:
            line = fifo.readline().strip()
            if line:
                print(f"Received: {line}")
                try:
                    parsed = json.loads(line)
                    cow_id = parsed["id"]
                    parsed["color"] = assign_color(cow_id)  # Add color to the data
                    if cow_id not in data_store:
                        data_store[cow_id] = []  # Initialize trace for new ID
                    data_store[cow_id].append(parsed)  # Add to trace
                except json.JSONDecodeError:
                    print(f"Invalid JSON: {line}")

app = Flask(__name__)

@app.route("/")
def index():
    """Serve the main map page."""
    return render_template_string("""
    <!DOCTYPE html>
    <html>
    <head>
        <title>Real-Time Tracking</title>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" href="https://unpkg.com/leaflet/dist/leaflet.css" />
        <style>
            #map {
                height: 100vh;
            }
            .text-icon {
                font-size: 16px;
                font-weight: bold;
                color: white;
                text-align: center;
                width: 24px;
                height: 24px;
                line-height: 24px;
                background-color: black;
                border-radius: 50%;
                display: inline-block;
                border: 2px solid white;
            }
        </style>
    </head>
    <body>
        <div id="map"></div>
        <script src="https://unpkg.com/leaflet/dist/leaflet.js"></script>
        <script>
            const defaultLat = {{ default_lat }};
            const defaultLon = {{ default_lon }};
            const defaultZoom = {{ default_zoom }};

            // Initialize the map
            const map = L.map('map').setView([defaultLat, defaultLon], defaultZoom);

            // Add a tile layer
            L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
                maxZoom: 19,
                attribution: '© OpenStreetMap contributors'
            }).addTo(map);

            // Store markers and polylines
            const markers = {};
            const polylines = {};

            function createTextIcon(letter, color) {
                const div = document.createElement("div");
                div.className = "text-icon";
                div.style.backgroundColor = color;
                div.innerHTML = letter;
                return L.divIcon({ className: "", html: div.outerHTML });
            }

            async function updateMap() {
                try {
                    // Fetch data from the server
                    const response = await fetch('/data');
                    const data = await response.json();

                    // Update map for each ID
                    for (const id in data) {
                        const trace = data[id];

                        // Draw trace (polyline)
                        const coordinates = trace
                            .filter(point => point.gps_lock === 1)
                            .map(point => [point.latitude, point.longitude]);

                        if (coordinates.length > 1) {
                            // Update or create the polyline
                            if (!polylines[id]) {
                                polylines[id] = L.polyline(coordinates, {
                                    color: trace[0].color,  // Use the first trace point's color
                                    weight: 2.5,
                                    opacity: 0.8
                                }).addTo(map);
                            } else {
                                polylines[id].setLatLngs(coordinates);
                            }
                        }

                        // Update or create the marker for the latest position
                        const latest = trace[trace.length - 1];
                        if (markers[id]) {
                            map.removeLayer(markers[id]);
                        }

                        if (latest.gps_lock === 1) {
                            markers[id] = L.marker([latest.latitude, latest.longitude], {
                                icon: createTextIcon(latest.id[0], latest.color),
                            }).addTo(map).bindPopup(`<b>ID: ${latest.id}</b><br>Satélites: ${latest.satellites}`);
                        }
                    }
                } catch (error) {
                    console.error('Error updating map:', error);
                }
            }

            // Update the map every 5 seconds
            setInterval(updateMap, 5000);

            // Initial map load
            updateMap();
        </script>
    </body>
    </html>
    """, default_lat=DEFAULT_LAT, default_lon=DEFAULT_LON, default_zoom=DEFAULT_ZOOM)

@app.route("/data")
def get_data():
    """Provide the current data store as JSON."""
    return jsonify(data_store)

# Run the FIFO reader in a separate thread
fifo_thread = threading.Thread(target=read_fifo, daemon=True)
fifo_thread.start()

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)

