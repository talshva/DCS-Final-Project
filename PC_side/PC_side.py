import PySimpleGUI as sg
import time
import serial as ser
import os
import matplotlib.pyplot as plt
import matplotlib.cm as cm
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import math
import numpy as np

light_epsilon = 0.3
object_light_epsilon = 0.3


def objects_detector():
    layout = [
        [sg.Text("Enter Masking Distance [cm]:", text_color='#000000', background_color='#9AF1FF',
                 font=('Segoe UI', 10)),
         sg.InputText(key="-DISTANCE-", size=(20, 1))],
        [sg.Button("Start Objects Scan", key="-SCAN-", button_color='#000000'),
         sg.Button("Back", key="-BACK-", button_color='#000000')],
        [sg.Output(key="-OUTPUT-", size=(45, 5))]
    ]
    window = sg.Window("Objects Detector System", layout, background_color='#9AF1FF', font=('Segoe UI', 10),
                       resizable=True, return_keyboard_events=True, finalize=True)
    window.bind("<Escape>", "-ESCAPE-")

    while True:
        event, values = window.read()
        if event in (sg.WINDOW_CLOSED, "-BACK-", "-ESCAPE-"):
            send_command('0')
            break
        elif event == "-SCAN-" or event == '\r':
            window["-SCAN-"].update(disabled=True)
            window["-BACK-"].update(disabled=True)
            distance_arr = []
            masking_distance = values["-DISTANCE-"]
            send_command('S')
            angle1 = int(receive_data())
            angle2 = int(receive_data())
            counter = 0
            while True:
                window.refresh()
                distance = int(receive_data())
                if distance == 999:
                    break
                if counter > 4:  # Servo stuck at around first 12 samples
                    if distance < int(masking_distance):
                        distance_arr.append(distance)
                        window['-OUTPUT-'].update(f"Distance: {'{:>3}'.format(distance)} [cm]\n", append=True)
                    else:
                        window['-OUTPUT-'].update(f"Distance: {'{:>3}'.format(distance)} [cm]", append=True)
                        window['-OUTPUT-'].Widget.tag_configure("red_text", foreground="red")
                        window['-OUTPUT-'].Widget.insert("end", " - MASKED\n", "red_text")
                        distance_arr.append(0)
                counter += 1

            degree_arr = [round(float(5) + i * (float(180) - float(0)) / (len(distance_arr) - 1), 1)
                          for i in range(len(distance_arr))]
            window['-OUTPUT-'].update(f"Distance array: {distance_arr}\n", append=True)
            window['-OUTPUT-'].update(f"Degree array: {degree_arr}\n", append=True)
            draw_scanner_map(distance_arr, degree_arr)
            window["-SCAN-"].update(disabled=False)
            window["-BACK-"].update(disabled=False)

    window.close()


def telemeter():
    #  angle = 0
    dynamic_flag = 0

    layout = [
        [sg.Text("Angle [0°-180°]:", text_color='#000000', background_color='#9AF1FF', font=('Segoe UI', 10)),
         sg.InputText(key="-ANGLE-", size=(20, 1))],
        [sg.Button("Start Measure", key="-START-", button_color='#000000'),
         sg.Button("Stop Measure", key="-STOP-", button_color='#000000'),
         sg.Button("Back", key="-BACK-", button_color='#000000')],
        [sg.Output(key="-OUTPUT-", size=(30, 2))]
    ]

    window = sg.Window("Telemeter", layout, background_color='#9AF1FF', font=('Segoe UI', 10), resizable=True,
                       return_keyboard_events=True, finalize=True)
    window.bind("<Escape>", "-ESCAPE-")

    while True:
        window.refresh()
        event, values = window.read(timeout=100)
        if event in (sg.WINDOW_CLOSED, "-BACK-", "-ESCAPE-"):
            send_command('0')
            break
        elif event == "-START-" or event == '\r':

            window["-START-"].update(disabled=True)
            window["-BACK-"].update(disabled=True)
            window["-STOP-"].update(disabled=False)
            print("Performing Ultrasonic Scan")
            window.refresh()
            send_command('T')
            angle = int(values["-ANGLE-"])
            dynamic_flag = 1
            send_angle(angle)

        elif event == "-STOP-":
            window["-STOP-"].update(disabled=True)
            window["-START-"].update(disabled=False)
            window["-BACK-"].update(disabled=False)
            window.refresh()
            send_command('M')
            dynamic_flag = 0

        elif event == "__TIMEOUT__":
            if dynamic_flag:
                distance = int(receive_data())
                angle = int(receive_data())
                window['-OUTPUT-'].update(f"Distance: {'{:>3}'.format(distance)} [cm] ", append=True)
                window['-OUTPUT-'].update(f"| Angle: {'{:>3}'.format(angle)} [°]\n", append=True)
    window.close()


