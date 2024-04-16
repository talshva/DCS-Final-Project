![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/a5927da7-4cda-444c-92bb-2f2e96de3e99)# DCS Final Project - Object and Light Source Detection System

## Project Overview

This project focuses on implementing a system capable of detecting light sources and monitoring objects in space using the MSP430G2553 microcontroller. It integrates a blend of sensors and actuators managed by a microcontroller to perform precise detection and location measurements.

This project is developed primarily in C, with some parts implemented in assembly language. It employs a layered architecture approach including Board Support Package (BSP), Hardware Abstraction Layer (HAL), Application Programming Interface (API), and the main application logic to ensure modularity and maintainability.

For detailed information, check my pdf.

### System Features:
- **Timers and ADC**: Utilized for precise timing and analog-to-digital conversions.
- **Interrupts and DMA**: Enhance real-time processing capabilities.
- **Flash Memory**: Used for storing scripts and calibration data, handled through the system's flash utilities.

## Project Description

### Components Used:
1. **Ultrasonic Sensor**: Detects objects by emitting sound waves and measuring the echoes.
2. **LDR Sensors**: Measure light intensity and convert it to voltage to determine proximity to light sources.
3. **Servo Motor**: Adjusts the angle of sensors to optimize detection accuracy.

### System Operations and Setup
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/700fda15-fc2f-44ab-a298-bf8898e7cb47)
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/711bfcda-9ad4-40a1-ad6e-5b2c0b92bf94)

### Object Detection
- Conducts a 180-degree scan to identify objects in space based on predefined parameters.
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/40787dae-65e1-4c17-a6c7-3992bf56fa34)
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/bd153117-9a64-4fe3-9d23-b04fbb9c1e89)

### Telemeter - Dynamic Scanning Interface
- Allows for angle-specific distance measurements, providing real-time data output on object locations.
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/baf53ac6-4726-41ce-80ab-577b79146261)
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/12bb6d12-80b9-4777-8e4d-80405d5d6d7f)

### Light Source Detection
- Identifies light sources within a 180-degree scan. 
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/edeb1809-076b-4046-86cb-157117f492ec)
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/2a8f3f0b-8464-4606-9bba-366e35286ba1)

### Calibration Process:
- Involves linear interpolation based on multiple samples to accurately calibrate sensor readings, adjust to varying environmental conditions.
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/89062fac-5eb9-43ed-911a-34a5cf91a196)
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/f299bc63-daad-47dd-9ec8-6ac49e8b3ff3)

### Integrated Detection of Light Sources and Objects (Bonus)
- Merges the functionalities of detecting both objects and light sources, producing a combined output of detected entities.
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/68ef671d-d2bf-495a-92e2-ffcfcb683390)
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/2923f1b6-a5e9-4099-a43f-d5f6b94abe7a)
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/cd11d01f-b6cf-4ad4-a0b9-8f53327985ae)

### Script Mode
- Supports converting scripts written in a High-Level Language (HLL) to machine language (Hex), transmitting them from the computer to the controller, storing them in flash memory, and executing them independently. Each script contains up to 10 commands that trigger various controller functions, such as counting up and down on an LCD and scanning objects between two angles defined by the user.
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/fea9428a-c1c0-48b5-8cfd-2b047302e365)
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/7b3a80c3-0110-413e-8b97-aaa2ecd676ad)

## Work Division Between Hardware and Software
The guiding principle for dividing the work is to allow the controller to operate actively and independently from the computer side, which is designed more passively for user interface. Due to the lack of specific units like a Floating Point Unit in the controller and potential bottlenecks in communication between the controller and the computer, core computations are performed in the controller, while more complex calculations, such as binary searches and interpolation, are handled on the computer side.

## Workflow and FSM:
![image](https://github.com/talshva/DCS-Final-Project/assets/82408347/f60fe36c-5595-43ee-a600-ef14669b9c35)

This Finite State Machine (FSM) diagram visually represents the logic flow and operational states of a multi-functional detection system. It outlines the navigation pathways between various modes such as Object Detection, Telemeter, Light Detection, combined Lights & Objects Detection, and Script Mode. The transitions between states are triggered by user inputs or process completions, with additional functionalities like sleep, calibration, and script operations (upload, play, modify LCD display, and set delays) available for comprehensive system control.

## Project limitations and future improvements:
The project's execution, when compared with the technical specifications, highlighted key areas for improvement: optimizing the limited flash memory capacity to better accommodate multiple scripts, enhancing the LDR sensors' sensitivity to prevent false light detection due to indirect light and environmental noise, and configuring the MSP430G2553's two available timers for parallel tasks which required dynamic reconfiguration during runtime to meet the system's operational needs.

## Team Members
- Tal Shvartzberg
- Roi Osman
