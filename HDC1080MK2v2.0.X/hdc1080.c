#define FCY 16000000UL

#include "xc.h"
#include <assert.h>
#include <stdbool.h>
#include <libpic30.h>
#include <stdint.h>
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/i2c1.h"
#include "hdc1080.h"

#if defined(__dsPIC33E__)
	#include <p33exxxx.h>
#elif defined(__dsPIC33F__)
	#include <p33fxxxx.h>
#elif defined(__dsPIC30F__)
	#include <p30fxxxx.h>
#elif defined(__PIC24E__)
	#include <p24exxxx.h>
#elif defined(__PIC24H__)
	#include <p24hxxxx.h>
#elif defined(__PIC24F__) || defined(__PIC24FK__)
	#include <p24fxxxx.h>
#endif

#define HDC1080_ADDR    0x40

#define TEMP_OUT        0x00
#define HUMY_OUT        0x01
#define CONFIG          0x02

I2C1_MESSAGE_STATUS status;                                             // 

uint8_t Slave_Address;                                                  // definiert "Slave_Address" 

uint8_t  writeBuffer[3] = {0x00, 0x00, 0x00};                           // definiert "writeBuffer"
uint8_t  readBuffer[10] = {0x00, 0x00, 0x00};                           // definiert "readBuffer" 
uint16_t retryTimeOut1;                                                 // definiert "retryTimeOut1" 
uint16_t retryTimeOut2;                                                 // definiert "retryTimeOut2" 


void HDC1080_Init(void){                                                // "HDC1080_Init"
    Slave_Address = HDC1080_ADDR;                                       // schreibt "HDC1080_ADDR" in "Slave_Address"    
  
    writeBuffer[0] = CONFIG;                                            // schreibt "CONFIG" in "writeBuffer[0]"
    writeBuffer[1] = 0x10;                                              // schreibt "0x10" in "writeBuffer[1]"
    I2C1_MasterWrite(&writeBuffer[0], 2, Slave_Address, &status);       // schreibt 2 Byte (writeBuffer[0]) an I2C1                  
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf 
    __delay_ms(100);                                                    // warte 100ms
}                                                                       // 

void HDC1080_Read(void){                                                // "HDC1080_Read"
    Slave_Address = HDC1080_ADDR;                                       // schreibt "HDC1080_ADDR" in "Slave_Address"    
  
    writeBuffer[0] = 0x00;                                              // schreibt "0x00" in "writeBuffer[0]"
    I2C1_MasterWrite(&writeBuffer[0], 1, Slave_Address, &status);       // schreibt 1 Byte (writeBuffer[0]) an I2C1
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf   
    __delay_ms(15000);                                                  // warte 15s
    I2C1_MasterRead(&readBuffer[0], 6, Slave_Address, &status);         // liest von I2C1 in 6 Byte (readBuffer[0]) 
    i2c1_message_pending_100();                                         // ruft "i2c1_message_pending_100" auf
}                                                                       // 

float HDC1080_temp_GetValue(void){                                      // "HDC1080_temp_GetValue"  
    float    tempDataAnz;                                               // definiert "tempDataAnz"
    
    HDC1080_Read();                                                     // ruft "HDC1080_Read" auf 
    tempDataAnz = (readBuffer[0] << 8) + readBuffer[1];                 //       
    tempDataAnz = tempDataAnz * 165 / 65536 - 40;                       //
    
    return tempDataAnz;                                                 // gibt "tempDataAnz" zur?ck     
}                                                                       // 

float HDC1080_humy_GetValue(void){                                      // "HDC1080_humy_GetValue" 
    float    humyDataAnz;                                               // definiert "humyDataAnz"
    
    HDC1080_Read();                                                     // ruft "HDC1080_Read" auf 
    humyDataAnz = (readBuffer[2] << 8) + readBuffer[3];                 //    
    humyDataAnz = humyDataAnz * 100 / 65536;                            //
    
    return humyDataAnz;                                                 // gibt "humyDataAnz" zur?ck    
}                                                                       // 

void i2c1_message_pending_100(void){                                    // "i2c1_message_pending_100"
    retryTimeOut1 = 0;                                                  // setzt "retryTimeOut1" auf 0
    retryTimeOut2 = 0;                                                  // setzt "retryTimeOut2" auf 0
    while(status == I2C1_MESSAGE_PENDING){                              // solange "status" gleich "I2C1_MESSAGE_PENDING" ist
        if (retryTimeOut2 == 1000){                                      // wenn "retryTimeOut2" gleich 100 ist
            break;                                                      // Abbruch der Schleife
        }else{                                                          // sonst
            retryTimeOut2++;                                            // "retryTimeOut2" +1
        }                                                               // 
    }
    while(status != I2C1_MESSAGE_FAIL){                                 // solange "status" nicht gleich "I2C1_MESSAGE_FAIL" ist
        if (retryTimeOut1 == 1000){                                      // wenn "retryTimeOut1" gleich 100 ist
            break;                                                      // Abbruch der Schleife
        }else{                                                          // sonst
            retryTimeOut1++;                                            // "retryTimeOut1" +1
        }                                                               // 
    }                                                                   // 
}                                                                       // 