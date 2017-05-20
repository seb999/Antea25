#include <pic16f1709.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

int screenTimer = 70;
bool alarmOn = false;
bool imgSetup=true;
bool imgCancel=false;
bool imgValidate=false;
bool flightMode = false;
bool isPhoneNumber = false;

#include "header.h"
#include "misc.h"
#include "uart.h"
#include "spi.h"
#include "oled.h"
#include "gsm.h"
#include "adxl.h"
#include "rc522.h"

void SetAlarmOn();
void SetAlarmOff();
void SettingMenu();
void Sleep();
void Sleep2();
void CheckSW1();
void CheckSW2();
void Reset();
void RaiseAlarm();
void ResetScreenTimer();
void WakeUp();

void main() {
    //------Init Hardware------------
    SWDTEN = 1;
    OSC_Init();
    PIN_Init();
    INTER_Init();
    UART_Init();
    SPI_Init();   
    Oled_Init();
    ACC_Init(); 
    //RFID_Init(); 
    CLRWDT(); 
    
    
    //---------Debug RFID--------------------
//    ShowMessage("RFID@ON@", 3);
//    GIE = 1;
//    __delay_ms(100); //time to stabilized interruption
//    while(1){
//        CLRWDT(); 
//        if(RFID_Ok()) {
//        }
//
//        if(SW1==0){
//            RfidOff();
//            ShowMessage("RFID@OFF", 3);
//        }
//        
//        if(SW2==0){
//            RfidOn();
//            ShowMessage("RFID@ON@", 3);
//        }
//    }
        

    SendUartCmd("AT\n");
    __delay_sec(1);
    GsmOn();
    __delay_sec(1);
    SimCard_Init();
    
    //---------GPS-------------
//    SWDTEN = 0;
//    ShowMessage("GPS@TEST", 3);
//    //GIE = 1;
//    __delay_ms(100); //time to stabilized interruption
//    while(1){
//        
//    }
  
    //------Welcome message------------  
    
    ShowMessage("WELCOME", 3);
    Left_HorizontalScroll(3, 4, 4);
     __delay_sec(4); 
    Deactivate_Scroll();
    Oled_FillScreen(0x00);
    SWDTEN = 1;
    
    //------Check network------------
    while(1){         
        CLRWDT(); 
        if(CheckNetwork()){
            break;  
        }    
        __delay_sec(1);
        ShowMessage("CHECK@NETWORK", 3);
    }
    
    //------Check User phone------------ Valid this with no phone stored
    if(ReadPhoneNumber() != ""){
        isPhoneNumber = true;
    }
        
    //------Life start here------------ 
    GIE = 1;
    SetAlarmOff();
    CheckSW1();
    while(1){ 
        CLRWDT(); 
        
        //Timer interruption
        if(TMR0IF){
            __delay_ms(150);
            CheckBattery();
            if(!flightMode) CheckNetwork(); 
            Sleep(); 
            TMR0IF = 0;
//            if(RFID_Ok()){
//                alarmOn = !alarmOn;
//                if(alarmOn)SetAlarmOn();
//                else SetAlarmOff();
//                __delay_ms(150);
//            }
        }
        
        //Accelerometer interruption
        if(IOCCF1){
            __delay_ms(300);
            ResetScreenTimer();
            if(alarmOn) RaiseAlarm();
            IOCCF1=0; 
        }

        //SW1 interruption
        if(IOCBF7){ 
            CheckSW1();
            IOCBF7=0; 
        }
 
        //SW2 interruption
        if(IOCBF6){ 
            CheckSW2();
            IOCBF6=0; 
        }
    }
}
//--------------MAIN METHODS LIBRARY------------------ 
void ResetScreenTimer(){
    screenTimer=70;
}

void Sleep(){
    screenTimer--;
    if(screenTimer==0){
        ScreenOff();
        GsmOff();
        SWDTEN = 0;
        TMR0IE = 0; //stop timer interruption
        if(!alarmOn){IOCCP1 = 0;}; //stop accelerometer interruption
        SLEEP();
        WakeUp();
        //RfidOff(); 
    }
}

void WakeUp(){
    GsmWakeUp();
    SWDTEN = 1;
    TMR0IE = 1;
    if(!alarmOn){  
        IOCCP1 = 1;
        ScreenOn();
        ShowMessage("WELCOME@@", 3);
        Left_HorizontalScroll(3, 4, 4);
        __delay_sec (4);
        Deactivate_Scroll();
        SetAlarmOff();
        //__delay_ms(300);
        //RfidOn();
        //__delay_ms(300);
        ShowIcon("1", 112, 6);
        imgSetup=true;
    }
}

