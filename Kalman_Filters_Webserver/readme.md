# Kalman Filters Webserver

This project implements a Kalman Filter to estimate the true value of a noisy signal. It includes a web server that visualizes the true, noisy, and filtered values in real-time using Plotly.

## Features

- Real-time data visualization using WebSockets and Plotly.
- Kalman Filter implementation for noise reduction.
- Web interface to display the results.

## Code Explanation

### Kalman Filter Class

The `KalmanFilter` class is used to filter noisy measurements and estimate the true value.

```cpp
class KalmanFilter {
private:
    float Q = 0.1;  // Process noise
    float R = 1.0;  // Measurement noise
    float P = 1.0;  // Estimation error
    float K = 0.0;  // Kalman gain
    float X = 0.0;  // State estimate

public:
    float update(float measurement) {
        P = P + Q;
        K = P / (P + R);
        X = X + K * (measurement - X);
        P = (1 - K) * P;
        return X;
    }

    void setParameters(float process_noise, float measurement_noise) {
        Q = process_noise;
        R = measurement_noise;
    }
};
```

### WiFi and Web Server Setup

The code sets up the WiFi connection and initializes the web server and WebSocket.

```cpp
const char* ssid = "Testwifi";
const char* password = "x11y22z33";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
KalmanFilter kalmanFilter;
```

### HTML Content

The HTML content for the web interface is stored as a string in program memory.

```cpp
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Kalman Filter Visualization</title>
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
    <style>
        #chart {
            width: 100%;
            height: 600px;
        }
    </style>
</head>
<body>
    <div id="chart"></div>
    <script>
        const maxDataPoints = 100;
        let trueData = Array(maxDataPoints).fill(0);
        let noisyData = Array(maxDataPoints).fill(0);
        let filteredData = Array(maxDataPoints).fill(0);
        
        const layout = {
            title: 'Kalman Filter Visualization',
            xaxis: { title: 'Sample' },
            yaxis: { title: 'Value' }
        };
        
        const trace1 = {
            y: trueData,
            name: 'True Value',
            type: 'scatter'
        };
        
        const trace2 = {
            y: noisyData,
            name: 'Noisy Measurement',
            type: 'scatter'
        };
        
        const trace3 = {
            y: filteredData,
            name: 'Filtered Value',
            type: 'scatter'
        };
        
        Plotly.newPlot('chart', [trace1, trace2, trace3], layout);
        
        const ws = new WebSocket('ws://' + window.location.hostname + '/ws');
        
        ws.onmessage = function(event) {
            const data = JSON.parse(event.data);
            
            trueData.push(data.true);
            noisyData.push(data.noisy);
            filteredData.push(data.filtered);
            
            if(trueData.length > maxDataPoints) {
                trueData.shift();
                noisyData.shift();
                filteredData.shift();
            }
            
            Plotly.update('chart', {
                y: [trueData, noisyData, filteredData]
            });
        };
    </script>
</body>
</html>
)rawliteral";
```

### WebSocket Event Handling

The `onWsEvent` function handles WebSocket events such as client connections and disconnections.

```cpp
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if(type == WS_EVT_CONNECT){
        Serial.println("WebSocket client connected");
    } else if(type == WS_EVT_DISCONNECT){
        Serial.println("WebSocket client disconnected");
    }
}
```

### Setup Function

The `setup` function initializes the serial communication, LittleFS, WiFi connection, WebSocket, and web server.

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize LittleFS
    if(!LittleFS.begin(true)) {
        Serial.println("LittleFS Mount Failed");
        return;
    }

    // Initialize Kalman Filter
    kalmanFilter.setParameters(0.1, 1.0);
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Setup WebSocket
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    
    // Serve web page from program memory
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", index_html);
    });
    
    server.begin();
}
```

### Loop Function

The `loop` function generates sample data, applies the Kalman filter, and sends the data to connected WebSocket clients.

```cpp
void loop() {
    static unsigned long lastUpdate = 0;
    const unsigned long UPDATE_INTERVAL = 50; // 50ms update interval
    
    if (millis() - lastUpdate >= UPDATE_INTERVAL) {
        lastUpdate = millis();
        
        // Generate sample data
        float true_value = 10 * sin(millis() / 1000.0);
        float noisy_measurement = true_value + random(-100, 100) / 100.0;
        float filtered_value = kalmanFilter.update(noisy_measurement);
        
        // Create JSON string
        String jsonString = "{\"true\":" + String(true_value, 3) + 
                           ",\"noisy\":" + String(noisy_measurement, 3) + 
                           ",\"filtered\":" + String(filtered_value, 3) + "}";
                           
        // Send to all connected clients
        ws.textAll(jsonString);
        
        // Also output to Serial for debugging
        Serial.printf("True: %.3f, Noisy: %.3f, Filtered: %.3f\n", 
                     true_value, noisy_measurement, filtered_value);
    }
}
```

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/)
- [Arduino Framework](https://www.arduino.cc/)
- ESP32 development board

### Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/Control_Systems.git
    cd Control_Systems
    ```

2. Open the project in [PlatformIO](https://platformio.org/).

3. Build and upload the project to your ESP32 board.

### Usage

1. Connect your ESP32 board to your computer.
2. Open the Serial Monitor to view the output.
3. Connect to the ESP32's WiFi network and open the provided IP address in a web browser to view the real-time visualization.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [PlatformIO](https://platformio.org/)
- [Arduino](https://www.arduino.cc/)
- [Plotly](https://plotly.com/)

Feel free to contribute to this repository by submitting issues or pull requests.