def lights_detector():
    layout = [
        [sg.Button("Start Light Sources Scan", key="-SCAN-", size=(40, 1), button_color='#000000'),
         sg.Button("Calibrate Using PB0", key="-CALIBRATE-", button_color='#000000'),
         sg.Button("Back", key="-BACK-", button_color='#000000')],
        [sg.ProgressBar(10, orientation='h', expand_x=True, size=(20, 20), bar_color=('#FFFFFF', '#9AF1FF'),
                        key='-PBAR-')],
        [sg.Text(' ', key='-OUT-', enable_events=True, text_color='#000000', background_color='#9AF1FF',
                 font=('Segoe UI', 10), justification='center', expand_x=True)],
        [sg.Output(key="-OUTPUT-", size=(80, 3))]

    ]
    window = sg.Window("Light Sources Detector System", layout, background_color='#9AF1FF', font=('Segoe UI', 10),
                       resizable=True, return_keyboard_events=True, finalize=True)
    window.bind("<Escape>", "-ESCAPE-")

    while True:
        event, values = window.read()

        if event in (sg.WINDOW_CLOSED, "-BACK-", "-ESCAPE-"):
            send_command('0')
            break
        elif event == "-CALIBRATE-":
            LDR_calibrate_arr = []
            window["-BACK-"].update(disabled=True)
            window["-SCAN-"].update(disabled=True)
            window["-CALIBRATE-"].update(disabled=True)
            window["-CALIBRATE-"].update("Press PB0 to calibrate")
            window['-PBAR-'].update(current_count=0)
            window['-OUT-'].update("")
            send_command('J')

            for i in range(10):
                window.refresh()
                window['-OUTPUT-'].update(f"Please Press Push Button 0 to take a sample: {i + 1}\n", append=True)
                LDR1_val = int(receive_data()) / 292
                LDR2_val = int(receive_data()) / 292
                LDR1_val_trunc = "%.2f" % LDR1_val
                LDR2_val_trunc = "%.2f" % LDR2_val
                LDRavg_val_trunc = "%.2f" % ((LDR1_val + LDR2_val) / 2)

                window['-OUTPUT-'].update(
                    f"Left LDR value: {LDR1_val_trunc} [V]| Right LDR value: {LDR2_val_trunc} [V]\n", append=True)
                window['-OUTPUT-'].update(f"Average value: {LDRavg_val_trunc} [V]\n", append=True)

                LDR_calibrate_arr.append((LDR1_val + LDR2_val) / 2)

                window['-PBAR-'].update(current_count=i + 1)
                window['-OUT-'].update(f"sample {i + 1} received")
                time.sleep(0.05)

            window["-BACK-"].update(disabled=False)
            window["-SCAN-"].update(disabled=False)
            window["-CALIBRATE-"].update("Done Calibrating!")
            window['-PBAR-'].update(current_count=0)

            expanded_LDR_calibrate_arr = expand_calibration_array(LDR_calibrate_arr, 50)
            save_calibration_values(expanded_LDR_calibrate_arr)

        elif event == "-SCAN-" or event == '\r':
            window["-BACK-"].update(disabled=True)
            window["-SCAN-"].update(disabled=True)
            window["-CALIBRATE-"].update(disabled=True)
            distance_arr = []
            masking_distance = 49
            send_command('K')
            angle1 = int(receive_data())
            angle2 = int(receive_data())
            counter = 0
            flag = 0
            while True:
                window.refresh()
                arr = measure_two_ldr_samples()
                if counter > 8:  # Servo stuck at around first 12 samples
                    light_distance = arr[0] + 1
                    ldr_val1 = arr[1]
                    ldr_val2 = arr[2]
                    if light_distance == 0:
                        break

                    LDR1_val_trunc = "%.2f" % ldr_val1
                    LDR2_val_trunc = "%.2f" % ldr_val2
                    window['-OUTPUT-'].update(
                        f"Left LDR value: {LDR1_val_trunc} [V] | Right LDR value: {LDR2_val_trunc} [V]", append=True)

                    window['-OUTPUT-'].update(f" | Estimate Distance: {light_distance} [cm]", append=True)
                    if abs(ldr_val1 - ldr_val2) < light_epsilon and ldr_val1 < 3 and ldr_val2 < 3:
                        if light_distance > int(masking_distance):
                            window['-OUTPUT-'].Widget.tag_configure("red_text", foreground="red")
                            window['-OUTPUT-'].Widget.insert("end", " (MASKED) \n", "red_text")
                            distance_arr.append(0)
                            flag = 0
                        else:

                            window['-OUTPUT-'].Widget.tag_configure("green_text", foreground="green")
                            window['-OUTPUT-'].Widget.insert("end", " - LIGHT DETECTED \n", "green_text")
                            if flag == 1:
                                distance_arr.append(light_distance)
                            else:
                                distance_arr.append(0)
                            flag = 1
                    else:
                        flag = 0
                        window['-OUTPUT-'].Widget.tag_configure("red_text", foreground="red")
                        window['-OUTPUT-'].Widget.insert("end", " (NOISE) \n", "red_text")
                        distance_arr.append(0)
                counter += 1

            degree_arr = [round(float(5) + i * (float(180) - float(0)) / (len(distance_arr) - 1), 1)
                          for i in range(len(distance_arr))]

            window['-OUTPUT-'].update(f"Distance array: {distance_arr}\n", append=True)
            window['-OUTPUT-'].update(f"Degree array: {degree_arr}\n", append=True)

            draw_scanner_map_lights(distance_arr, distance_arr, degree_arr)
            window["-BACK-"].update(disabled=False)
            window["-SCAN-"].update(disabled=False)
            window["-CALIBRATE-"].update(disabled=False)

    window.close()


