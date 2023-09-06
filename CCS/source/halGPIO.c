#include  "..\header\halGPIO.h"     // private library - HAL layer
//#include  "..\header\app.h"         // private library - APP layer
#include <stdio.h>
#include <stdlib.h>

//•••••••••••••••••••••••••••••• Variables ••••••••••••••••••••••••••••••

// General variables
enum FSMstate state;
enum SYSmode lpm_mode;

// Sending Calibration Array
calib_flag = 0;
calib_ind = 0;
char calib_val [11] = {'0','0','0','0','0','0','0','0','0','0','\n'};


// Variables used for Object detector function
enum FSM_object_detector state_object_detector;

// Variables used for telemeter function
enum FSM_telemeter state_telemeter;
int tele_angle_flag = 0;
char tele_angle [4] = {'0','0','0','\n'}; // in deg
int tele_index = 0;
int tele_angle_int = 0;
int tele_flag = 0;
int tele_str_ind = 0;
int tele_dist[4] = {'0','0','0','\n'};    // in cm

// Variables used for Light Detector function
enum FSM_light_detector state_light_detector;
char LDR_val[5] = {'0','0','0','0','\n'}; // up to 1023, will be translated in python to x.xx [v]
int light_str_ind = 0;
int calibrate_index = 0;
int light_flag = 0;
int avg_sample = 0;
int sample1 = 0;
int sample2 = 0;


// Variables used for Light & Object Detector function
enum FSM_light_object_detector state_light_object_detector;

// Variables used for script menu
int scriptFlag = 0;
enum FSM_script state_script;
char script_string [64];
int script_index = 0;
int script_length = 0;
ScriptManager scriptManager = {
    .numScripts = 0,
    .filenames = {NULL},
    .scriptSizes = {0},
    .file_location = {0x1000, 0x1040, 0x1080}
};
char *Flash_ptr;                          // Flash pointer
char opcode [3] = {'0','0','\n'};
char   arg1 [3] = {'0','0','\n'};
char   arg2 [3] = {'0','0','\n'};
int  opcode_int = 0;
int    arg1_int = 0;
int    arg2_int = 0;

// Variables used for the measuring distance
int capture_index=0;
int echo_arr [2];
int diff;

// Variable used for the delay timer
int timer_counter = 0;


//•••••••••••••••••••••••••••••• Configs ••••••••••••••••••••••••••••••

void sysConfig(void){ 
	GPIOconfig();
	commConfig();
    lcd_init();
}


void pwmOutServoConfig(int freq){
      Timer_1_CCR0 = (int) 20000;
      Timer_1_CCR2 = (int) freq;                 // PWM Duty Cycle
                                                 // Min=200,
                                                 //Max = 2350 (180 deg) or 2600 (190 deg)
      Timer_1_CCTL2=OUTMOD_7;                    //pwm mode

      Timer_1_CTL =  MC_1 +Timer_1_SSEL2 ;       //Continious up CCR0 + SMCLK
}


void pwmOutTrigConfig(){
      Timer_2_CCR0 = 65535;                      // Counts up to 65535 (period for PWM) , one cycle one usec (clock is 1MHz)
      Timer_2_CCR1 = 10;                         // Duty cycle A=10 (usec) for trigger
      Timer_2_CCTL1 = OUTMOD_7;                  //pwm mode
      Timer_2_CTL =  MC_1 +Timer_2_SSEL2 ;       //Continious up CCR0 + SMCLK
}


//•••••••••••••••••••••••••••••••• LCD •••••••••••••••••••••••••••••••

//******************************************************************
//                      send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){
  
	LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

	if (LCD_MODE == FOURBIT_MODE)
	{
		LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
		lcd_strobe();
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
    		LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
		lcd_strobe();
	}
	else
	{
		LCD_DATA_WRITE = c;
		lcd_strobe();
	}
}
//******************************************************************
//                      send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){
        
	LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

	LCD_DATA_WRITE &= ~OUTPUT_DATA;       
	LCD_RS(1);
	if (LCD_MODE == FOURBIT_MODE)
	{
    		LCD_DATA_WRITE &= ~OUTPUT_DATA;
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;  
		lcd_strobe();		
                LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
		LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET; 
		lcd_strobe();
	}
	else
	{
		LCD_DATA_WRITE = c;
		lcd_strobe();
	}
          
	LCD_RS(0);   
}

//******************************************************************
//                      initialize the LCD
//******************************************************************

