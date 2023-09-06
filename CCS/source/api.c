#include  "..\header\api.h"     		// private library - API layer
//#include  "..\header\app.h"           // private library - APP layer
//#include  "..\header\halGPIO.h"
#include "stdio.h"
#include "string.h"
#include <stdlib.h>




//••••••••••••••••••••••••••••••• Main Functions •••••••••••••••••••••••••••••••

//------------------------------------------------------------------------------
// 1. Object Detector function
//------------------------------------------------------------------------------

void object_detector(){

    state_object_detector = detector_sleep;
    while(state == state1){
        // Object Detector FSM

        switch(state_object_detector){
        case detector_sleep:
            lcd_init();
            lcd_puts("Object Detector");
            enterLPM(lpm_mode);
            break;

        case detector_action:
            enable_interrupts();
            lcd_clear();
            lcd_puts("Scanning");
            lcd_cursor2();
            lcd_puts("Environment");
            servo_scan(0, 180, 1);
            state_object_detector = detector_sleep;
            break;
        }
    }
}

//------------------------------------------------------------------------------
// 2. Telemeter function
//------------------------------------------------------------------------------

void telemeter(){

    state_telemeter = tele_sleep;
    while(state == state2){
        // TELEMETER FSM

        switch(state_telemeter){
        case tele_sleep:
            lcd_init();
            lcd_puts("Telemeter");
            enterLPM(lpm_mode);
            break;

        case tele_action:
            enable_interrupts();
            enterLPM(lpm_mode);  // expect to receive angle (array of chars)
            tele_angle_int = atoi(tele_angle);

            move_servo(tele_angle_int); // move the servo to deg p

            while(state_telemeter == tele_action){
                servo_deg(tele_angle_int); // send measured distance and angle to pc (and LCD)
                __delay_cycles(100000);
            }
            state_telemeter = tele_sleep;
        break;
        }
    }
}


//------------------------------------------------------------------------------
// 3. Light sources detection
//------------------------------------------------------------------------------

void light_detector(){


    state_light_detector = light_sleep;
      while(state == state3){
          // LIGHT DETECTOR FSM

          switch(state_light_detector){
          case light_sleep:
              lcd_init();
              lcd_puts("Light sources");
              enable_interrupts();
              enterLPM(lpm_mode);
              break;

          case light_calibrate:
                lcd_init();
                lcd_puts("Calibrating");
                enable_interrupts();
                enterLPM(lpm_mode);  // expect to PB0 Press
                disable_interrupts();

                LDR(1);             // Sample LDR1 (left),  changes the global LDR_val array
                send_LDR_value();   // Send value to PC


                __delay_cycles(200000);

                LDR(2);             // Sample LDR2 (right), changes the global LDR_val array
                send_LDR_value();   // Send value to PC


                flash_write_calib(avg_sample, 0X10B4 + calibrate_index);
                calibrate_index++;


                if (calibrate_index == 10){
                       calibrate_index = 0;
                       state_light_detector = light_sleep;
                       disable_flash_write();
                }
                break;

          case light_scan:
                enable_interrupts();

                servo_scan(0, 180, 2);

                state_light_detector = light_sleep;
                break;
          }
      }
}


//------------------------------------------------------------------------------
// 4. Light sources and Objects detection function (bonus)
//------------------------------------------------------------------------------

void light_object_detector(){
    state_light_object_detector = light_object_sleep;
    while(state == state4){
        // Lights & Object Detector FSM

        switch(state_light_object_detector){
        case light_object_sleep:
            lcd_init();
            lcd_puts("Lights & Object");
            lcd_cursor2();
            lcd_puts("Detector");
            enterLPM(lpm_mode);
            break;

        case light_object_scan:
            enable_interrupts();
            lcd_clear();
            lcd_puts("Scanning");
            lcd_cursor2();
            lcd_puts("Environment");
            servo_scan(0, 180, 3);
            state_light_object_detector = light_object_sleep;
            break;
        }
    }
}

//------------------------------------------------------------------------------
// 5. script menu FSM function
//------------------------------------------------------------------------------

