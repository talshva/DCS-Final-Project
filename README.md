# DCS Final Project - Object and Light Source Detection System

## Project Overview

This project focuses on implementing a system capable of detecting light sources and monitoring objects in space using the MSP430G2553 microcontroller. It integrates a blend of sensors and actuators managed by a microcontroller to perform precise detection and location measurements.
This project is developed primarily in C, with some parts implemented in assembly language. It employs a layered architecture approach including Board Support Package (BSP), Hardware Abstraction Layer (HAL), Application Programming Interface (API), and the main application logic to ensure modularity and maintainability.

## Team Members
- Tal Shvartzberg
- Roi Osman

### System Features:
- **Timers and ADC**: Utilized for precise timing and analog-to-digital conversions.
- **Interrupts and DMA**: Enhance real-time processing capabilities.
- **Flash Memory**: Used for storing scripts and calibration data, handled through the system's flash utilities.

## Project Description

### Components Used:
1. **Ultrasonic Sensor**: Detects objects by emitting sound waves and measuring the echoes.
2. **LDR Sensors**: Measure light intensity and convert it to voltage to determine proximity to light sources.
3. **Servo Motor**: Adjusts the angle of sensors to optimize detection accuracy.

### Image 1: System Setup
Placeholder for the final working environment setup image.

## System Operations

### Object Detection
- Conducts a 180-degree scan to identify objects in space based on predefined parameters.

#### Image 2: Object Detection Mode
Placeholder for the sub-menu for object detection during a scan.

### Telemeter
- Allows for angle-specific distance measurements, providing real-time data output on object locations.

#### Image 3: Dynamic Scanning Interface
Placeholder for the telemeter operation interface showing dynamic distance measurement.

### Light Source Detection
- Identifies light sources within a 180-degree scan, integrating calibration to adjust to varying environmental conditions.

### Integrated Detection of Light Sources and Objects (Bonus)
- Merges the functionalities of detecting both objects and light sources, producing a combined output of detected entities.

## Calibration and Setup

### Calibration Process:
- Involves linear interpolation based on multiple samples to accurately calibrate sensor readings.

#### Image 4: Calibration Menu
Placeholder for the calibration menu after sampling.
