/* // Original main()

#include <mbed.h>

int main() {

    // put your setup code here, to run once:

    while(1) {
        // put your main code here, to run repeatedly:
    }
} */

#include "mbed.h"
#include "Dht11.h"
#include "DS1820.h"
#include <stdio.h>
#include <iostream>
#include <string>

DigitalOut led1(LED1), led2(LED2), led3(LED3), led4(LED4);
Serial pc(USBTX, USBRX);
Serial Pi(p13, p14);
DigitalIn cts(p12);
DigitalOut rts(p11);
Dht11 TempHumid(p8);
DS1820 ds1820(p29);
AnalogIn PhotoRes(p20);
Timer timer;

void light();
void dht();
void digitemp();

int InitHour = 0;
int InitMinute = 0;
int TimeCtl = 0;
int MinLast = 0;
int seconds = 0;
int hour = 0;
int minutes = 0;

// main() runs in its own thread in the OS
int main() {
    pc.printf("Starting.\r\n");
    pc.printf("Enter current time in the following format.\r\n");
    pc.printf("[Hour] [minute]\r\n");
    pc.scanf("%d %d", &InitHour, &InitMinute);
    timer.start();
    while(1) {
        seconds = timer.read();
        hour = (seconds/3600) + InitHour + TimeCtl;
        minutes = (((seconds%3600)/60) + InitMinute)%60;
        int secondsover = (seconds%60);
        
        if (TimeCtl == 0 && MinLast > minutes) {
            TimeCtl = 1;
            hour = InitHour + 1;
        }
        
        pc.printf("Current time:\t%02d:%02d:%02d\r\n", hour, minutes, secondsover);
        MinLast = minutes;
        light();
        dht();
        digitemp();
        wait(10);
    }
}

void light()
{
    float light = PhotoRes;
    pc.printf("Lightvalue is:\t%f\r\n", light);
}

void dht()
{
    TempHumid.read();
    float dhttemp = TempHumid.getCelsius();
    float dhthumidity = TempHumid.getHumidity();
    pc.printf("Temp:\t%.1fC\tHumidity:\t%.1f%%\r\n", dhttemp, dhthumidity);
}

void digitemp()
{
    float   temp = 0;
    int     error = 0; 
                             
    if(ds1820.begin()) {
        ds1820.startConversion();  // start temperature conversion
        wait(1.0);                 // let DS1820 complete the temperature conversion
        error = ds1820.read(temp); // read temperature from DS1820 and perform cyclic redundancy check (CRC)
        switch(error) {
        case 0:    // no errors -> 'temp' contains the value of measured temperature
            pc.printf("temp = %3.1f\r\n", temp);
            break;
        case 1:    // no sensor present -> 'temp' is not updated
            pc.printf("no sensor present\n\r");
            break;
        case 2:    // CRC error -> 'temp' is not updated
            pc.printf("CRC error\r\n");
        } 

    } else
        pc.printf("No DS1820 sensor found!\r\n");
}