# Adaptive Cruise Control System

## Overview

This project implements an adaptive cruise control system for vehicles. The system maintains a constant vehicle speed and adjusts the speed based on real-time sensor data to ensure safe following distances. The system utilizes Hall Effect sensors for distance measurement, IR sensors for obstacle detection, and integrates with cloud services (ThingSpeak) for distance logging when the cruise control is disengaged.

## Hardware Components

- **LPC2148 Microcontroller**: Main controller of the system.
- **Hall Effect Sensor**: Used to measure the vehicle's wheel rotations for calculating the distance traveled.
- **IR Sensors**: Used to detect obstacles in front of the vehicle and adjust the cruise control speed accordingly.
- **ESP8266**: Provides Wi-Fi connectivity for cloud integration.
- **LCD (16x2)**: Displays real-time distance traveled and sensor data.
- **CC2500**: Wireless communication module for remote control and monitoring.
- **Buzzer**: Alerts the driver in case of any obstacles or critical conditions.

## Model

<div style="display: flex; justify-content: space-between;">
  <img src="https://github.com/user-attachments/assets/3ebbf005-f449-4e89-b1e3-5a534347fc20" width="48%" />
  <img src="https://github.com/user-attachments/assets/3b70a224-c6e2-4cc2-a265-dec3e0570349" width="48%" />
</div>

## Cloud Dashboard

<div style="display: flex; justify-content: space-between;">
  <img src="https://github.com/user-attachments/assets/ca284875-acdc-4054-a0de-0706b814ca53" width="48%" />
  <img src="https://github.com/user-attachments/assets/fd148bae-8a61-41c3-8111-e08d49f7ead3" width="48%" />
</div>

## Software

- **Keil**: IDE used for LPC2148 development.
- **Arduino IDE**: Used for cloud integration with ESP8266 and ThingSpeak.
- **UART, ADC/DAC**: Used for communication and sensor data processing.

## Key Features

- **Adaptive Speed Control**: Adjusts motor speed based on real-time sensor data to maintain a safe following distance.
- **Distance Logging**: Distance traveled is logged to the cloud (ThingSpeak) upon disengagement.
- **Obstacle Detection**: IR sensors trigger motor speed reduction when obstacles are detected.
- **Motor Speed Control**: Controlled using ADC/DAC for smooth operation.
- **LCD Display**: Displays distance traveled and real-time data for easy monitoring.
- **Wireless Communication**: Sends data to the cloud using the ESP8266 module.

## Challenges

- **Precise Sensor Integration**: Ensuring accurate data from Hall Effect and IR sensors for reliable speed and distance control.
- **Wireless Communication**: Ensuring stable data transmission via the ESP8266 for cloud integration.
- **Cloud Data Management**: Efficiently logging and retrieving data from ThingSpeak for monitoring and analysis.

## Installation

1. Clone this repository to your local machine: https://github.com/P-Shreyas543/Cruise-Control-System
2. Set up the hardware components as per the wiring diagram provided in the repository.
3. Open the project in Keil IDE for LPC2148 development and compile the code.
4. Use Arduino IDE to set up and configure the ESP8266 for cloud communication.
5. Upload the code to the LPC2148 microcontroller and the ESP8266.

## Usage

- After uploading the code, the system will start automatically.
- The LCD will display the distance traveled in real-time.
- The system will adjust motor speed based on the sensor inputs.
- When the cruise control is disengaged, the distance will be logged to the ThingSpeak cloud platform.

## Cloud Integration

- The system uses ThingSpeak to log distance data.
- You can view the logged data on the ThingSpeak dashboard to monitor the system's performance and distance traveled.

## Future Improvements

- Integration with GPS for more advanced navigation and speed control.
- Enhanced obstacle detection using more IR sensors or ultrasonic sensors.
- Integration with mobile applications for real-time monitoring and control.

## Contribution

Feel free to contribute to this project! Whether it's fixing bugs, suggesting features, or improving the documentation, your help is appreciated.

To contribute:
1. Fork the repository.
2. Create a new branch for your changes.
3. Make your changes and commit them.
4. Submit a pull request with a detailed description of your changes.