def light_objects_detector():
    layout = [
        [sg.Text("Masking Distance [cm]:", text_color='#000000', background_color='#9AF1FF', font=('Segoe UI', 10)),
         sg.InputText(key="-DISTANCE-", size=(20, 1))],
        [sg.Text("Masking range for lights is 0.5 meter", text_color='#000000', background_color='#9AF1FF',
                 font=('Segoe UI', 10))],
        [sg.Button("Start Lights & Object Scan", key="-LIGHT-OBJECT_DETECT-", size=(45, 1), button_color='#000000'),
         sg.Button("Back", key="-BACK-", button_color='#000000')],
        [sg.Output(key="-OUTPUT-", size=(80, 3))]
    ]

    window = sg.Window("Light Sources and Objects Detector System", layout, background_color='#9AF1FF',
                       font=('Segoe UI', 10), resizable=True, return_keyboard_events=True, finalize=True)
    window.bind("<Escape>", "-ESCAPE-")

    while True:
        event, values = window.read()
        if event in (sg.WINDOW_CLOSED, "-BACK-", "-ESCAPE-"):
            send_command('0')
            break
        elif event == "-LIGHT-OBJECT_DETECT-" or event == '\r':
            window["-LIGHT-OBJECT_DETECT-"].update(disabled=True)
            window["-BACK-"].update(disabled=True)
            distance_arr = []
            light_arr = []
            masking_distance_objects = values["-DISTANCE-"]
            masking_distance_lights = 50
            send_command('X')

            angle1 = int(receive_data())
            angle2 = int(receive_data())
            counter = 0
            flag = 0
            while True:
                window.refresh()
                distance = int(receive_data())
                if distance == 9999:
                    break
                arr = measure_two_ldr_samples()
                if counter > 4:  # Servo stuck around first 12 samples
                    light_distance = arr[0] + 1
                    ldr_val1 = arr[1]
                    ldr_val2 = arr[2]

                    if distance > int(masking_distance_objects):
                        window['-OUTPUT-'].update(f"Measured Distance: {distance} [cm]", append=True)
                        window['-OUTPUT-'].Widget.tag_configure("red_text", foreground="red")
                        window['-OUTPUT-'].Widget.insert("end", " (MASKED) ", "red_text")
                        distance_arr.append(0)
                    else:
                        window['-OUTPUT-'].update(f"Measured Distance: {distance} [cm]", append=True)
                        distance_arr.append(distance)

                    window['-OUTPUT-'].update(f" | Estimate Light Distance: {light_distance} [cm]", append=True)

                    if abs(ldr_val1 - ldr_val2) < object_light_epsilon and ldr_val1 < 3 and ldr_val2 < 3:
                        window['-OUTPUT-'].Widget.tag_configure("green_text", foreground="green")
                        window['-OUTPUT-'].Widget.insert("end", " - LIGHT DETECTED", "green_text")
                        if light_distance > int(masking_distance_lights):
                            light_arr.append(0)
                            flag = 0
                            window['-OUTPUT-'].Widget.tag_configure("red_text", foreground="red")
                            window['-OUTPUT-'].Widget.insert("end", " (MASKED) \n", "red_text")
                        else:
                            if flag == 1:
                                light_arr.append(light_distance)
                            else:
                                light_arr.append(0)
                            window['-OUTPUT-'].update("\n", append=True)
                            flag = 1
                    else:
                        window['-OUTPUT-'].Widget.tag_configure("red_text", foreground="red")
                        window['-OUTPUT-'].Widget.insert("end", " (NOISE) \n", "red_text")
                        flag = 0
                        light_arr.append(0)

                counter += 1

            degree_arr = [round(float(5) + i * (float(180) - float(0)) / (len(distance_arr) - 1), 1)
                          for i in range(len(distance_arr))]

            window['-OUTPUT-'].update(f"Distance array: {distance_arr}\n", append=True)
            window['-OUTPUT-'].update(f"Lights array: {light_arr}\n", append=True)
            window['-OUTPUT-'].update(f"Degree array: {degree_arr}\n", append=True)

            draw_scanner_map_lights(distance_arr, light_arr, degree_arr)
            window["-LIGHT-OBJECT_DETECT-"].update(disabled=False)
            window["-BACK-"].update(disabled=False)

    window.close()


