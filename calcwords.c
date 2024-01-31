#include <wiringPi.h>
#include <stdio.h>
#include <wiringShift.h>
#include <signal.h>
#include <unistd.h>

const int dataPin = 5;  //DS Pin of 74HC595(Pin14)
const int latchPin = 4; //ST_CP Pin of 74HC595(Pin12)
const int clockPin = 1; //CH_CP Pin of 74HC595(Pin11)
const int digitPin[]={0,2,3,12};        // Define 7-segment display common pin
const int num_displays = 4;
const int display_index[]= {0x01, 0x02, 0x04, 0x08};

// character 0-9 code of common anode 7-segment display 
// 0    1    2    3    4    5    6    7    8    9
// 0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90
unsigned char words[][4] = {
        {0xf8, 0xb0, 0x82, 0xc0}, //lego
        {0xf8, 0xc0, 0x82, 0xc0}, //logo
        {0xb0, 0x82, 0x82, 0x92}, //eggs
        {0xc0, 0x99, 0xf9, 0xc0}, //Ohio
        {0x80, 0xc0, 0x92, 0x92}, //boss
        {0x92, 0x99, 0xc0, 0xb0}, //shoe
    };
int counter = 0;// keeps track of which word to display

//Open one of the 7-segment display and close the remaining three, the parameter digit is optional for 1,2,4,8
void selectDigit(int digit){  
    for(int i=0;i<4;i++){
        digitalWrite(digitPin[i],(digit == display_index[num_displays-1-i]) ? LOW : HIGH);
    }
}
void _shiftOut(int dPin,int cPin,int order,int val){     
    for(int i=0; i < 8; i++){
        digitalWrite(cPin,LOW);
        if(order == LSBFIRST){
            digitalWrite(dPin,((0x01&(val>>i)) == 0x01) ? HIGH : LOW);
            delayMicroseconds(1);
		}
        else {//if(order == MSBFIRST){
            digitalWrite(dPin,((0x80&(val<<i)) == 0x80) ? HIGH : LOW);
            delayMicroseconds(1);
		}
        digitalWrite(cPin,HIGH);
        delayMicroseconds(1);
	}
}
void outData(int8_t data){      //function used to output data for 74HC595
    digitalWrite(latchPin,LOW);
    _shiftOut(dataPin,clockPin,MSBFIRST,data);
    digitalWrite(latchPin,HIGH);
}
void output_words(int dec){
    
    int num_words = sizeof(words) / sizeof(words[0]);
    
    //select nth 7-segment display, and display nth letter
    for(int i=0;i<num_displays;i++){
        selectDigit(display_index[i]);
        outData(words[dec%num_words][i]);
        delay(1); 
    }
}
void timer(int  sig){       //Timer function
    if(sig == SIGALRM){   //If the signal is SIGALRM, the value of counter plus 1, and update the number displayed by 7-segment display
        counter++;         
        alarm(2);           //set the next timer time
        printf("counter : %d \n",counter);
        
    }
}
int main(void)
{
    
    printf("Let's print some calculator words!\n");
    
    wiringPiSetup();

    //set the pin connected to 74HC595 for output mode
    pinMode(dataPin,OUTPUT);  
    pinMode(latchPin,OUTPUT);
    pinMode(clockPin,OUTPUT);
    //set the pin connected to 7-segment display common end to output mode
    for(int i=0;i<4;i++){       
        pinMode(digitPin[i],OUTPUT);
        digitalWrite(digitPin[i],HIGH);
    }
    signal(SIGALRM,timer);  //configure the timer
    alarm(2);               //set the time of timer to 1s
    while(1){
        output_words(counter);   //display the number counter
    }
    return 0;
}