void CheckSW1(){
    ResetScreenTimer();
    imgSetup=!imgSetup;
    __delay_ms (200);
    if(imgSetup)
        ShowIcon("0", 112, 6);
    else
        ShowIcon("1", 112, 6);
}

void CheckSW2(){
    //Remove with RFID
    alarmOn = !alarmOn;
    if(alarmOn)SetAlarmOn();
    else SetAlarmOff();
    __delay_ms(150);
    
//    if(imgSetup){
//        ResetScreenTimer();
//        ShowMessage("BADGE@@@@", 3);
//        ShowMessage("@@@@@@@@", 5);
//        ShowIcon("4", 112, 6);
//        while(1) {
//            CLRWDT();
//            if(SW1 == 0){
//                imgCancel=!imgCancel;
//                if(!imgCancel)
//                    ShowIcon("4", 112, 6);
//                else 
//                    ShowIcon("5", 112, 6);
//                __delay_ms(200);
//            }
            
//            if(RFID_Ok()) {  ADD for RFID
//                SettingMenu();
//                break;
//            }
            
//            if(SW2 == 0 && imgCancel){
//                imgCancel=false;  
//                Reset();
//                break;
//            }  
//            if(SW2 == 0 && !imgCancel){ //remove for RFID
//                GsmOn();
//                SettingMenu();
//                break;
//            }  
//        }
//    }
}
        
void SettingMenu(){
    Oled_FillScreen(0x00);
    char * phoneNumber = ReadLastSMS();
    ShowIcon("2", 90, 6);
    ShowIcon("4", 112, 6);
    __delay_ms(200);
    while(1){
        CLRWDT();
        //if(SW1 == 0){
        if(IOCBF7){
            imgValidate=!imgValidate;
            if(!imgValidate){
                ShowIcon("3", 90, 6);
                ShowIcon("4", 112, 6);
            }
            else{
                ShowIcon("2", 90, 6);
                ShowIcon("5", 112, 6);
            }
            __delay_ms(200);
        }

        if(SW2 == 0 && imgValidate || strlen(phoneNumber) == 0){
            Reset();
            break;
        } 
        if(!imgValidate && SW2 == 0 ){
            SavePhoneNumber(phoneNumber);
            ShowMessage("SAVED@@@@@",6);
            __delay_sec(2);
            Reset();
            break;
        } 
    }
}

void Reset(){ 
    Oled_FillScreen(0x00);
    SetAlarmOff();
    ShowIcon("1", 112, 6);
    imgSetup=false; 
    if(flightMode) ShowFlightIcon();
}

void SetAlarmOff(){
    alarmOn = false;
    ResetScreenTimer();
    ShowMessage("ALARM@OFF@", 3);
}

void SetAlarmOn(){
    IOCCP1 = 0; //Stop accelerometer interruption
    IOCBF6=0;  //clear SW2 flag otherwise collision
    ResetScreenTimer();
    if(flightMode){
        GsmOn();  
        __delay_ms(800); //needed to give time for gsm to be stable
    }
    int i = 0;
    while(i<21){
        CLRWDT();
        CheckNetwork();
        sprintf(buffer,"%02d",20-i);
        ShowNumber(buffer, 56, 6);
        __delay_ms(800);
        //if(SW1 == 0){
        if(IOCBF6){
            __delay_ms(150);
            SetAlarmOff();
            IOCBF6=0; 
            IOCCP1 = 1;
            return;
        }
        i++;
    }
    
    Bip(4);
    ShowMessage("ALARM@ON@@", 3);
    IOCCP1 = 1;
}

void RaiseAlarm(){
    CLRWDT();
    IOCBF6=0; //clear SW2 flag otherwise collision
    ShowMessage("ALERT@@@@@", 3);
    ScreenOn();
    ResetScreenTimer();
    //RfidOn();
    //__delay_ms(500);
    if(flightMode){
        GsmOn();
        __delay_ms(800);
    }
    int i = 0;
    while(i<11){
        CLRWDT();
        CheckNetwork();
        sprintf(buffer,"%02d",10-i);
        ShowNumber(buffer, 56, 6);
        __delay_ms(800);
        if(IOCBF6){
            __delay_ms(150);
            SetAlarmOff();
            IOCBF6=0; 
            return;
        }
        i++;
    }
    //RfidOff();
    SendSms();
    __delay_sec(3);
    CLRWDT();
    //RfidOn();
    ShowMessage("ALARM@ON@@", 3);
}