def script_mode():
    global ACK
    global window
    working_directory = "C:/Users/Tal Shvartzberg/Desktop/Software projects/C/BGU/Tal/CCS/FinalProject/Scripts"

    layout = [
        [sg.Text("Choose a TXT file to upload:", text_color='#000000', background_color='#9AF1FF',
                 font=('Segoe UI', 10))],
        [sg.InputText(key="-FILE_PATH-", size=(70, 1)),
         sg.FileBrowse(initial_folder=working_directory, file_types=[("text Files", "*.txt")], button_color='#000000')],
        [sg.Button("Upload Script1", key="-UPLOAD1-", button_color='#000000'),
         sg.Button("Play Script1", key="-PLAY1-", button_color='#000000')],
        [sg.Button("Upload Script2", key="-UPLOAD2-", button_color='#000000'),
         sg.Button("Play Script2", key="-PLAY2-", button_color='#000000')],
        [sg.Button("Upload Script3", key="-UPLOAD3-", button_color='#000000'),
         sg.Button("Play Script3", key="-PLAY3-", button_color='#000000')],
        [sg.Exit(button_color='#000000', key="-BACK-")],
        [sg.Text(key='Script Transferred', text_color='#000000', background_color='#9AF1FF', font=('Segoe UI', 10))],
        [sg.Output(key="-OUTPUT-", size=(75, 5))]

    ]

    window = sg.Window("Upload File", layout, background_color='#9AF1FF', font=('Segoe UI', 10), resizable=True,
                       return_keyboard_events=True, finalize=True)
    window.bind("<Escape>", "-ESCAPE-")

    while True:
        window.refresh()
        ACK = '0'
        event, values = window.read()

        if event in (sg.WIN_CLOSED, 'Exit', "-ESCAPE-"):
            send_command('0')
            break
        elif event in ["-UPLOAD1-", "-UPLOAD2-", "-UPLOAD3-"]:
            window["-FILE_PATH-"].update(disabled=True)
            window["-UPLOAD1-"].update(disabled=True)
            window["-UPLOAD2-"].update(disabled=True)
            window["-UPLOAD3-"].update(disabled=True)
            window["-PLAY1-"].update(disabled=True)
            window["-PLAY2-"].update(disabled=True)
            window["-PLAY3-"].update(disabled=True)
            window["-BACK-"].update(disabled=True)
            if event == "-UPLOAD1-":
                send_command('A')
                file_address = values["-FILE_PATH-"]
                script = open(file_address)
                string = command_encoder(script.read())
                print(os.path.basename(script.name))
                command = str(len(string).to_bytes(1, 'big'))[2]
                send_command(command)
                send_data(string)
                receive_ack()
            elif event == "-UPLOAD2-":
                send_command('B')
                file_address = values["-FILE_PATH-"]
                script = open(file_address)
                string = command_encoder(script.read())
                print(os.path.basename(script.name))
                command = str(len(string).to_bytes(1, 'big'))[2]
                send_command(command)
                send_data(string)
                receive_ack()
            elif event == "-UPLOAD3-":
                send_command('C')
                file_address = values["-FILE_PATH-"]
                script = open(file_address)
                string = command_encoder(script.read())
                print(os.path.basename(script.name))
                command = str(len(string).to_bytes(1, 'big'))[2]
                send_command(command)
                send_data(string)
                receive_ack()
        elif event in ["-PLAY1-", "-PLAY2-", "-PLAY3-"]:
            window["-FILE_PATH-"].update(disabled=True)
            window["-UPLOAD1-"].update(disabled=True)
            window["-UPLOAD2-"].update(disabled=True)
            window["-UPLOAD3-"].update(disabled=True)
            window["-PLAY1-"].update(disabled=True)
            window["-PLAY2-"].update(disabled=True)
            window["-PLAY3-"].update(disabled=True)
            window["-BACK-"].update(disabled=True)

            if event == "-PLAY1-":
                send_command('D')
                window['Script Transferred'].update("Playing Script1, Please wait.")
            elif event == "-PLAY2-":
                send_command('E')
                window['Script Transferred'].update("Playing Script2, Please wait.")
            elif event == "-PLAY3-":
                send_command('F')
                window['Script Transferred'].update("Playing Script3, Please wait.")

            # while finish_script_flag == 0:
            while True:
                window.refresh()
                opcode = receive_char()
                opcode_dict = {
                    '1': "Increment char on LCD from 0 to 'x'",
                    '2': "Decrement char on LCD from 'x' to 0",
                    '3': "Right rotating char 'x' on LCD Screen",
                    '4': "Setting delay value",
                    '5': "Clearing LCD screen",
                    '6': "Moving sensor to specific angle and measure distance",
                    '7': "Scanning environment from angle1 to angle2",
                    '8': "MSP goes back to sleep mode"
                }

                window['-OUTPUT-'].Widget.tag_configure("blue_text", foreground="blue")
                window['-OUTPUT-'].Widget.insert("end", f"Playing Opcode {opcode}: ", "blue_text")
                print(f"({opcode_dict[opcode]})")

                # print(f"Playing Opcode: {opcode} ({opcode_dict[opcode]})")
                if opcode == '6':
                    window.refresh()
                    distance = int(receive_data())
                    angle = int(receive_data())
                    print(f"Measured Distance: {distance} [cm], Measured Angle: {angle} [°]")

                elif opcode == '7':
                    distance_arr = []
                    angle1 = int(receive_data())
                    angle2 = int(receive_data())

                    while True:
                        window.refresh()
                        distance = int(receive_data())
                        if distance == 999:
                            break
                        print(f"Measured Distance: {distance} [cm]")
                        distance_arr.append(distance)

                    degree_arr = [
                        round(float(angle1) + i * (float(angle2) - float(angle1)) / (len(distance_arr) - 1), 1)
                        for i in range(len(distance_arr))]
                    print(f"Distance array: {distance_arr}")
                    print(f"Degree array: {degree_arr}")
                    draw_scanner_map(distance_arr, degree_arr)

                elif opcode == '8':
                    break

        if ACK == '1':
            window['Script Transferred'].update("Script1 Transferred")
        elif ACK == '2':
            window['Script Transferred'].update("Script2 Transferred")
        elif ACK == '3':
            window['Script Transferred'].update("Script3 Transferred")
        else:
            window['Script Transferred'].update("")
        window["-FILE_PATH-"].update(disabled=False)
        window["-UPLOAD1-"].update(disabled=False)
        window["-UPLOAD2-"].update(disabled=False)
        window["-UPLOAD3-"].update(disabled=False)
        window["-PLAY1-"].update(disabled=False)
        window["-PLAY2-"].update(disabled=False)
        window["-PLAY3-"].update(disabled=False)
        window["-BACK-"].update(disabled=False)

    window.close()