void script_fsm(){

 state_script = sleep;
 while(state == state5){
    switch(state_script){
    case sleep:
        enterLPM(lpm_mode);
        break;

    case upload_file1:
        lcd_init();
        lcd_puts("Uploading File 1");
        enable_interrupts();
        enterLPM(lpm_mode);  // expect to receive script data
        send_char('1');  // Send acknowledge to the PC side after receiving the script successfully.
        addScript("script0", script_length); // NEED TO ADD ARRAY OF POINTERS TO THE START ADDRRESS OF EVERY FILE
        flash_write(1);   // put script_string into flash

        state_script = sleep;
    break;

    case upload_file2:
        lcd_init();
        lcd_puts("Uploading File 2");
        enable_interrupts();
        enterLPM(lpm_mode);  // expect to receive script data
        send_char('2');  // Send acknowledge to the PC side after receiving the script successfully.
        addScript("script1", script_length);
        flash_write(2);   // put script_string into flash


        state_script = sleep;
    break;

    case upload_file3:
        lcd_init();
        lcd_puts("Uploading File 3");
        enable_interrupts();
        enterLPM(lpm_mode);  // expect to receive script data
        send_char('3');  // Send acknowledge to the PC side after receiving the script successfully.
        addScript("script2", script_length);
        flash_write(3);   // put script_string into flash

        state_script = sleep;
        break;

    case play_file1:
        lcd_init();
        lcd_puts("Playing File1");
        enable_interrupts();
        play_script(1);
        lcd_clear();
        lcd_puts("Done Playing");
        lcd_cursor2();
        lcd_puts("File 1");

        state_script = sleep;
        break;

    case play_file2:
        lcd_init();
        lcd_puts("Playing File2");
        enable_interrupts();
        play_script(2);
        lcd_clear();
        lcd_puts("Done Playing");
        lcd_cursor2();
        lcd_puts("File 2");
        state_script = sleep;
        break;

    case play_file3:
        lcd_init();
        lcd_puts("Playing File3");
        enable_interrupts();
        play_script(3);
        lcd_clear();
        lcd_puts("Done Playing");
        lcd_cursor2();
        lcd_puts("File 3");
        state_script = sleep;
        break;

    }
  }
}

//••••••••••••••••••••••••••••••• Helper Functions •••••••••••••••••••••••••••••

//------------------------------------------------------------------------------
//                                  Servo motor
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Move the servo (straight to the angle)
//------------------------------------------------------------------------------
void move_servo(int angle){
    angle = (angle * 10) + 100;     // angle*(2000-200)/180 + 100

    TIMER1config();

    pwmOutServoConfig(angle);
    __delay_cycles(500000);

    disable_timerA1();
}

//------------------------------------------------------------------------------
// Move the servo (angle1 to angle2 continuous scan)
//------------------------------------------------------------------------------
void servo_scan(int angle1, int angle2, int dist_or_light_flag){
    int width = 2000;
    int curr_angle = 100;
    // d is 50 [10*ms] default
    d = 25;    // Configured to 250 ms (Lower limit is d=5 !)
    TIMER1config();
    TIMER2config();
    curr_angle = (angle1 * 10) + 100;     // angle*(2000-200)/180 + 100     // innitialize
    width =      (angle2 * 10) + 100;     // angle*(2000-200)/180 + 100     // up to 180 deg


    calc_and_send_angle(angle1);
    __delay_cycles(50000);
    calc_and_send_angle(angle2);


    pwmOutServoConfig(curr_angle);
    __delay_cycles(100000);


    while(curr_angle < width){
        enable_timerA0(); //enable the timer with delay of d[msec]
        enterLPM(lpm_mode);
        disable_timerA0();
        curr_angle += 32;
        pwmOutServoConfig(curr_angle);
        __delay_cycles(50000);
        if (dist_or_light_flag == 1){
            meas_and_send_distance();       // function is changing Timer2 settings!
        }
        else if(dist_or_light_flag == 2){
            meas_and_send_ldr();
            __delay_cycles(100000);

        }
        else if(dist_or_light_flag == 3){
            meas_and_send_distance();       // function is changing Timer2 settings!
            __delay_cycles(100000);
            meas_and_send_ldr();
        }

        TIMER2config();
    }

    __delay_cycles(100000);

    // sending ending signals to PC_side

    if (dist_or_light_flag == 1){
        tele_dist[0] = '9';
        tele_dist[1] = '9';
        tele_dist[2] = '9';
        send_tele_distance();
    }
    else if(dist_or_light_flag == 2 || dist_or_light_flag == 3){
        LDR_val[0] = '9';
        LDR_val[1] = '9';
        LDR_val[2] = '9';
        LDR_val[3] = '9';
        send_LDR_value();
    }

    __delay_cycles(1500000);
    pwmOutServoConfig(1000); // INIT to 90 deg
    __delay_cycles(1500000);

    disable_timerA1();
    disable_timerA0();
}

