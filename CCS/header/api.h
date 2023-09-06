#ifndef _api_H_
#define _api_H_

#include  "..\header\halGPIO.h"      // private library - HAL layer
//#include  "..\header\app.h"        // private library - APP layer


void object_detector();
void telemeter();
void light_detector();
void light_object_detector();
void script_fsm();
void move_servo(int);
void servo_scan(int, int, int);
void meas_and_send_distance();
void meas_and_send_ldr();
void calc_and_send_angle(int);
int measure_distance();
void inc_lcd(int);
void dec_lcd(int);
void rra_lcd(int);
void set_delay(int);
void servo_deg(int);
void LDR(int);
void flash_write(int);
void flash_write_calib(int, int);
void play_script(int);
void addScript(const char*, int);
void lcd_puts(const char * s);
void div16(int, int, int *, int *);
extern void send_calib_arr();

#endif




