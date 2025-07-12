![Status](https://img.shields.io/badge/Status-Active%20%26%20Updating-brightgreen) 
![Stars](https://img.shields.io/github/stars/jeronimoagullo/zephyr-lvgl-aiot-showcase?style=social) 
![Contributions Welcome](https://img.shields.io/badge/Contributions-Welcome-blue)  

This project is a comprehensive demonstration of various capabilities available within the Zephyr RTOS ecosystem. It showcases a rich user interface built with LVGL, real-time sensor data visualization, machine learning inference with TensorFlow Lite, and IoT connectivity via MQTT.

![Project demo main screen](images/display_image_01.jpg)

# ✨ Features
The demo uses the `LVGL` library to create different tabs providing the following features:
- **Boot-up Window:** A splash screen shown on device startup.
- **LVGL User Interface:** A responsive, tab-based UI for navigating different features.
- **Real-time Data Graphing:** Visualizes Inertial Measurement Unit (IMU) sensor data on a live chart.
- **IoT Connectivity:** 
  - **Network:** Establishes a network connection (Ethernet).
  - **MQTT:** Sends and receives data (e.g., from UI sliders) to and from an MQTT broker.
- **Machine Learning:** Performs gesture recognition using a TensorFlow Lite model that takes IMU data as input.
- **Device Configuration:** A dedicated tab to display system and network configuration.

# 🎯 Setup Zephyr Environment
First, ensure you have a working Zephyr development environment. If you don't, please follow the official [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

- **Zephyr version:** 3.7.1 (latest LTS version)
- **Zephyr SDK:** v0.16.8

Make sure you are using the correct Zephyr version for this project:

```shell
# Navigate to your Zephyr installation
cd zephyrproject

# Checkout the correct branch and update dependencies
git switch v3.7-branch
west update
```

- **hash commit:** c291ed46f915cb72aef28479c709356f18f37462 (24 January 2025)
- **Tested boards:**
  - stm32f746g_disco

## 🛠️ Build
```
# From the project's root directory
west build -b stm32f746g_disco
```

## ⚡️ Flash
```
west flash
```

# 🔧 Project Structure

```
display-poc-1_jero/
├── CMakeLists.txt      # Main CMake build script
├── prj.conf            # Kconfig project configuration
├── README.md           # This file
└── src/
    ├── main.c              # Main application entry point and thread setup
    ├── config.h            # Project-wide configuration (MQTT, etc.)
    ├── config_network.c    # Network initialization logic
    ├── config_network.h
    └── jeroagullo_lvgl/    # LVGL UI source files
        ├── jeroagullo_tabs.c   # UI tabs implementation
        ├── jeroagullo_charts.c # Charting logic for sensor data
        ├── jeroagullo_styles.c # Custom LVGL styles
        └── ...
```

# 📅 TODO list
- [x] Add basic interface in LVGL with styles
- [x] disable/enable the threads when a tab is pressed
- [x] Add mutex to all lvgl functions to synchronize with lv_task_handler() call
- [x] Add Network connection (tested with ethernet)
- [ ] Add sliders to corresponding windows
- [ ] Add mqtt to send the sliders values
- [ ] Add IMU sensor
- [ ] Add TensorFlow Lite model


# 🌟 How to Contribute
Star the repo if it helps you! ⭐

Open an Issue for questions or suggestions. Submit a Pull Request to add new features.