
#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430xG46x.h>             // MSP430x4xx

//-----------------------------------------------
extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);

#define   debounceVal      250
#define   LEDs_SHOW_RATE   0xCCBB
                          // 0xCCBB is 50ms
                          // 0x7FF4D is 500ms
                          //0xFFFF  // 62_5ms // needs 8 times for 0.5 sec


//••••••••••••••••••••Inputs •••••••••••••••••••••
//-------------------------------------------------
// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0xF0
//-------------------------------------------------
// PushButtons abstraction
#define PBsArrPort	   P1IN
#define PBsArrIntPend	   P1IFG
#define PBsArrIntEn	   P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PB0                0x01  //P1.0
#define PB1                0x02  //P1.1
#define PB2                0x04  //P1.2
#define PB3                0x08  //P1.3
//-------------------------------------------------
//••••••••••••••••••••Outputs •••••••••••••••••••••
//-------------------------------------------------
// LEDs abstraction
#define LEDsArrPort        P1OUT
#define LEDsArrPortDir     P1DIR
#define LEDsArrPortSel     P1SEL
//-------------------------------------------------
// PWM abstraction
#define PwmPort           P2OUT
#define PwmPortDir        P2DIR
#define PwmPortSel        P2SEL
#define Port2out          P2OUT  


//•••••••••••••••••••• Timers •••••••••••••••••••••
                  
#define CntDownTimerVec TIMER0_A1_VECTOR      
#define FreqCntTimerVec TIMERB1_VECTOR  
#define ADC_Int_Vec     ADC12_VECTOR
                                  
#define PWM_OUTLeg       0x04       //p2.2 
#define PWM_INLeg        0x08       //p2.3  //  NOT  P2.4

#define Timer_1_CLR        TBCLR
#define Timer_1_SSEL2      TBSSEL_2
#define Timer_1_CCR1_IFG   TBIV_TBCCR1
#define Timer_1_CCR2_IFG   TBIV_TBCCR2
#define Timer_1_IV_NONE    TBIV_NONE
#define Timer_1_IFG       TBIFG                                 
#define Timer_1_IV        TBIV
#define Timer_1_CTL       TBCTL     
#define Timer_1_CCR0      TBCCR0
#define Timer_1_CCR1      TBCCR1
#define Timer_1_CCR2      TBCCR2
#define Timer_1_CCTL0     TBCCTL0                                  
#define Timer_1_CCTL1     TBCCTL1                                  
#define Timer_1_CCTL2     TBCCTL2
#define Timer_1_R         TBR
#define Timer_1_IE        TBIE  
   
#define Timer_2_SSEL2     TASSEL_2
#define Timer_2_CLR       TACLR
#define Timer_2_CTL       TACTL 
#define Timer_2_CCR0      TACCR0  
#define Timer_2_CCTL0     TACCTL0                                    
#define Timer_2_R         TAR            
#define Timer_2_IE        TAIE 
#define Timer_2_IFG       TAIFG  

              
//•••••••••••••••ADC abstraction•••••••••••••••••

#define AdcPortSelIn        P2IN
#define AdcPortSelOut       P2OUT
#define AdcPortDir          P2DIR
#define AdcPortSel          P2SEL
#define AdcPortIntPend      P2IFG
#define AdcPortIntEn        P2IE
#define AdcPortIntEdgeSel   P2IES
#define AdcPwmLeg           0x04       //p2.2 
#define Adc_Mem             ADC12MEM0
#define Adc_CTL0            ADC12CTL0
#define Adc_CTL1            ADC12CTL1
#define Adc_AE0             ADC12AE0
#define Adc_ON              ADC12ON
#define Adc_IE              ADC12IE
#define Adc_SHT_3           SHT0_3
#define Adc_SSEL_3          ADC12SSEL_3
#define ADC_SC              ADC12SC
//----------------------------------------------




//-------------------------------------------------
// LCD abstraction

#ifdef CHECKBUSY
  #define	LCD_WAIT lcd_check_busy()
#else
  #define LCD_WAIT DelayMs(5)
#endif

/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection
------------------------------------------------------------*/
#define LCD_EN(a)	(!a ? (P2OUT&=~0X20) : (P2OUT|=0X20)) // P1.5 is lcd enable pin
#define LCD_EN_DIR(a)	(!a ? (P2DIR&=~0X20) : (P2DIR|=0X20)) // P1.5 pin direction 

#define LCD_RS(a)	(!a ? (P2OUT&=~0X40) : (P2OUT|=0X40)) // P1.6 is lcd RS pin
#define LCD_RS_DIR(a)	(!a ? (P2DIR&=~0X40) : (P2DIR|=0X40)) // P1.6 pin direction  
  
#define LCD_RW(a)	(!a ? (P2OUT&=~0X80) : (P2OUT|=0X80)) // P1.7 is lcd RW pin
#define LCD_RW_DIR(a)	(!a ? (P2DIR&=~0X80) : (P2DIR|=0X80)) // P1.7 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset
   
#define LCD_DATA_WRITE	P1OUT
#define LCD_DATA_DIR	P1DIR
#define LCD_DATA_READ	P1IN


/*---------------------------------------------------------
  END CONFIG
-----------------------------------------------------------*/
#define FOURBIT_MODE	0x0
#define EIGHTBIT_MODE	0x1
#define LCD_MODE        FOURBIT_MODE
   
#define OUTPUT_PIN      1	
#define INPUT_PIN       0	
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00	

#define LCD_STROBE_READ(value)	LCD_EN(1), \
				asm("nop"), asm("nop"), \
				value=LCD_DATA_READ, \
				LCD_EN(0) 

#define	lcd_cursor(x)		lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()		lcd_cmd(0x01)
#define lcd_putchar(x)		lcd_data(x)
#define lcd_goto(x)		lcd_cmd(0x80+(x))
#define lcd_cursor_right()	lcd_cmd(0x14)
#define lcd_cursor_left()	lcd_cmd(0x10)
#define lcd_display_shift()	lcd_cmd(0x1C)
#define lcd_home()		lcd_cmd(0x02)
#define cursor_off              lcd_cmd(0x0C)
#define cursor_on               lcd_cmd(0x0F) 
#define lcd_function_set        lcd_cmd(0x3C) // 8bit,two lines,5x10 dots 
#define lcd_new_line            lcd_cmd(0xC0)                                  





#endif

//Changes from Pesronal Kit to Lab:
 
//bsp.c:   
//hal.c:   
//api.c: 
//main.c:  



