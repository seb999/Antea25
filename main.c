#include <pic16f1709.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

int timer = 0;
int screenTimer;
bool alarmOn = false;
bool imgSetup=false;
bool imgCancel=false;
bool imgValidate=false;
bool flightMode = false;
bool sleepMode = false;

#include "header.h"
#include "misc.h"
#include "uart.h"
#include "spi.h"
#include "gsm.h"
#include "adxl.h"
#include "rc522.h"

void SetAlarmOn();
void SetAlarmOff();
void SettingMenu();
void OnOff();
void CheckSW1();
void CheckSW2();
void Reset();
void RaiseAlarm();
void ResetScreenTimer();

void main() {
    //------Init Hardware------------
    SWDTEN = 1;
    OSC_Init();
    PIN_Init();
    INTER_Init();
    UART_Init();
    SPI_Init();   
    Oled_Init();
    ADLX_Init(); 
    //RFID_Init(); 
    CLRWDT(); 
    
    SendUartCmd("AT\n");
    __delay_sec(1);
    GsmOn();
    __delay_sec(1);
    SimCard_Init();
    
    //------Welcome message------------  
    SWDTEN = 0;
    ResetScreenTimer();
    ShowMessage("WELCOME", 3);
    Left_HorizontalScroll(3, 4, 4);
     __delay_sec(4); //if delay trop long ca crash, why ???
    Deactivate_Scroll();
    Oled_FillScreen(0x00);
    SWDTEN = 0;
    
    //------Check network------------
    while(1){
        CLRWDT(); 
        if(CheckNetwork()){
            break;  
        }    
        __delay_sec(1);
        ShowMessage("CHECK@NETWORK", 3);
    }
    
    //------Check User phone------------
    while(1){
        CLRWDT(); 
        CheckSW1();
        CheckSW2();
        OnOff();
        char * phoneNumber = ReadPhoneNumber();
        if(phoneNumber!=""){
            Bip(1,100);
            Reset();
            break;  
        }         
    }

//Debug RFID
//    ShowMessage("RFID@CHECK", 3);
//while(1){
//    CLRWDT(); 
//    if(RFID_Ok()) {
//        Bip(1,100);
//        ShowMessage("RFID@OK", 3);
//        __delay_sec(1);
//        ShowMessage("@@@@@@@", 3);
//        break;
//    }
//}
        
    //------Life start here------------ 
    GIE = 1;
    while(1){ 
        CLRWDT(); 
        
        if(TMR0IF)
        {
            timer++;
            if(timer==61){ //every second
                ShowMessage("INTER@@", 3);
                __delay_ms(300);
                ShowMessage("@@@@@@@", 3); 
                if(!sleepMode){
                    CheckBattery();
                    if(!flightMode) CheckNetwork();
                }
                timer=0;
                screenTimer--;
            }
            TMR0IF = 0;
        }
        
        if(IOCCF1)
        {
            ShowMessage("ACCEL@@", 3);
             __delay_ms(300);
             if(!sleepMode)ResetScreenTimer();
             //ShowMessage("@@@@@", 3);
            IOCCF1=0; 
        }
        
        if(!sleepMode){
//            if(screenTimer%2==0){
//                CheckBattery();
//                if(!flightMode) CheckNetwork();
//            }
            
            //if(RFID_Ok()){
            if(SW2==0){
                if(flightMode){
                    GsmOn();
                }
                alarmOn = !alarmOn;
                if(alarmOn)SetAlarmOn();
                else SetAlarmOff();
                __delay_ms(300);
            }
        }
        
        CheckSW1();
        CheckSW2();
        OnOff();
        
        if(ADXL_INT2 == 1 && alarmOn){
            RaiseAlarm();
        }
    }
}

//void interrupt inter(void) // Interrupt function definition
//{
//    if(TMR0IF)
//    {
//        timer++;
//        if(timer==31){ //every second
//            timer=0;
//            CheckBattery();
//            if(!flightMode) CheckNetwork();
//        }
//        TMR0IF = 0;
//    }
//    
//    if(IOCCF1)
//    {
//        ShowMessage("PIN@@", 3);
//         __delay_ms(500);
//         ShowMessage("@@@@@", 3);
//        IOCCF1=0; 
//    }
//}
//--------------MAIN METHODS LIBRARY------------------ 
void ResetScreenTimer(){
    screenTimer=20;
}

void OnOff(){
//    if(ADXL_INT2 == 1 && !sleepMode){
//        ResetScreenTimer();
//    };
    
    if(screenTimer==1){
        ScreenOff();
        sleepMode = true;
        //RfidOff();
        GsmOff();
    }
    //if(screenTimer>=1)screenTimer--;
}

void CheckSW1(){
    if(SW1 == 0){
        if(!alarmOn && sleepMode){
            sleepMode = false;
            ScreenOn();
            GsmWakeUp();
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
        ResetScreenTimer();
        imgSetup=!imgSetup;
        if(imgSetup)
            ShowIcon("0", 112, 6);
        else
            ShowIcon("1", 112, 6);
        __delay_ms(200);
     }
}

void CheckSW2(){
    if(SW2==0 && imgSetup){
        ResetScreenTimer();
        ShowMessage("BADGE@@@@", 3);
        ShowMessage("@@@@@@@@", 5);
        ShowIcon("4", 112, 6);
        while(1) {
            CLRWDT();
            if(SW1 == 0){
                imgCancel=!imgCancel;
                if(!imgCancel)
                    ShowIcon("4", 112, 6);
                else 
                    ShowIcon("5", 112, 6);
                __delay_ms(200);
            }
            
            if(RFID_Ok()) {
                SettingMenu();
                break;
            }
            
            if(SW2 == 0 && imgCancel){
                imgCancel=false;  
                Reset();
                break;
            }   
        }
    }
}
        
void SettingMenu(){
    Oled_FillScreen(0x00);
    char * phoneNumber = ReadLastSMS();
    ShowIcon("2", 90, 6);
    ShowIcon("4", 112, 6);
    __delay_ms(200);
    while(1){
        CLRWDT();
        if(SW1 == 0){
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
    ResetScreenTimer();
    for(int i=0;i<=10;i++)
    {
        CLRWDT();
        CheckNetwork();
        //if(RFID_Ok()){
        if(SW2==0 && !imgSetup){
            SetAlarmOff();
            return;
        }
        sprintf(buffer,"%02d",10-i);
        ShowNumber(buffer, 56, 6);
        __delay_ms(800);
     }
    Bip(4,100);
    ShowMessage("ALARM@ON@@", 3);
}

void RaiseAlarm(){
    GsmWakeUp();
    sleepMode = false;
    ShowMessage("ALERT@@@@@", 3);
    ScreenOn();
    ResetScreenTimer();
    //RfidOn();
    //__delay_ms(500);
    if(flightMode){
        //GsmWakeUp();
        CLRWDT();
        GsmOn();
        //__delay_ms(800);
    }
    for(int i=0;i<=10;i++)
    {
        CLRWDT();
        CheckNetwork();
        //if(RFID_Ok()){
        if(SW2==0){
            SetAlarmOff();
            __delay_ms(300);
            return;
        }
        __delay_ms(800);
     }
    //RfidOff();
    SendSms();
    __delay_sec(3);
    //RfidOn();
    ShowMessage("ALARM@ON@@", 3);
}
