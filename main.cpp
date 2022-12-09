/** Sample program to read temperature and humidity
 *
 * @author Alex Lipford
 * Georgia Institute of Technology
 * ECE 4180 Embeded Systems Design
 * Professor Hamblen
 * 10/19/2014
 *
 * @section LICENSE
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <alexlipford@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 *
 *
 * @section DESCRIPTION
 *
 * Honeywell HTU21D Humidity and Temperature sensor.
 *
 * Datasheet, specs, and information:
 *
 * https://www.sparkfun.com/products/12064
 */
 
#include "mbed.h"
#include <string>
#include <stdlib.h>
#include "HTU21D.h"
#include "uLCD_4DGL.h"
#include "SongPlayer.h"
#include <math.h>
#include "rtos.h"

Serial pc(USBTX, USBRX);

DigitalOut reset(p26);
DigitalOut led1(LED1);
DigitalOut led4(LED4);
Timer t;

AnalogIn CO2(p18);
uLCD_4DGL uLCD(p28,p27,p30); // serial tx, serial rx, reset pin;
HTU21D temphumid(p9, p10); //Temp humid sensor || SDA, SCL
int sample_ftemp;
int sample_ctemp;
int sample_humid;
float sample_co2;

float note[7]= {700.0,0,700.0,0,700.0,0,700.0
                };
float duration[7]= {0.48,0.48,0.48,0.48,0.48,0.48, 0.0
                    };
                   

 
void sensorsthread(void const *args) {
    while (1) {
        sample_ftemp = temphumid.sample_ftemp();
        sample_ctemp = temphumid.sample_ctemp();
        sample_humid = temphumid.sample_humid();
        sample_co2 = CO2.read()*1000;
        Thread::wait(1000);
    } 
}
 

void lcdthread(void const *args) {
    uLCD.background_color(BLACK);
    uLCD.cls();
    uLCD.baudrate(30000); //jack up baud rate to max for fast display
    
    while (1) {         
        uLCD.cls();
        uLCD.locate(0,2);
        uLCD.color(WHITE);
        uLCD.printf("Temperature: %d F\n\r", sample_ftemp);
        uLCD.printf("Temperature: %d C\n\r", sample_ctemp);
        uLCD.printf("Humidity: %d %%\n\r", sample_humid);
        uLCD.printf("Gas: %0.2f ppm \n\r", sample_co2);
        if (sample_co2 > 80){ uLCD.printf("Evacuate! CO is high!");}
        uLCD.printf("\n\r");
        wait(0.5);
       
        Thread::wait(1000);
    }
}

int main() {
   
    sample_ftemp = temphumid.sample_ftemp();
    sample_ctemp = temphumid.sample_ctemp();
    sample_humid = temphumid.sample_humid();

    pc.printf("Begin Thread\n");
    Thread t1(sensorsthread);
    Thread t3(lcdthread);
   
    SongPlayer mySpeaker(p26);
   
   
    while(1) {
        if (sample_co2 > 80) {
            mySpeaker.PlaySong(note,duration);
            }
        
        Thread::wait(1000);
    }
}