void lcd_init(){
  
	char init_value;
    
        P2SEL &= 0xD5; // 11010101 clear bits p2.5, p2.3, p2.1
        
	if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
        else init_value = 0x3F;
	
	LCD_RS_DIR(OUTPUT_PIN);
	LCD_EN_DIR(OUTPUT_PIN);
	LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
	LCD_EN(0);
	LCD_RW(0);
        
	DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
	LCD_DATA_WRITE |= init_value;
	lcd_strobe();
	DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
	LCD_DATA_WRITE |= init_value;
	lcd_strobe();
	DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
	LCD_DATA_WRITE |= init_value;
	lcd_strobe();
	
	if (LCD_MODE == FOURBIT_MODE){
		LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
		LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
		lcd_strobe();
		lcd_cmd(0x28); // Function Set
	}
        else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots 
	
	lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
	lcd_cmd(0x1); //Display Clear
	lcd_cmd(0x6); //Entry Mode
	lcd_cmd(0x80); //Initialize DDRAM address to zero
}

//******************************************************************
//                      lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm("Nop");
  asm("nop");
  LCD_EN(0);
}

//******************************************************************
//                      lcd move cursor functions
//******************************************************************
void lcd_cursor2(){
    lcd_cmd(0xC0);
}

void lcd_cursorLeft(){
    lcd_cmd(0x10);
}

//------------------------------------------------------------------------------
// write a string of chars to the LCD
//------------------------------------------------------------------------------
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}


//•••••••••••••••••••••••••••• Delay ••••••••••••••••••••••••••••••••••


//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	for(i=t; i>0; i--);
}


//---------------------------------------------------------------------
//            Delay usec functions
//---------------------------------------------------------------------
void DelayUs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) asm("nOp"); // tha command asm("nop") takes raphly 1usec

}
//---------------------------------------------------------------------
//           Delay msec functions
//---------------------------------------------------------------------
void DelayMs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000);
        // the command asm("nop") takes roughly 1usec

}


//•••••••••••••••••••••••••• Flash Memory ••••••••••••••••••••••••••••••

void init_flash_write(int addr){
    Flash_ptr = (char *) addr;                // Initialize Flash pointer  // 0x1000,0x1040,0x1080
    FCTL1 = FWKEY + ERASE;                    // Set Erase bit
    FCTL3 = FWKEY;                            // Clear Lock bit
    *Flash_ptr = 0;                           // Dummy write to erase Flash segment
    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
}

void disable_flash_write(){
    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void write_with_addr_flash_char(char value, int addr){

    Flash_ptr = (char *) addr;
    FCTL1 = FWKEY;                      // Set WRT bit for write operation
    FCTL3 = FWKEY;                            // Clear Lock bit
    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

    *Flash_ptr = (char)value;
}

void write_flash_char(char value){
    *Flash_ptr++ = (char)value;               // Write value to flash
}



//•••••••••••••••••••••••••• Sleep Modes ••••••••••••••••••••••••••••••


//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits + GIE);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits + GIE);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits + GIE);     /* Enter Low Power Mode 2 */
        else if(LPM_level == 0x03)
	  _BIS_SR(LPM3_bits + GIE);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits + GIE);     /* Enter Low Power Mode 4 */
}

//••••••••••••••••••••••• Enable / Disable ••••••••••••••••••••••••••••
//---------------------------------------------------------------------
//                        Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//                       Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//••••••••••••••••••••••••• Timers and ADC ••••••••••••••••••••••••••••

//---------------------------------------------------------------------
//                          Timer1 - Timer A1
//---------------------------------------------------------------------
void enable_timerA1(){
    Timer_1_CCTL1 |=  CCIE;          // Timer A1 CCR2 interrupt enable
}

void disable_timerA1(){
    Timer_1_CTL   &= ~Timer_1_IE;    // TA1CTL interrupt disable
    Timer_1_CCTL1 &= ~CCIE;          // Timer A1 CCR1 interrupt disable
    Timer_1_CCTL2 &= ~CCIE;          // Timer A1 CCR2 interrupt disable 
    Timer_1_CCTL2=OUTMOD_5;                  //reset PWM mode

}

void reset_timerA1(){
    Timer_1_CTL   |= Timer_1_CLR;    // Timer A1 counter clear
    //TBR = 0x00;
}


//---------------------------------------------------------------------
//                           Timer2 - Timer A0
//---------------------------------------------------------------------
void enable_timerA0(){
    Timer_2_CCTL0 |=  CCIE;          // Timer A1 CCR0 interrupt enable
}