//------------------------------------------------------------------------------
//                    Ultrasonic sensor and LDRs
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Measure distance [0-500 cm] using ultrasonic sensor
//------------------------------------------------------------------------------
int measure_distance(){
    int distance = 0;

    pwmOutTrigConfig();
    TIMER3config();
    //enable_timerA1();
    enterLPM(lpm_mode);

    if (diff > 0 && diff < 65535){
        distance = (diff/58);
    }

    disable_timerA1();

    return distance; // return distance

}

//------------------------------------------------------------------------------
//Point the ultrasonic sensor to degree p and show the degree and distance onto PC screen
//------------------------------------------------------------------------------

void servo_deg(int p){
    char lcd_print_str[4] = {'0','0','0','\0'};
    //send distance to PC
    meas_and_send_distance();
    __delay_cycles(250000);
    //send angle to PC
    calc_and_send_angle(p);

    //Print angle and distance onto LCD
    lcd_print_str[0] = tele_dist[0];
    lcd_print_str[1] = tele_dist[1];
    lcd_print_str[2] = tele_dist[2];
    lcd_init();
    lcd_puts("Distance: ");
    lcd_puts(lcd_print_str);
    lcd_puts(" cm");

    lcd_cursor2();

    lcd_print_str[0] = tele_angle[0];
    lcd_print_str[1] = tele_angle[1];
    lcd_print_str[2] = tele_angle[2];
    lcd_puts("Angle: ");
    lcd_puts(lcd_print_str);

}

//------------------------------------------------------------------------------
// Measure the distance and send to PC_SIDE
//------------------------------------------------------------------------------
void meas_and_send_distance(){
    int distance;
    int i;
    distance = measure_distance(); // measure distance
                                    // using ultrasonic sensor [0-500 cm]

     // convert distance(int) to array of chars tele_dist[] (defined in hal).

     for (i = 2; i >= 0; --i, distance /= 10)   // Use div16 (?)
     {
         tele_dist[i] = (distance % 10) + '0';      // Use div16 (?)
     }

     // send distance to PC_side
     send_tele_distance();

}

//------------------------------------------------------------------------------
// Measure the LDR values and send to PC_SIDE
//------------------------------------------------------------------------------
void meas_and_send_ldr(){
    LDR(1);             // Sample LDR1 (left),  changes the global LDR_val array
    send_LDR_value();   // Send value to PC

    __delay_cycles(50000);

    LDR(2);             // Sample LDR2 (right), changes the global LDR_val array
    send_LDR_value();   // Send value to PC
}



//------------------------------------------------------------------------------
// Calculate the angle and send to PC_SIDE
//------------------------------------------------------------------------------
void calc_and_send_angle(int p){
   int i;

    for (i = 2; i >= 0; --i, p /= 10)   // Use div16 (?)
         {
        tele_angle[i] = (p % 10) + '0';      // Use div16 (?)
         }

    send_tele_angle();
}


//------------------------------------------------------------------------------
// LDR
//------------------------------------------------------------------------------

void LDR(int LDR_index){
    ADCsampLDRconfig();
    int sample = 0;

    int i;

    if (LDR_index == 1){
        ADCconfigLDR1();
    }
    else if (LDR_index == 2){
        ADCconfigLDR2();
    }

    enable_ADC();
    __bis_SR_register(LPM0_bits + GIE); //enter sleep
    disable_ADC();

    sample = sample_ADC();

    if (LDR_index == 1){
        sample1 = sample;
    }
    else if (LDR_index == 2){
        sample2 = sample;
        avg_sample = sample1 + sample2;
    }

    for (i = 3; i >= 0; --i, sample /= 10)     // Use div16 (?)
        {
        LDR_val[i] = (sample % 10) + '0';      // Use div16 (?)
        }
}


