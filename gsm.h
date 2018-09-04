#ifndef GSM_H
#define	GSM_H

//AT+COPS? Check network
//AT+CSQ   Get signal level
//AT+CBC   Check battery
//AT+IPR?  Get baud rate
//AT+IPR=0 Put in Auto baud 
void ShowBattIcon(cschar*);
void ShowMessage(cschar*, uchar);
void ShowNumber(cschar *, uchar, uchar);
void ShowReceptionIcon(cschar*);
void ShowFlightIcon();
void ShowIcon(cschar *, uchar, uchar);
void SavePhoneNumber(cschar*);
char * ReadPhoneNumber();
void GsmOff();
void GsmOn();
void ShowBattLoad(cschar *);

void SimCard_Init(){
    char res[15];
    SendUartCmd("AT+CMGF=1\n");//Put in text mode
    UART_Read_Text(res, sizeof(res)/sizeof(char)); ///Is it necessary ??
}

void GsmOff(){
    flightMode = true;
    ShowMessage("@@@@", 0);
    SendUartCmd("AT+CFUN=4\n");
    SendUartCmd("AT+CSCLK=2\n");
    ShowFlightIcon();
}

void GsmWakeUp(){
    SendUartCmd("\n");   
}

void GsmOn(){
    SendUartCmd("AT+CFUN=1\n");
    flightMode = false;
    ShowMessage("@@@@", 0);
}

void CheckBattery(){
    char res[31];
    int batt;
    char * token;
    SendUartCmd("AT+CBC\n");
    UART_Read_Text(res, sizeof(res)/sizeof(char));
    
    token = strtok (res," ,");//READ 1
    if(token == NULL ) return;
    token = strtok (NULL,","); //READ 2
    if(token == NULL ) return;   
    token = strtok (NULL,","); //READ 3
    if(token == NULL ) return;
     
    ShowMessage("@@@@@@@@", 6);
    ShowBattLoad(token);
    batt = atoi(token);
    if(batt>15 && batt<=25) ShowBattIcon("1"); 
    if(batt>25 && batt<=50) ShowBattIcon("2");
    if(batt>50 && batt<=75) ShowBattIcon("3"); 
    if(batt>75 && batt<=100) ShowBattIcon("4"); 
}

bool CheckNetwork(){
    char res[20];
    char * token;
    int netSignal;
    SendUartCmd("AT+CSQ\n");
    UART_Read_Text(res, sizeof(res)/sizeof(char));
    
    token = strtok (res," ,");
    if(token == NULL ){ 
        ShowReceptionIcon("22222");
        return false; 
    }
    token = strtok (NULL,",");
    if(token == NULL ) { 
        ShowReceptionIcon("22222");
        return false; 
    }
    
    netSignal = atoi(token);
    
    if(netSignal==0 || netSignal==99){ 
        ShowReceptionIcon("22222");
        return false; 
    }
    if(netSignal==1) ShowReceptionIcon("12222");
    if(netSignal>1 && netSignal<=5) ShowReceptionIcon("11222");
    if(netSignal>5 && netSignal<=10) ShowReceptionIcon("11122");
    if(netSignal>10 && netSignal<=17) ShowReceptionIcon("11112");
    if(netSignal>17 && netSignal<=31) ShowReceptionIcon("11111");
    return true;
}

char * ReadLastSMS(){
    char res[67]; 
    char * phoneNbr;
    SendUartCmd("AT+CMGL=\"REC UNREAD\",0\n");
    UART_Read_Text(res, sizeof(res)/sizeof(char));  
    strtok (res," ,");
    strtok (NULL,",");
    strtok (NULL,",");
    strtok (NULL,",");
    phoneNbr = strtok (NULL,",");
    if(strlen(phoneNbr) == 0) {
        ShowMessage("NO@SMS", 2);
        __delay_sec(3);
        return "";
    }

    ShowMessage("SAVE@PHONE?", 0);
    if(phoneNbr[1]=='+')
        ShowNumber(phoneNbr+2, 0, 3);
    else ShowNumber(phoneNbr, 0, 3);
    return phoneNbr;
}

void SavePhoneNumber(cschar *myPhone){
    char str1[50], str2[50];
    strcpy(str1,"AT+CPBW=10,");
    strcat(str1, myPhone);
    strcpy(str2, ",129,\"Bike Alarm\"\n");
    strcat(str1, str2);
    SendUartCmd("AT+CPBS=\"SM\"\n");
    SendUartCmd(str1);
}

char * ReadPhoneNumber(){
    char result0[20], result1[50];
    char * phoneNumber;
    SendUartCmd("AT+CPBS=\"SM\"\n");
    UART_Read_Text(result0, sizeof(result0)/sizeof(char));
    SendUartCmd("AT+CPBR=10\n");
    UART_Read_Text(result1, sizeof(result1)/sizeof(char)); 
    strtok (result1," ,");
    strtok (NULL,",");
    phoneNumber = strtok (NULL,",");
    __delay_sec(2);
    if(strlen(phoneNumber)==0){
             ShowMessage("SET@PHONE", 2);
             ShowMessage("NUMBER", 5);
             __delay_sec(2);
            return "";}
    return phoneNumber;
}

void SendSms(){
    char result[10];
    ShowMessage("SMS@SENT", 3);
    char * phoneNumber = ReadPhoneNumber();
    char str1[20];
    strcpy(str1,"AT+CMGS=");
    strcat(str1,phoneNumber);
    strcat(str1, "\n");
    SendUartCmd(str1);
    SendUartCmd("Your attention is required, something is happening!!");
    SendUartCmd("\032"); //ctrl + z
    UART_Read_Text(result, sizeof(result)/sizeof(char)); 
}

//----------------DISPLAY ON OLED---------------------------
void ShowBattIcon(cschar *level){
    Oled_SetFont(Battery8x21, 21, 8, 48, 52);
    Oled_ConstText(level, 107, 0);
    Oled_ConstText("5", 107, 1);
}

void ShowBattLoad(cschar *level){
    strcat(level, "/");
    Oled_SetFont(Battery6x8, 6, 8, 47, 58);
    Oled_ConstText(level, 86, 0);
}

void ShowReceptionIcon(cschar *level){
    Oled_SetFont(Reception8x8, 8, 8, 49, 50);
    Oled_ConstText(level, 0, 0);
}

void ShowMessage(cschar *message, uchar page){
    Oled_SetFont(Terminal12x16, 12, 16, 63, 90);
    Oled_ConstText(message, 0, page);
}

void ShowNumber(cschar *number, uchar seg, uchar page){
    Oled_SetFont(Numeric12x16, 12, 16, 47, 57);
    Oled_ConstText(number, seg, page);
}

void ShowFlightIcon(){
    Oled_SetFont(FlightMode8x12, 16, 16, 48, 48);
    Oled_ConstText("0", 0, 0);
}

void ShowIcon(cschar *id, uchar seg, uchar page){
    Oled_SetFont(Icon16x16, 16, 16, 48, 49);
    Oled_ConstText(id, seg, page);
}

#endif	/* GSM_H */