void disable_timerA0(){
    Timer_2_CCTL0 &= ~CCIE;          // Timer A1 CCR2 interrupt disable
}

void reset_timerA0(){
    Timer_2_CTL   |= TACLR;          // Timer A0 counter clear
}

//---------------------------------------------------------------------
//                              ADC
//---------------------------------------------------------------------

void enable_ADC(){
 Adc_CTL0 |= Adc_ON + ENC + ADC_SC ; // interrupt enabled
 Adc_CTL0 |= Adc_IE;
}

void disable_ADC(){
   Adc_CTL0 &= ~ENC ;
   Adc_CTL0 &= ~Adc_ON ;             // adc off
   Adc_CTL0 &= ~ADC_SC ;
   Adc_CTL0 &= ~ADC10IE;
}

unsigned int sample_ADC(){
    return (unsigned int)Adc_Mem;    // voltage from ADC
}

void ADCconfigLDR1(){
    Adc_CTL1 = INCH_0+Adc_SSEL_3;    // Input channel A0 (p1.0) + SMCLK
}

void ADCconfigLDR2(){
    Adc_CTL1 = INCH_3+Adc_SSEL_3;    // Input channel A3 (p1.3) + SMCLK
}


//•••••••••••••••••••••••••••• UART TX ••••••••••••••••••••••••••••••••
//---------------------------------------------------------------------
//           Send telemeter distance string
//---------------------------------------------------------------------
void send_tele_distance(){
    tele_flag = 1;
    tele_str_ind = 0;
    IE2 |= UCA0TXIE;
    UCA0TXBUF = tele_dist[tele_str_ind];
}

//---------------------------------------------------------------------
//           Send telemeter distance string
//---------------------------------------------------------------------
void send_tele_angle(){
    tele_flag = 2;
    tele_str_ind = 0;
    IE2 |= UCA0TXIE;
    UCA0TXBUF = tele_angle[tele_str_ind];
}

//---------------------------------------------------------------------
//           Send LDR value string
//---------------------------------------------------------------------
void send_LDR_value(){
    light_flag = 1;
    light_str_ind = 0;
    IE2 |= UCA0TXIE;
    UCA0TXBUF = LDR_val[light_str_ind];
}

//---------------------------------------------------------------------
//           Send opcode char
//---------------------------------------------------------------------
void send_opcode(){
    IE2 |= UCA0TXIE;
    UCA0TXBUF = opcode[1];
}

//------------------------------------------------------------------------------
// Send Char from the MSP to the PC_side
//------------------------------------------------------------------------------

void send_char(char a){
  IE2 |= UCA0TXIE;                       // Enable USCI_A0 TX interrupt
  UCA0TXBUF = a;                         // TX  character
}


void send_calib(){
    calib_flag = 1;
    calib_ind = 0;
    IE2 |= UCA0TXIE;
    UCA0TXBUF = calib_val[calib_ind];
}




//•••••••••••••••••••••••••••••• ISR ••••••••••••••••••••••••••••••••••

//*********************************************************************
//            Port1 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT2_VECTOR
  __interrupt void PBs_handler(void){
	delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
	    if(PBsArrIntPend & PB0){
          PBsArrIntPend &= ~PB0;    
          __bic_SR_register_on_exit(LPM0_bits); //out from sleep
        }
//---------------------------------------------------------------------
//            Exit from a given LPM 
//---------------------------------------------------------------------	
        switch(lpm_mode){
		case mode0:
             LPM0_EXIT; // must be called from ISR only
             break;
		 
		case mode1:
             LPM1_EXIT; // must be called from ISR only
             break;
		 
		case mode2:
             LPM2_EXIT; // must be called from ISR only
             break;
                 
        case mode3:
             LPM3_EXIT; // must be called from ISR only
             break;
                 
        case mode4:
             LPM4_EXIT; // must be called from ISR only
             break;
	}     
}
 

//********************************************************************
//            Timer 1 Interrupt Service Routine
//********************************************************************



#pragma vector=PWM_LDR_or_Echo
 __interrupt void Timer_1_ISR(void){

    if (ISR_FLAG==0){
      Timer_1_CTL   &= ~Timer_1_IFG;            // turn off flag
      __bic_SR_register_on_exit(LPM0_bits);     // out from sleep (+GIE?)
    }
    else{                                       // ECHO input capture
        echo_arr[capture_index] = (int)Timer_1_CCR1;
        capture_index += 1;
        Timer_1_CCTL1 &= ~CCIFG;
        Timer_1_CTL   &= ~Timer_1_IFG;          //turn off flag
        if(capture_index==2){
         diff = echo_arr[1]-echo_arr[0];
         capture_index=0;
         __bic_SR_register_on_exit(LPM0_bits);  //out from sleep
        }
    }
 }


