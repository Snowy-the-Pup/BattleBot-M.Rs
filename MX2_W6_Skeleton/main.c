/* 
 * File:   main.c
 * Author: Robert Stephenson
 *
 * MX2: MXK W6 Skeleton
 */
#include <xc.h>
#include "ProcessorConfig.h"
#include "ISR.h"
#include "MXK.h"
#include "Config.h"
#include "Functions.h"

#include "Colours.h"
#include "Console.h"
#include <stdio.h>
#include "Motor.h"
#include "LCD.h"
#include "LED.h"
#include "HMI.h"

struct rec{ //create structure to record data 
    char bte[];
};
struct rec dd;
    unsigned char test;

void tx_send(unsigned char op)
//function to send code (type 'tx_send(required opcode command)')
{
       
    while (TXSTA1bits.TRMT == 0);  // when TSR is full -> TRMT =0
            TXREG1 =  op; 
        
}

void main()
{
    //Init MXK Pins
    MXK_Init();

    //Init HMI
    if (MXK_BlockSwitchTo(eMXK_HMI)) {

        HMI_Init();
        
        if (MXK_Release())
            MXK_Dequeue();
    }

    //Task 1 - Initialise the EUART (refer to datasheet section 20.0)
    //
    TRISCbits.RC6 = 1;
    TRISCbits.RC7  =1; //Set correct pin directions (refer to schematic)  Hint: TX=OUT and RX=IN
    TXSTA1bits.TX9 = 0; // NOT SURE //Set correct number of transmission bits (refer to iRobot user manual)
    TXSTA1bits.TXEN = 1;//Enable transmission
    TXSTA1bits.SYNC = 0 ; //Decide on transmission mode - Asynchronous - we want stuff to talk around without waiting
    TXSTA1bits.SENDB = 0;//Disable sync break characters
    
    RCSTA1bits.SPEN = 1; //Enable the serial port
    RCSTA1bits.RX9 = 0;//Set correct number of reception bits (same as transmission) 
    RCSTA1bits.CREN = 1;//Enable continuous reception
    RCSTA1bits.FERR = 0;//Disable any framing or overrun errors
    
    BAUDCONbits.DTRXP = 0;//Disable data inversion
    BAUDCONbits.SCKP = 0; //Idle state is high
    BAUDCONbits.WUE = 0;//Disable monitoring of the RX pin for wake up
    BAUDCONbits. ABDEN = 0;//Disable automatic baud detection
    
    //Task 2 - Set the baud rate (bits per second)
    //Check the iRobot user guide for the required baud rate and note it down
    // 57600 baud using fosc/(16(n+1)); where n is 155
    //Use the formulas in the datasheet (table 20-1) to calculate the required value for SPBRG
    SPBRG1 = 51; //set spbrg
    BAUDCONbits.BRG16 = 0;
    TXSTA1bits.BRGH =1;
    //BAUDCONbits.//Ensure that BRGH and BRG16 are set according to the formula you chose to use
    //Calculate the baud rate error using the formulas in the datasheet (example 20-1)
    //calculated error % is 0.0016
    
    int count = 1;  
    int c = 0;
    loop()
    {
        
        if (count != 0) //send code once
        { 
        tx_send(128); //start command
        tx_send(132); //full mode
        count = 0; //breaks out of code
        void script_1() //script to drive 40cm
        { 
            tx_send(152); //script initiate
            tx_send(13); //script length
            tx_send(137); //drive command
            tx_send(1);
            tx_send(44);
            tx_send(128);
            tx_send(0);
            tx_send(156); //distance wait command
            tx_send(1);
            tx_send(144);
            tx_send(137); //drive stop command
            tx_send(0);
            tx_send(0); // empty apart of the speed control commands
            tx_send(0);
            tx_send(0);
        }
            tx_send(153); //play script
        
        
        }
        
        //Task 3 - Transmit commands to the iRobot
        
        //Use the same conditional looping technique that was used last week to
        //poll the TRMT bit (shows the status of the transmission shift register)
        //If the shift register is empty then assign a value to be transmitted
        //Transmission will now take place automatically
        
         
        
         
        //Task 4 - Receive data from the iRobot
                
          while (PIR1bits.RC1IF == 0) ; // wait while buffer empty
         
        
         for(c=0;c<10;c++){  //record data from RCREG1
             
             dd.bte[c] = RCREG1;
         }
           // rec = RCREG1;
           PIR1bits.RC1IF = 0; //clear flags

        //*within hmi blockswitch*

                 
        //Use the same conditional looping technique above to check the receive flag
        //If you are expecting data to be returned then you must wait for it
        //Once data is received you can read it from the receiver and store it
        //Clear any relevant flags as the final step
        //reading the byte sequence. send a byte, give you 1 byte
        //operation code 142
        
        //HMI code
         if (MXK_BlockSwitchTo(eMXK_HMI)) 
         {
            printf("%c", ENDOFTEXT);
            printf("Hello!! \n"); // displaying important information
            
            printf("rec: %u\n", dd.bte[0]);//display total byte value
            
            printf("bumper right: %d\n", dd.bte[1] & 1); //value plus mask to show first bit
            printf("bumper left: %d\n", (dd.bte[2] >> 1) & 1); //value bitshifted by 1 and masked to show first bit
            printf("wheeldrop right: %d\n", (dd.bte[3] >> 2) & 1); //value bitshifted by 2, etc
            printf("wheeldrop left: %d\n", (dd.bte[4] >> 3) & 1);
            printf("wheeldrop castor: %d\n", (dd.bte[5] >> 4) & 1);
            Console_Render();
            if (MXK_Release())
            MXK_Dequeue();
        }
    }
}