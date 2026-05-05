import os
from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS

# Resolve directory where this file lives (works both locally and on Vercel)
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

app = Flask(__name__)
CORS(app)

# In-memory store — NOTE: resets on Vercel cold starts.
# For production use Railway/Render instead of Vercel.
parking_states = {
    "left": "0",
    "right": "0"
}

@app.route('/')
def index():
    return send_from_directory(BASE_DIR, 'index.html')

@app.route('/<path:filename>')
def serve_files(filename):
    return send_from_directory(BASE_DIR, filename)

@app.route('/api/parking', methods=['GET'])
def parking():
    sensor = request.args.get('sensor')  # 'left'/'right'  OR  '1'/'2'
    status = request.args.get('status')  # '1'/'0'         OR  'PARK'/'nothing'

    # Normalize sensor: numeric IDs → named sides
    sensor_map = {'1': 'left', '2': 'right'}
    sensor = sensor_map.get(sensor, sensor)

    # Normalize status: string labels → binary
    status_map = {'PARK': '1', 'park': '1', 'nothing': '0', 'NOTHING': '0'}
    status = status_map.get(status, status)

    if sensor in parking_states:
        parking_states[sensor] = status
        label = 'OCCUPIED' if status == '1' else 'FREE'
        print(f"Update - Sensor {sensor}: {label}")

    return "speGETti is good", 200

@app.route('/api/status', methods=['GET'])
def get_status():
    return jsonify(parking_states)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=3000, debug=True)