//------------------------------------------------------------------------------
//                              Scripts
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Play a Script
//------------------------------------------------------------------------------
void play_script(int script_num){
    char d_string [12] = {'d',' ','=',' ','$','$',' ','[','m','s',']','\0'};
    int sleep_state_flag = 0;
    int start_addr = 0x1000;

    start_addr = scriptManager.file_location[script_num-1]; // get script's start addr in flash
    char * read_ptr = (char *) start_addr; // set the pointer to the start address.

    while (sleep_state_flag == 0){
        opcode[0] = *read_ptr++;
        opcode[1] = *read_ptr++;
        send_opcode(); // send opcode to pc_side
        __delay_cycles(1250000);


        if(*read_ptr != 10){    // if there are any arguments:
            // get first argument:
            arg1[0] = *read_ptr++;
            arg1[1] = *read_ptr++;

            if(*read_ptr != 10){ // if there are 2 arguments:
                      // get second argument:
                      arg2[0] = *read_ptr++;
                      arg2[1] = *read_ptr++;
                      *read_ptr++; // next line
            }
            else{ // next line
                *read_ptr++;
            }
        }
        else{ // next line
            *read_ptr++;
        }

     // after receiving a whole line:
        opcode_int = atoi(opcode);
        arg1_int =   strtol(arg1, NULL, 16);
        arg2_int =   strtol(arg2, NULL, 16);

        switch(opcode_int){
            case 1:
                inc_lcd(arg1_int);
            break;

            case 2:
                dec_lcd(arg1_int);
            break;

            case 3:
                rra_lcd(arg1_int);
            break;

            case 4:
                d_string[4] = (arg1_int / 10) + '0';
                d_string[5] = (arg1_int % 10) + '0';
                lcd_clear();
                lcd_puts(d_string);
                set_delay(arg1_int);
            break;

            case 5:
                lcd_clear();
            break;

            case 6:
                move_servo(arg1_int);
                servo_deg(arg1_int);
                __delay_cycles(100000); // IMPORTANT
            break;

            case 7:
                lcd_clear();
                lcd_puts("Scanning");
                lcd_cursor2();
                lcd_puts("Environment");
                servo_scan(arg1_int, arg2_int, 1);
            break;

            case 8:
                sleep_state_flag = 1;
            break;
        }
    }
}


//------------------------------------------------------------------------------
// A function to update the struct after successfully receiving a script
//------------------------------------------------------------------------------

void addScript(const char * filename, int scriptSize) {
    if (scriptManager.numScripts >= MAX_SCRIPTS) {
        // Maximum number of scripts reached
        return;
    }

    // Copy the filename to the fixed-length array
    strncpy(scriptManager.filenames[scriptManager.numScripts], filename, MAX_FILENAME_LENGTH - 1);
    scriptManager.filenames[scriptManager.numScripts][MAX_FILENAME_LENGTH - 1] = '\0';

    // Store the script size
    scriptManager.scriptSizes[scriptManager.numScripts] = scriptSize;

    // Increment the number of scripts
    scriptManager.numScripts++;
}



//------------------------------------------------------------------------------
//Count up from 0 to x with delay of d
//------------------------------------------------------------------------------
void inc_lcd(int x){
  int upcounter = 0;
  char upcounter_str[10];

  TIMER2config();
  lcd_init();
  reset_timerA0();
  disable_interrupts();
  while(upcounter <= x)
  {
      enable_timerA0(); //enable the timer with delay of d[msec]
      enterLPM(lpm_mode);
      disable_timerA0();
      sprintf(upcounter_str, "%d", upcounter);
      lcd_clear();
      lcd_puts(upcounter_str);      //print
      upcounter ++;
  }
  lcd_clear();
  disable_timerA0();
}


//------------------------------------------------------------------------------
//Count down from x to 0 with delay of d
//------------------------------------------------------------------------------
void dec_lcd(int x){
  int downcounter = x;
  char downcounter_str[10];

  TIMER2config();
  lcd_init();
  reset_timerA0();
  while(downcounter >= 0)
  {
      enable_timerA0(); //enable the timer with delay of d[msec]
      enterLPM(lpm_mode);
      disable_timerA0();
      sprintf(downcounter_str, "%d", downcounter);
      lcd_clear();
      lcd_puts(downcounter_str);      //print
      downcounter --;
  }
  lcd_clear();
  disable_timerA0();
}


