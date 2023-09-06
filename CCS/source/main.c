#include  "..\header\api.h"  		// private library - API layer
//#include  "..\header\app.h"   		// private library - APP layer

void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  lcd_clear();
  while(1){
    
	switch(state){
	case state0:
	    lcd_init();
	    lcd_puts("Main Menu");
        enterLPM(lpm_mode);
        break;

	case state1: // Object Detector
        lcd_init();
        lcd_puts("Object Detector");
        enable_interrupts();
        object_detector();
        state = state0;

	break;
		 
	case state2:    // Telemeter
        lcd_init();
        lcd_puts("Telemeter");
	    enable_interrupts();
	    telemeter();
        state = state0;

	break;
                
    case state3:    // Light sources detection
        lcd_init();
        lcd_puts("Light sources");
        enable_interrupts();
        light_detector();
        state = state0;
        break;

    case state4:    // Light sources and Objects detection
        lcd_init();
        lcd_puts("Light sources");
        lcd_cursor2();
        lcd_puts("and Objects");
        enable_interrupts();
        light_object_detector();
        state = state0;

        break;

    case state5:    // Script
        lcd_init();
        lcd_puts("Script Mode");
        enable_interrupts();
        script_fsm();
        state = state0;
        break;

    case state6:    // Send Calibration array
        enable_interrupts();
        send_calib_arr();
        state = state0;
        break;
	}
  }
}
  
  
  
  
  
  
