// private library - BSP layer

#ifdef  __MSP430FG4619__
#include  "..\header\bsp_msp430x4xx.h"   
//#pragma message("__MSP430FG4619__ is defined")
#else  // MSP430G2553    
#include  "..\header\bsp_msp430x2xx.h"
//#pragma message("__MSP430FG4619__ is not defined")
#endif 
//-----------Defining Global variables, used in higher hierarchies--------------

int d = 50;     // Timer delay value, may be changed by user in Script mode
int val;        // Timer delay value, affected by 'd'
int ISR_FLAG;   // A flag to decide which interrupt to handle on timer event

//------------------------------------------------------------------------------  
//                              GPIO configuration
//------------------------------------------------------------------------------
void GPIOconfig(void){
    WDTCTL = WDTHOLD | WDTPW;		  // Stop WDT
   
/* PushButtons Setup */
  
    PBsArrPortSel         &= ~PB0;         // PB0 is I/O
    PBsArrPortDir         &= ~PB0;         // PB0 is Input
    PBsArrIntEdgeSel      |=  PB0;  	   // PB0 is pull-up mode
    PBsArrIntEn           |=  PB0;         // enable PB0 interrupt
    PBsArrIntPend         &= ~PB0;         // clear pending interrupt
  
/* LCD Setup is in halGPIO.C */
  

/* LDR setup */       //LDR1, P1.0 (A0)  ,  LDR2, P1.3 (A3)
    LDRPortSel        &= ~LDR1leg; // LDR1 Input
    LDRPortSel        &= ~LDR2leg; // LDR2 Input
    LDRPortDir        &= ~LDR1leg; // LDR1 Input capture
    LDRPortDir        &= ~LDR2leg; // LDR2 Input capture


/* Servo motor setup */
    ServoPortSel        |=   ServoPwmLeg;   // Timer Mode
    ServoPortDir        |=   ServoPwmLeg;   //Output mode
    

/* Telemeter setup */


    // Configure P2.6 (and P2.7) to PWM out
    // Note: P1.2, P1.6 and P2.6 can all three be used as TA0.CCR1 PWM output. However, only P1.2 can be used as TA0.CCR1.CCI1A capture input.

    TelePortDir        |=     TeleTrigLeg1;
    TelePortSel        |=     TeleTrigLeg1;
    TelePortSel        &=   ~ TeleTrigLeg2;
    TelePortSel2       &=   ~(TeleTrigLeg1 | TeleTrigLeg2);


    // Configure P2.2 to timer mode, Input capture
    TelePortSel        |=    TeleEchoLeg;
    TelePortDir        &=   ~TeleEchoLeg;


    _BIS_SR(GIE);                           // enable interrupts globally

}                             
//------------------------------------------------------------------------------
//                             Timers configuration
//------------------------------------------------------------------------------

void TIMER1config(void){     /* First timer (A1) for sampling the LDRs using the ADC module,
                              * for PWMOUT (Servo) using outmod7  */
    ISR_FLAG = 0;
    Timer_1_CTL =  MC_1 +Timer_1_SSEL2 ;             //Continious up CCR0 + SMCLK
}

void TIMER2config(void){    /* Second Timer (A0) for performing script commands with 500ms interval default (will be changed to D)  */

    // Total time: [2{up/down} * 2^11 {val} * 2^4{interrupts}] / [2^20{smclk}/2^3{divby8}] = 0.5 sec default ^_^

    WDTCTL = WDTHOLD + WDTPW;                                  // Stop WDT
    val = 4*d*10;                                              //    val = 4 * 50(defaultD)*10 ~= 2^11
    Timer_2_CCR0   = (unsigned int)val;
    Timer_2_CTL    = MC_3+Timer_2_SSEL2+ID_3+Timer_2_CLR;      // UpDown to CCR0    +
                                                               // SMCLK             +
                                                               // Div by 8          +
                                                               // Clear Timer
}

void TIMER3config(void){    /* third Timer config for ultrasonic sensor */

    ISR_FLAG = 1;

    // Timer 2(A0) for PWM OUT (Trig) through P2.6 (TA0 CCR1),

    Timer_2_CTL =  MC_1 +Timer_2_SSEL2 ;                //Continious up CCR0 + SMCLK

    // Timer 1(A1) for Input capture through P2.2

    WDTCTL = WDTHOLD + WDTPW;
    Timer_1_CTL    = MC_2 + Timer_1_SSEL2;              // Continous, smclk
    Timer_1_CCTL1  = CAP | CCIE | CCIS_1 | CM_3 | SCS;  //  Capture mode + Enable interrupts +
                                                        //+ Capture/compare input channel B (p2.2) +
                                                        //+ Rising and Falling edge + Synchronize
}

//------------------------------------------------------------------------------
//                              ADC configuration (Using timer A1)
//------------------------------------------------------------------------------
void ADCsampLDRconfig(void){

  WDTCTL = WDTHOLD + WDTPW;                    // Stop WDT
  Timer_1_CTL =  MC_1 +Timer_1_SSEL2 ;             //Continious up CCR0 + SMCLK

  Adc_CTL0 = Adc_ON + Adc_IE+ Adc_SHT_3+SREF_0;   // ADC10 On/Enable           +
                                                  // Interrupt enable          +
                                                  // use 64 x ADC10CLK cycles  +
                                                  // Set ref to Vcc and Gnd

  Adc_CTL1 = INCH_0+Adc_SSEL_3;                    // Input channel A0 (p1.0) + SMCLK
  Adc_AE0 &=  ~0x08;                                 // P1.3 Analog enable
  Adc_AE0 |=   0x01;                                 // P1.0 Analog enable

}


//------------------------------------------------------------------------------
//                             UART configuration
//------------------------------------------------------------------------------

void commConfig(){

    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    //--STABILIZE
    if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
    {
      while(1);                               // do not load, trap CPU!!
    }
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;



     // P1DIR |= BIT1 + BIT2;                     // P1.1, P1.2 output
      P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
      P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
      //P1OUT &= ~(BIT1+BIT2);

      UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK
      UCA0BR0 = 104;                           //
      UCA0BR1 = 0x00;                           //
      UCA0MCTL = UCBRS0;               //
      UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
      IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}
 
             
//------------------------------------------------------------------------------
//                             FLASH configuration
//------------------------------------------------------------------------------
            
void FlashConfig(){
    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
     if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
     {
       while(1);                               // do not load, trap CPU!!
     }
     DCOCTL = 0;                               // Select lowest DCOx and MODx settings
     BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
     DCOCTL = CALDCO_1MHZ;
     FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
}


