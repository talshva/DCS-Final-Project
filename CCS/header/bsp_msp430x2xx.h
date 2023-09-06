#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx

//------------------PIN OUT---------------------
//P1.0 - Analog input signal (channel A0) - LDR1
//P1.1 - UART
//P1.2 - UART
//P1.3 - Analog input signal (channel A3) - LDR2
//P1.4 - LCD Data D4
//P1.5 - LCD Data D5
//P1.6 - LCD Data D6
//P1.7 - LCD Data D7

//P2.0 - PB0 (for calibrating the LDR's)

//P2.1 - LCD control signal E

//P2.2 - Ultrasonic sensor output signal (ECHO) - Timer A1 CCR1

//P2.3 - LCD control signal RS

//P2.4 - Timer A1 CCR2 - PWM out (for servo motor) - Timer A1 CCR2

//P2.5 - LCD control signal RW

//P2.6 - Ultrasonic sensor input signal  (Trig) - Timer A0 CCR1

//P2.7 - ####### reserved for p2.6 SEL


//------Exporting configuration functions and variables to higher hierarchies-----------
extern void GPIOconfig(void);
extern void TIMER1config(void);
extern void TIMER2config(void);
extern void TIMER3config(void);
extern void ADCsampLDRconfig(void);
extern void commConfig(void);
extern void FlashConfig(void);
extern int d;
extern int ISR_FLAG;
#define  debounceVal  250


//-------------------------------------------------
//•••••••••••••••••••• Timers •••••••••••••••••••••
//-------------------------------------------------
//----- ISR Vectors
#define delayD              TIMER0_A0_VECTOR
#define PWM_LDR_or_Echo     TIMER1_A1_VECTOR
#define ADC_Int_Vec         ADC10_VECTOR
                                  
//----- Timer1
#define Timer_1_CLR         TACLR
#define Timer_1_SSEL2       TASSEL_2
#define Timer_1_CCR1_IFG    TA1IV_TACCR1
#define Timer_1_CCR2_IFG    TA1IV_TACCR2
#define Timer_1_IV_NONE     TA1IV_NONE
#define Timer_1_IFG         TAIFG
#define Timer_1_IV          TA1IV
#define Timer_1_CTL         TA1CTL
#define Timer_1_CCR0        TA1CCR0
#define Timer_1_CCR1        TA1CCR1
#define Timer_1_CCR2        TA1CCR2
#define Timer_1_CCTL0       TA1CCTL0
#define Timer_1_CCTL1       TA1CCTL1
#define Timer_1_CCTL2       TA1CCTL2
#define Timer_1_R           TA1R
#define Timer_1_IE          TAIE

//----- Timer2
#define Timer_2_SSEL2       TASSEL_2 //SMCLK 2^20
#define Timer_2_SSEL1       TASSEL_1 //ACLK  2^15
#define Timer_2_SSEL0       TASSEL_0 //TACLK
#define Timer_2_CCR1_IFG    TA0IV_TACCR1
#define Timer_2_CCR2_IFG    TA0IV_TACCR2
#define Timer_2_IV_NONE     TA0IV_NONE
#define Timer_2_CLR         TACLR
#define Timer_2_CTL         TA0CTL
#define Timer_2_CCR0        TA0CCR0
#define Timer_2_CCR1        TA0CCR1
#define Timer_2_CCTL0       TA0CCTL0
#define Timer_2_CCTL1       TA0CCTL1
#define Timer_2_R           TA0R
#define Timer_2_IE          TAIE
#define Timer_2_IFG         TAIFG

//-------------------------------------------------
//•••••••••••••••ADC abstraction•••••••••••••••••
//-------------------------------------------------

#define Adc_Mem             ADC10MEM
#define Adc_CTL0            ADC10CTL0
#define Adc_CTL1            ADC10CTL1
#define Adc_AE0             ADC10AE0
#define Adc_ON              ADC10ON
#define Adc_IE              ADC10IE
#define Adc_SHT_3           ADC10SHT_3
#define Adc_SSEL_3          ADC10SSEL_3
#define ADC_SC              ADC10SC

//-------------------------------------------------
//•••••••••••••••UART abstraction•••••••••••••••••
//-------------------------------------------------
#define TXD                 BIT2
#define RXD                 BIT1

//-------------------------------------------------
//••••••••••••••••••••Inputs ••••••••••••••••••••••
//-------------------------------------------------

//-------------------------------------------------
// PushButtons abstraction
#define PBsArrPort	        P2IN
#define PBsArrIntPend	    P2IFG
#define PBsArrIntEn	        P2IE
#define PBsArrIntEdgeSel    P2IES
#define PBsArrPortSel       P2SEL
#define PBsArrPortDir       P2DIR
#define PB0                 0x01  //P2.0

//-------------------------------------------------
//••••••••••••••••••••Outputs •••••••••••••••••••••
//-------------------------------------------------

//-------------------------------------------------
// LDR abstraction
#define LDRPortOut          P1OUT
#define LDRPortDir          P1DIR
#define LDRPortSel          P1SEL
#define LDR1leg             0x01  //p1.0
#define LDR2leg             0x08  //p1.3


//-------------------------------------------------
// Servo Motor abstraction
#define ServoPortDir        P2DIR
#define ServoPortSel        P2SEL
#define ServoPortSel2       P2SEL2
#define ServoPwmLeg         0x10       //p2.4


//-------------------------------------------------
// Telemeter abstraction
#define TelePortDir         P2DIR
#define TelePortSel         P2SEL
#define TelePortSel2        P2SEL2
#define TeleEchoLeg         0x04       //p2.2
#define TeleTrigLeg1        0x40       //p2.6
#define TeleTrigLeg2        0x80       //p2.7


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
#define LCD_EN(a)	    (!a ? (P2OUT&=~0X02) : (P2OUT|=0X02)) // P2.1 is lcd enable pin // INSTEAD OF 2.5 (0X20)
#define LCD_EN_DIR(a)	(!a ? (P2DIR&=~0X02) : (P2DIR|=0X02)) // P2.1 pin direction     // INSTEAD OF 2.5 (0X20)

#define LCD_RS(a)	    (!a ? (P2OUT&=~0X08) : (P2OUT|=0X08)) // P2.3 is lcd RS pin     // INSTEAD OF 2.6 (0X40)
#define LCD_RS_DIR(a)	(!a ? (P2DIR&=~0X08) : (P2DIR|=0X08)) // P2.3 pin direction     // INSTEAD OF 2.6 (0X40)
  
#define LCD_RW(a)	    (!a ? (P2OUT&=~0X20) : (P2OUT|=0X20)) // P2.5 is lcd RW pin     // INSTEAD OF 2.7 (0X80)
#define LCD_RW_DIR(a)	(!a ? (P2DIR&=~0X20) : (P2DIR|=0X20)) // P2.5 pin direction     // INSTEAD OF 2.7 (0X80)

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

#define	lcd_cursor(x)		    lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()		        lcd_cmd(0x01)
#define lcd_putchar(x)		    lcd_data(x)
#define lcd_goto(x)		        lcd_cmd(0x80+(x))
#define lcd_cursor_right()	    lcd_cmd(0x14)
#define lcd_cursor_left()	    lcd_cmd(0x10)
#define lcd_display_shift()	    lcd_cmd(0x1C)
#define lcd_home()		        lcd_cmd(0x02)
#define cursor_off              lcd_cmd(0x0C)
#define cursor_on               lcd_cmd(0x0F) 
#define lcd_function_set        lcd_cmd(0x3C) // 8bit,two lines,5x10 dots 
#define lcd_new_line            lcd_cmd(0xC0)                                  



#endif



