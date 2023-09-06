Functions Description:

objects_detector(): This function detects objects using sensor data and performs relevant actions based on the detected objects.

telemeter(): It measures distances using a telemeter sensor and returns the measured distance.

lights_detector(): This function detects the presence of lights using light sensors and returns the status of the detected lights.

light_objects_detector(): It combines the functionality of both object detection and light detection to identify and react to specific objects with lights.

script_mode(): This function handles the execution of a script mode, which involves executing a predefined sequence of actions or commands.

send_angle(angle): It sends an angle value over the UART connection to the MSP430G2553 microcontroller.

init_uart(): This function initializes the UART connection between the Python PC-side server and the MSP430G2553 microcontroller.

send_data(data_str): It sends a string of data over the UART connection to the microcontroller.

send_command(char): This function sends a single character command over the UART connection to the microcontroller.

command_encoder(data_str): It encodes a string of data into a format suitable for transmission over UART.

receive_ack(): This function receives an acknowledgment signal from the microcontroller over the UART connection.

receive_data(): It receives data from the microcontroller over the UART connection.

receive_data2(): This function receives a second set of data from the microcontroller over the UART connection.

receive_char(): It receives a single character from the microcontroller over the UART connection.

receive_calib(): This function receives calibration data from the microcontroller over the UART connection.

save_calibration_values(calibration_values): It saves the calibration values received from the microcontroller.

expand_calibration_array(calibration_array, new_length): This function expands the calibration array to accommodate new calibration values.

measure_two_ldr_samples(): It measures samples from two light-dependent resistors (LDRs) and returns the measured values.

find_fitting_index(ldr1_value, ldr2_value): This function finds the index in the calibration array that best fits the given LDR values.

draw_scanner_map(distances, angles): It visualizes a scanner map based on the measured distances and angles.

draw_scanner_map_lights(distances, lights, angles): This function visualizes a scanner map with additional information about detected lights.

main(): This is the main function that serves as the entry point of the program and coordinates the execution of other functions.