#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <AsyncWebSocket.h>

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

const char* ssid = "Testwifi";
const char* password = "x11y22z33";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
KalmanFilter kalmanFilter;

// HTML content stored as a string since it's small
// For larger files, store in LittleFS
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

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if(type == WS_EVT_CONNECT){
        Serial.println("WebSocket client connected");
    } else if(type == WS_EVT_DISCONNECT){
        Serial.println("WebSocket client disconnected");
    }
}

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