//------------------------------------------------------------------------------
//Rotate right onto LCD
//------------------------------------------------------------------------------

void rra_lcd(int x){
    char char_to_rotate [2] = {'0','\0'};
    char_to_rotate[0] = x + '0';
    int counter=0;
    TIMER2config();
    lcd_init();
    lcd_clear();

    while (counter < 32){
        enable_timerA0(); //enable the timer with delay of d[msec]
        enterLPM(lpm_mode);
        disable_timerA0();


        lcd_cursorLeft();
        lcd_puts(" ");
        lcd_puts(char_to_rotate);


        if (counter == 15){
            lcd_cursorLeft();
            lcd_puts(" ");
            lcd_cursor2();
        }
        else if(counter == 31){
            lcd_clear();
            disable_timerA0();
        }
        counter++;
    }
}

//------------------------------------------------------------------------------
//Get delay time d[10 ms]
//------------------------------------------------------------------------------

void set_delay(int delay){
  d=delay;
}

//------------------------------------------------------------------------------
//                                   FLASH
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Write to flash memory
//------------------------------------------------------------------------------
void flash_write(int script_num){
   int i=0;
   int addr=0x1000;
   FlashConfig();

   switch(script_num){
    case 1:
        addr=0x1000;
    break;

    case 2:
        addr=0x1040;
    break;

    case 3:
        addr=0x1080;
    break;
   }

   init_flash_write(addr);

   while(script_string[i]!= '$' && i!=63){
       write_flash_char(script_string[i]);
       i++;
   }

   disable_flash_write();
}


void flash_write_calib(int ldr_value, int addr){
    char ldr_value_byte;
    FlashConfig();
    ldr_value_byte = ldr_value >> 3;
    write_with_addr_flash_char(ldr_value_byte, addr);
}


void send_calib_arr(){
    char val;
    int i;
    int start_addr = 0x10B4;
    char * read_ptr = (char *) start_addr; // set the pointer to the start address.
    for (i=0;i<10;i++){
        calib_val[i] = *read_ptr++;
        send_calib();
// __delay_cycles(100000);
    }
}

//------------------------------------------------------------------------------
//                                 QFORMAT
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DIV function:
//------------------------------------------------------------------------------

void div16(int divided, int divisor, int *quotient, int *remainder) {
    *quotient = divided / divisor;
    *remainder = divided % divisor;

}

/*
//------------------------------------------------------------------------------
// DIV32 function:
//------------------------------------------------------------------------------

void div32(unsigned long divided, unsigned long divisor, unsigned long *quotient, unsigned long *remainder) {

    unsigned long R9 = 32;
    unsigned long R4 = divided;
    unsigned long R5 = 0;
    unsigned short R6 = divisor;
    unsigned short median = divisor>>1;
    unsigned long R8 = 0;
    unsigned long carry = 0;
    while (R9 > 0){
        carry = (R4 & 0x80000000) >> 31;
        R4 = (R4<<1);
        R5 = (R5<<1);
        R5 |= carry;

        if (R5 < R6){
            R8 = (R8<<1);
        }
        else{
            R5 -= R6;
            carry = 1;
            R8 = (R8<<1);
            R8 |= carry;
        }
        R9--;
    }

    if(R5>median){
        R8++;
    }
    *quotient = R8;
    *remainder = R5;
}

//------------------------------------------------------------------------------
// MUL function:
//------------------------------------------------------------------------------

void mul(signed short X, signed short Y, unsigned long * result){
    unsigned short R8=1;
    unsigned long temp=0;
    unsigned short R4 = X;
    unsigned short R5 = Y;
    unsigned short R6=0;
    unsigned long R7=0;
    unsigned short carry = 0;
    while (R8!=0){
        if(R8 & R5){
            R7 += R4;
        }
        carry = R7 & 0x1;
        R6 = R6 >> 1;
        R6 = R6 | (carry<<15);
        R7 = R7 >> 1;
        R8 = R8<<1;
    }
    R7 = R7 <<16;
    temp = (unsigned long)R6;
    *result = R7 + temp;
}


*/