//*********************************************************************
//            Timer 2 Interrupt Service Routine
//*********************************************************************
#pragma vector=delayD
 __interrupt void Timer_2_ISR(void){
     // DelayD
     if (timer_counter==16){
     timer_counter = 0;
     __bic_SR_register_on_exit(LPM0_bits); //out from sleep (+GIE?)
     }
     timer_counter++;

 }

//*******************************************************************
//            ADC Interrupt Service Rotine
//*******************************************************************

  #pragma vector=ADC_Int_Vec
  __interrupt void adc_inter(void){  
  __bic_SR_register_on_exit(LPM0_bits);//out from sleep (+GIE?)
  }


//*******************************************************************
//            UART Interrupt Service Rotine
//*******************************************************************

//-------------------------------------------------------------------
//                        Transmit (TX)
//-------------------------------------------------------------------

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
        // send distance from telemeter to PC_side
        if(tele_flag == 1){
            UCA0TXBUF = tele_dist[tele_str_ind++];
            if (tele_dist[tele_str_ind-1] == '\n'){           // TX over?
                IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
                tele_flag = 0;
            }
        }
        // send angle from telemeter to PC_side
        else if(tele_flag == 2){
            UCA0TXBUF = tele_angle[tele_str_ind++];
            if (tele_angle[tele_str_ind-1] == '\n'){           // TX over?
                IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
                tele_flag = 0;
            }
        }
        // send LDR value to PC_side
        else if(light_flag == 1){
            UCA0TXBUF = LDR_val[light_str_ind++];
            if (LDR_val[light_str_ind-1] == '\n'){           // TX over?
                IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
                light_flag = 0;
            }
        }

        else if(calib_flag == 1){
                    UCA0TXBUF = calib_val[calib_ind++];
                    if (calib_val[calib_ind-1] == '\n'){           // TX over?
                        IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
                        calib_flag = 0;
                    }
                }

        else{
            IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt
        }

}


//-------------------------------------------------------------------
//                        Receive (RX)
//-------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    // --------- Receive telemeter angle ---------
    if (tele_angle_flag == 1){
        tele_angle[tele_index++] = UCA0RXBUF;
        if (tele_index==3){
            tele_angle_flag = 0;
            __bic_SR_register_on_exit(LPM0_bits);//out from sleep
        }

    }

    // --------- Receive scripts ---------
    else if (scriptFlag == 1){
        script_length = UCA0RXBUF;
        script_index = 0;
        scriptFlag = 2;
    }
    else if (scriptFlag == 2){
        script_string[script_index++] = UCA0RXBUF;
        if (script_index == script_length+1){
            scriptFlag = 0;
            __bic_SR_register_on_exit(LPM0_bits);//out from sleep
        }
    }
    else{
        switch(UCA0RXBUF){
            case 'Q':
                exit(0);
                break;

            case '0':
                state=state0;
                break;

            case '1':
                state=state1;
                break;

            case '2':
                state=state2;
                break;

            case '3':
                state=state3;
                break;

            case '4':
                state=state4;
                break;

            case '5':
                state=state5;
                break;

            case 'A':
                state_script = upload_file1;
                scriptFlag = 1;
                break;

            case 'B':
                state_script = upload_file2;
                scriptFlag = 1;
                break;

            case 'C':
                state_script = upload_file3;
                scriptFlag = 1;
                break;

            case 'D':
                state_script = play_file1;
                break;

            case 'E':
                state_script = play_file2;
                break;

            case 'F':
                state_script = play_file3;
                break;

            case 'T':
                tele_angle_flag = 1;
                tele_index = 0;
                state_telemeter = tele_action;
                break;

            case 'M':
                state_telemeter = tele_sleep;

            case 'S':
                state_object_detector = detector_action;
                break;

            case 'J':
                state_light_detector = light_calibrate;
                break;

            case 'K':
                state_light_detector = light_scan;
                break;

            case 'X':
                state_light_object_detector = light_object_scan;
                break;

            case 'Z':
                state=state6;
                break;

            default:
                lcd_init();
                lcd_puts("ERROR");
                break;
        }
        LPM0_EXIT;

    }
}