def send_angle(angle):
    send_data(str(angle).rjust(3, '0'))


def init_uart():
    global s, inChar
    s = ser.Serial('COM13', baudrate=9600, bytesize=ser.EIGHTBITS,
                   parity=ser.PARITY_NONE, stopbits=ser.STOPBITS_ONE,
                   #write_timeout=1,
                   timeout=1)  # timeout of 1 sec so that the read and write operations are blocking,
    # after the timeout the program continues
    # clear buffers
    s.reset_input_buffer()
    s.reset_output_buffer()
    inChar = '0'


def send_data(data_str):
    global s
    for char in data_str:
        a = len(data_str)
        send_command(char)
    time.sleep(0.05)
    s.write(bytes('$', 'ascii'))


def send_command(char):
    global s
    s.write(bytes(char, 'ascii'))
    time.sleep(0.05)  # delay for accurate read/write operations on both ends


def command_encoder(data_str):
    translated_string = ""
    lines = data_str.split('\n')
    for line in lines:
        line = line.strip()
        if line:
            parts = line.split(' ', 1)
            command = parts[0]
            args = parts[1] if len(parts) > 1 else ""
            hex_value = command_dict.get(command)
            if hex_value is not None:
                opcode = hex(hex_value)[2:].zfill(2)  # Get the opcode in hex format
                hex_args = ''
                if args:
                    # Split the arguments by comma and convert each argument to hex format
                    hex_args_list = [hex(int(arg))[2:].zfill(2).upper() for arg in args.split(',')]
                    hex_args = ''.join(hex_args_list)  # Concatenate the hex arguments
                translated_string += opcode + hex_args + '\n'
    return translated_string


