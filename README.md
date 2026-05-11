# 🅿️ Parking Spot Monitor

A real-time parking spot detection system using an ESP32, ultrasonic sensors, and a web dashboard. The Arduino detects whether each spot is occupied and reports the status to a Flask backend, which serves a live dashboard viewable in any browser.

**Live demo:** [arduino-parking.vercel.app](https://arduino-parking.vercel.app)

---

## How It Works

```
[Ultrasonic Sensors] → [ESP32] → [Flask API] → [Web Dashboard]
```

Two HC-SR04 ultrasonic sensors (one per parking spot) measure distance every second. If an object is detected within 5 cm, the spot is marked **OCCUPIED**; otherwise it's **FREE**. The ESP32 sends updates to the Flask API only when the status changes, keeping network traffic minimal. The dashboard polls the API and updates in real time.

---

## Hardware Required

- ESP32 development board
- 2× HC-SR04 ultrasonic distance sensors
- 2× LEDs or indicators (optional, connected to pins 22 and 32)
- Jumper wires

**Wiring:**

| Component | ESP32 Pin |
|-----------|-----------|
| Sensor 1 TRIG | 13 |
| Sensor 1 ECHO | 18 |
| Sensor 2 TRIG | 14 |
| Sensor 2 ECHO | 21 |
| Indicator 1 | 22 |
| Indicator 2 | 32 |

---

## Project Structure

```
parking-spot/
├── app.py            # Flask backend / REST API
├── arduino.cpp       # ESP32 firmware (upload via Arduino IDE)
├── index.html        # Web dashboard frontend
├── requirements.txt  # Python dependencies
└── vercel.json       # Vercel deployment config
```

---

## Setup

### 1. Clone the repo

```bash
git clone https://github.com/Static0l0/parking-spot.git
cd parking-spot
```

### 2. Backend (Flask server)

Install dependencies:

```bash
pip install -r requirements.txt
```

Run locally:

```bash
python app.py
```

The server starts on `http://localhost:3000`. The dashboard is served at `/` and the API is at `/api/status`.

> **Note:** The server uses in-memory storage — parking states reset on restart. For persistent state, consider deploying on Railway or Render instead of Vercel.

### 3. Deploy to Vercel (optional)

```bash
npm i -g vercel
vercel
```

The included `vercel.json` handles routing automatically.

### 4. Arduino firmware

1. Open `arduino.cpp` in the Arduino IDE (rename to `.ino` if needed, or use PlatformIO).
2. Update the WiFi credentials and server URL at the top of the file:

```cpp
const char* ssid     = "YourWiFiName";
const char* password = "YourWiFiPassword";
String serverName    = "https://your-deployment-url/api/parking";
```

3. Select your ESP32 board and the correct COM port.
4. Upload the sketch.

---

## API Reference

### `GET /api/parking`

Called by the ESP32 to report a sensor update.

| Query Param | Values | Description |
|-------------|--------|-------------|
| `sensor` | `1` or `2` (or `left`/`right`) | Which parking spot |
| `status` | `PARK` or `nothing` (or `1`/`0`) | Detected status |

Example:
```
GET /api/parking?sensor=1&status=PARK
```

### `GET /api/status`

Returns the current state of both spots as JSON.

```json
{
  "left": "1",
  "right": "0"
}
```

`"1"` = occupied, `"0"` = free.

---

## Notes

- The detection threshold is **5 cm** — objects closer than that are considered parked. Adjust in `arduino.cpp` if needed.
- The ESP32 only sends an HTTP request when the status *changes*, so it won't spam the server.
- Vercel cold starts will reset the in-memory parking state. For a production setup, add a database or use a always-on host.
