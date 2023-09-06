#ifndef _halGPIO_H_
#define _halGPIO_H_

#ifdef  __MSP430FG4619__
#include  "..\header\bsp_msp430x4xx.h"   
#else  // MSP430G2553
#include  "..\header\bsp_msp430x2xx.h"
#endif 
 
#include  "..\header\app.h"       	// private library - APP layer

//------Exporting configuration functions and variables to higher hierarchies-----------


// General variables
extern enum FSMstate state;        
extern enum SYSmode lpm_mode;
extern char calib_val[11];


// Variables used for Object detector function
extern enum FSM_object_detector state_object_detector;

// Variables used for Telemeter function
extern enum FSM_telemeter state_telemeter;
extern char tele_angle[4];
extern int tele_angle_int;
extern int tele_dist[4];

// Variables used for Light Detector function
extern enum FSM_light_detector state_light_detector;
extern char LDR_val[5];
extern int calibrate_index;
extern int avg_sample;
extern int sample1;
extern int sample2;
extern void write_with_addr_flash_char(char, int);



// Variables used for Light & Object Detector function
extern enum FSM_light_object_detector state_light_object_detector;

// Variables used for script menu
extern enum FSM_script state_script;
extern char opcode [3];
extern char   arg1 [3];
extern char   arg2 [3];
extern int  opcode_int;
extern int    arg1_int;
extern int    arg2_int;
extern char script_string [64];
extern int script_length;
#define MAX_SCRIPTS 3
#define MAX_FILENAME_LENGTH 8 // including "\0"

typedef struct {
    char numScripts;
    char filenames[MAX_SCRIPTS][MAX_FILENAME_LENGTH];
    int scriptSizes[MAX_SCRIPTS];
    int file_location[MAX_SCRIPTS];
} ScriptManager;

extern ScriptManager scriptManager;

// Variables used for the measuring distance
extern int diff;


// Functions
extern __interrupt void PBs_handler(void);
extern __interrupt void Timer_1_ISR(void);
extern __interrupt void Timer_2_ISR(void);
extern __interrupt void adc_inter(void);

extern void enable_timerA0(void);
extern void enable_timerA1(void);
extern void reset_timerA0(void);
extern void reset_timerA1(void);
extern void disable_timerA0(void);
extern void disable_timerA1(void);
extern void enable_ADC(void);
extern void disable_ADC(void);
extern void ADCconfigLDR1();
extern void ADCconfigLDR2();

extern unsigned int sample_ADC();
extern void send_tele_distance();
extern void send_tele_angle();
extern void send_opcode();
extern void send_char(char);
extern void send_LDR_value();
extern void send_calib();

extern void sysConfig(void);
extern void commConfig(void);
extern void pwmOutServoConfig(int);
extern void pwmOutTrigConfig();

extern void write_flash_char(char);
extern void init_flash_write(int);
extern void disable_flash_write();

extern void delay(unsigned int);
extern void enterLPM(unsigned char);

extern void enable_interrupts();
extern void disable_interrupts();

extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_init();
extern void lcd_strobe();
extern void lcd_cursor2();
extern void lcd_cursorLeft();
extern void lcd_puts(const char *);

extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);

#endif