def receive_ack():
    global s, ACK
    time.sleep(0.25)  # delay for accurate read/write operations on both ends
    ACK = s.read_until(terminator=b'\0').decode('ascii')  # read  from the buffer until the terminator is received,


def receive_data():
    chr = b''
    while chr[-1:] != b'\n':
        chr += s.read(1)

    return chr.decode('ascii')


def receive_data2():
    chr = b''
    while chr[-1:] != b'\n':
        chr += s.read(1)
    print(chr)
    return chr


def receive_char():
    data = b''
    time.sleep(0.25)  # delay for accurate read/write operations on both ends
    while len(data.decode('ascii')) == 0:
        data = s.read_until(terminator=b'\n')  # read  from the buffer until the terminator is received,
    return data.decode('ascii')


def receive_calib():
    data = b''
    time.sleep(0.25)  # delay for accurate read/write operations on both ends
    while len(data.decode('ascii')) == 0:
        data = s.read_until(terminator=b'\n')  # read  from the buffer until the terminator is received,
    return data


def save_calibration_values(calibration_values):
    with open('calibration_values.txt', 'w') as file:
        for value in calibration_values:
            file.write(str(value) + '\n')


def expand_calibration_array(calibration_array, new_length):
    expanded_array = np.zeros(new_length)

    for i in range(new_length):
        index = i // 5
        fraction = i % 5
        if i < 45:
            value = calibration_array[index] + (calibration_array[index + 1] - calibration_array[index]) * fraction / 5
        else:
            value = calibration_array[9]
        expanded_array[i] = value

    return expanded_array.tolist()


def measure_two_ldr_samples():
    LDR1_val = int(receive_data()) / 292
    time.sleep(0.25)  # delay for accurate read/write operations on both ends
    if LDR1_val > 1023 / 292:
        return [-1, 0, 0]
    LDR2_val = int(receive_data()) / 292
    fitting_index = find_fitting_index(LDR1_val, LDR2_val)
    return [fitting_index, LDR1_val, LDR2_val]


def find_fitting_index(ldr1_value, ldr2_value):
    # Load calibration values from file
    with open('calibration_values.txt', 'r') as file:
        calibration_arr = [float(line.strip()) for line in file]

    # Calculate average LDR value
    average_ldr_value = (ldr1_value + ldr2_value) / 2

    # Perform binary search on calibration array
    left = 0
    right = len(calibration_arr) - 1
    fitting_value = None

    while left <= right:
        mid = (left + right) // 2
        if calibration_arr[mid] == average_ldr_value:
            fitting_value = calibration_arr[mid]
            break
        elif calibration_arr[mid] > average_ldr_value:
            fitting_value = calibration_arr[mid]
            left = mid + 1
        else:
            fitting_value = calibration_arr[mid]
            right = mid - 1

    # Handle edge cases if the average LDR value is outside the range of the calibration values
    if fitting_value is None:
        if average_ldr_value < calibration_arr[-1]:
            fitting_value = calibration_arr[-1]
        elif average_ldr_value > calibration_arr[0]:
            fitting_value = calibration_arr[0]

    return 49 - calibration_arr.index(fitting_value)


