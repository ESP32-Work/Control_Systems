# Control_Systems

This repository contains multiple projects related to control systems with visualizations. Each project demonstrates different aspects of control systems, including filtering, estimation, and real-time data visualization.

## Projects

### 1. Kalman Filters Webserver

This project implements a Kalman Filter to estimate the true value of a noisy signal. It includes a web server that visualizes the true, noisy, and filtered values in real-time using Plotly.

- **Directory:** `Kalman_Filters_Webserver`
- **Features:**
  - Real-time data visualization using WebSockets and Plotly.
  - Kalman Filter implementation for noise reduction.
  - Web interface to display the results.


A simple plotter to visualize sensor data. This project reads sensor data, applies a Kalman Filter, and outputs the results to the Serial Plotter.

- **Directory:** `Kalman_Filters_Webserver/src/simple_plotter.txt`
- **Features:**
  - Serial output for real-time data visualization.
  - Kalman Filter implementation for noise reduction.




## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/)
- [Arduino Framework](https://www.arduino.cc/)
- ESP32 development board

### Installation

1. Clone the repository:
    ```sh
    git clone https://github.com/ibrahimmansur4/Control_Systems.git
    cd Control_Systems
    ```

2. Open the project in [PlatformIO](https://platformio.org/).

3. Build and upload the project to your ESP32 board.

### Usage

1. Connect your ESP32 board to your computer.
2. Open the Serial Monitor to view the output.
3. For the Kalman Filters Webserver project, connect to the ESP32's WiFi network and open the provided IP address in a web browser to view the real-time visualization.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [PlatformIO](https://platformio.org/)
- [Arduino](https://www.arduino.cc/)
- [Plotly](https://plotly.com/)

Feel free to contribute to this repository by submitting issues or pull requests.