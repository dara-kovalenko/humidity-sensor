#include <Arduino.h>

static const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html>
<head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width,initial-scale=1" />
    <title>ESP32 Temperature Monitor</title>

    <style>
        body {
            font-family: system-ui;
            margin: 0;
            padding: 16px;
            background: #111;
            color: #eee;
        }
        .wrap {
            max-width: 520px;
            margin: 0 auto;
            text-align: center;
        }
        h1 {
            font-size: 22px;
            margin-bottom: 20px;
        }
        .card {
            background: #222;
            padding: 20px;
            border-radius: 14px;
            margin-bottom: 16px;
        }
        .value {
            font-size: 32px;
            font-weight: bold;
        }
        .label {
            opacity: 0.8;
            font-size: 14px;
        }
    </style>
</head>

<body>
    <div class="wrap">
        <h1>🌡️ ESP32 Sensor Data</h1>

        <div class="card">
            <div class="value" id="temp">-- °C</div>
            <div class="label">Temperature</div>
        </div>

        <div class="card">
            <div class="value" id="hum">-- %</div>
            <div class="label">Humidity</div>
        </div>
    </div>

<script>
async function updateSensor() {
    try {
        const res = await fetch("/sensor");
        const data = await res.json();

        document.getElementById("temp").textContent =
            data.temperature.toFixed(1) + " °C";

        document.getElementById("hum").textContent =
            data.humidity.toFixed(1) + " %";
    } catch (e) {
        console.log("Sensor read failed");
    }
}

setInterval(updateSensor, 3000);
updateSensor();
</script>

</body>
</html>
)HTML";