def draw_scanner_map(distances, angles):
    # Define the layout of the PySimpleGUI window
    layout = [
        [sg.Canvas(key="-CANVAS-", size=(800, 800))]
    ]

    # Create the PySimpleGUI window
    window = sg.Window("Scanner Map", layout, finalize=True, resizable=True)

    # Calculate the center point
    center = (0, 0)

    # Create the figure and axis
    fig = plt.figure(figsize=(8, 8))  # Adjust the figsize for a larger plot
    ax = fig.add_subplot(111, polar=True)

    # Determine the color map range based on the distances
    min_distance = min(distances)
    max_distance = max(distances)
    norm = plt.Normalize(vmin=min_distance, vmax=max_distance)
    cmap = cm.get_cmap('coolwarm')

    # Draw the radar lines with varying colors based on distance
    for distance, angle in zip(distances, angles):
        # Convert the angle to radians
        rad_angle = math.radians(angle)

        # Determine the color based on distance
        color = cmap(1 - norm(distance))

        # Draw the dots with the corresponding color
        ax.scatter(rad_angle, distance, color=color, s=10)

    # Add distance labels
    radii = ax.get_yticks()
    for radius in radii[1:]:
        ax.text(0, radius, str(radius), ha='center', va='bottom', fontsize=8)

    # Set the limits of the plot
    padding = max_distance * 0.1  # Add a 10% padding
    ax.set_ylim(0, max_distance + padding)
    ax.set_yticklabels([])  # Hide radial tick labels
    ax.set_xticklabels([])  # Hide angular tick labels

    # Set only the top spine visible
    ax.spines["polar"].set_visible(True)
    ax.spines["polar"].set_color("black")
    ax.spines["polar"].set_linewidth(0.5)
    ax.spines["polar"].set_position(("data", 0))

    # Remove the other spines
    for spine in ax.spines.values():
        if spine.spine_type != "polar":
            spine.set_visible(False)

    # Adjust the y-axis limits to zoom in on the angles
    ax.set_ylim(0, max_distance + padding * 0.3)

    # Modify the angle labels for better visibility
    ax.set_xticks(ax.get_xticks()[::2])
    ax.set_xticklabels([str(int(math.degrees(tick))) + "°" for tick in ax.get_xticks()])

    # Add bold lines for start and end angles
    start_angle = angles[0]-5
    end_angle = angles[-1]-5
    if start_angle != 0:
        ax.plot([math.radians(start_angle), math.radians(start_angle)], [0, max_distance], 'k-', linewidth=3)
        ax.text(math.radians(start_angle), max_distance + padding * 0.15, f"{start_angle}°", ha='center', va='center',
                fontsize=15)
    if end_angle != 180:
        ax.plot([math.radians(end_angle), math.radians(end_angle)], [0, max_distance], 'k-', linewidth=3)
        ax.text(math.radians(end_angle), max_distance + padding * 0.15, f"{end_angle}°", ha='center', va='center',
                fontsize=15)

    # Create the canvas and add it to the PySimpleGUI window
    canvas = FigureCanvasTkAgg(fig, master=window["-CANVAS-"].TKCanvas)
    canvas.draw()
    canvas.get_tk_widget().pack(side="top", fill="both", expand=True)

    while True:
        event, values = window.read()
        if event == sg.WINDOW_CLOSED:
            break

    window.close()


def draw_scanner_map_lights(distances, lights, angles):
    # Define the layout of the PySimpleGUI window
    layout = [
        [sg.Canvas(key="-CANVAS-", size=(800, 800))]
    ]

    # Create the PySimpleGUI window
    window = sg.Window("Scanner Map", layout, finalize=True, resizable=True)

    # Calculate the center point
    center = (0, 0)

    # Create the figure and axis
    fig = plt.figure(figsize=(8, 8))  # Adjust the figsize for a larger plot
    ax = fig.add_subplot(111, polar=True)

    max_distance = max(distances)
    if max_distance == 0:
        max_distance = 50

    # Draw the radar lines with varying colors based on distance
    i = 0
    j = 0
    for distance, light, angle in zip(distances, lights, angles):
        # Convert the angle to radians
        rad_angle = math.radians(angle)

        # Determine the color based on distance
        if light > 0 and distance < 50:
            color = "yellow"
            ax.scatter(rad_angle, distance, color=color, s=50, edgecolors='black', # light insead of distance?
                       label="Light Source" if i == 0 else "")  # remove label?
            i += 1
        else:
            color = "black"
            ax.scatter(rad_angle, distance, color=color, s=10, label="Object" if j == 0 else "")  # remove label?
            j += 1
    plt.legend()  # remove?

    # Set the limits of the plot
    padding = max_distance * 0.1  # Add a 10% padding
    ax.set_ylim(0, max_distance + padding)
    ax.set_yticklabels([])  # Hide radial tick labels
    ax.set_xticklabels([])  # Hide angular tick labels

    radii = ax.get_yticks()
    for radius in radii[1:]:
        ax.text(0, radius, str(radius), ha='center', va='bottom', fontsize=8)

    # Set only the top spine visible
    ax.spines["polar"].set_visible(True)
    ax.spines["polar"].set_color("black")
    ax.spines["polar"].set_linewidth(0.5)
    ax.spines["polar"].set_position(("data", 0))

    # Remove the other spines
    for spine in ax.spines.values():
        if spine.spine_type != "polar":
            spine.set_visible(False)

    # Adjust the y-axis limits to zoom in on the angles
    ax.set_ylim(0, max_distance + padding * 0.3)

    # Modify the angle labels for better visibility
    ax.set_xticks(ax.get_xticks()[::2])
    ax.set_xticklabels([str(int(math.degrees(tick))) + "°" for tick in ax.get_xticks()])

    # Add bold lines for start and end angles
    start_angle = angles[0]-5
    end_angle = angles[-1]-5
    if start_angle != 0:
        ax.plot([math.radians(start_angle), math.radians(start_angle)], [0, max_distance], 'k-', linewidth=3)
        ax.text(math.radians(start_angle), max_distance + padding * 0.15, f"{start_angle}°", ha='center', va='center',
                fontsize=15)
    if end_angle != 180:
        ax.plot([math.radians(end_angle), math.radians(end_angle)], [0, max_distance], 'k-', linewidth=3)
        ax.text(math.radians(end_angle), max_distance + padding * 0.15, f"{end_angle}°", ha='center', va='center',
                fontsize=15)

    # Create the canvas and add it to the PySimpleGUI window
    canvas = FigureCanvasTkAgg(fig, master=window["-CANVAS-"].TKCanvas)
    canvas.draw()
    canvas.get_tk_widget().pack(side="top", fill="both", expand=True)

    while True:
        event, values = window.read()
        if event == sg.WINDOW_CLOSED:
            break

    window.close()


def main():
    global s

    MenuLayout = [[sg.Button('', image_filename='Menu Buttons/button1.png',
                             button_color=(sg.theme_background_color(), sg.theme_background_color()), border_width=0,
                             key='Object Detector System'),
                   sg.Button('', image_filename='Menu Buttons/button2.png',
                             button_color=(sg.theme_background_color(), sg.theme_background_color()), border_width=0,
                             key='Telemeter'),
                   sg.Button('', image_filename='Menu Buttons/button3.png',
                             button_color=(sg.theme_background_color(), sg.theme_background_color()), border_width=0,
                             key='Light Sources Detector System'),
                   ],
                  [sg.Button('', image_filename='Menu Buttons/button4.png',
                             button_color=(sg.theme_background_color(), sg.theme_background_color()), border_width=0,
                             key='Light Sources and Objects Detector System'),
                   sg.Button('', image_filename='Menu Buttons/button5.png',
                             button_color=(sg.theme_background_color(), sg.theme_background_color()), border_width=0,
                             key='Script Mode'),
                   sg.Button('', image_filename='Menu Buttons/button6.png',
                             button_color=(sg.theme_background_color(), sg.theme_background_color()), border_width=0,
                             key='Exit'),
                   ],
                  [sg.Text("© Tal Shvartzberg & Roi Osman", justification='left', font=('Segoe UI', 8))]
                  ]
    window = sg.Window("Light Source and Object Proximity Detector System", MenuLayout, background_color='#9AF1FF',
                       font=('Segoe UI', 14))
    init_uart()
    send_command('Z')
    msp_calib_arr = []
    ldr_val = receive_data2()
    for i in ldr_val:
        msp_calib_arr.append((4 * i)/ 292.0)
    #flash_expanded_LDR_calibrate_arr = expand_calibration_array(msp_calib_arr, 50)
    #save_calibration_values(flash_expanded_LDR_calibrate_arr)

    while True:
        event, values = window.read()
        if event == sg.WIN_CLOSED or event == 'Exit':  # if user closes window or clicks cancel
            send_command('Q')  # quit
            break
        elif event == 'Object Detector System':
            send_command('1')
            objects_detector()
        elif event == 'Telemeter':
            send_command('2')
            telemeter()
        elif event == 'Light Sources Detector System':
            send_command('3')
            lights_detector()
        elif event == 'Light Sources and Objects Detector System':
            send_command('4')
            light_objects_detector()
        elif event == 'Script Mode':
            send_command('5')
            script_mode()
        else:
            print('You entered ', values[0])

    window.close()


if __name__ == '__main__':
    command_dict = {
        "inc_lcd": 0x01,
        "dec_lcd": 0x02,
        "rra_lcd": 0x03,
        "set_delay": 0x04,
        "clear_lcd": 0x05,
        "servo_deg": 0x06,
        "servo_scan": 0x07,
        "sleep": 0x08
    }
   # init_uart()
   # send_command('2')
    main()

