#include "mbed.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(D1, D0, D2);
InterruptIn up(D3);
InterruptIn down(D6);
InterruptIn enter(D5);
AnalogOut Aout(D7);
AnalogIn Ain(A0);

EventQueue btn;
Thread btnThread;

EventQueue wave;
Thread waveThread;

EventQueue ADC;
Thread ADC_Thread;

int cursor = 0;
int output = 0;

void cursor_update(int pos, int select)
{
    if(select == 0)
        uLCD.color(RED);
    else
        uLCD.color(GREEN);
    
    uLCD.locate(0, 0);
    uLCD.printf(" ");
    uLCD.locate(0, 1);
    uLCD.printf(" ");
    uLCD.locate(0, 2);
    uLCD.printf(" ");
    uLCD.locate(0, 3);
    uLCD.printf(" ");
    uLCD.locate(0, pos);
    uLCD.printf("v");
}

void ISR1(){
    if(cursor != 0){
        cursor--;
        btn.call(cursor_update, cursor, 0);
    }
}

void ISR2(){
    if(cursor != 3){
        cursor++;
        btn.call(cursor_update, cursor, 0);
    }
}

void ISR3(){
    btn.call(cursor_update, cursor, 1);
    output = 1;
}

void genWave(){
    float i = 0.0f;
    float step;
    float high = 3.0f / 3.3f;
    float slope_list[] = { high / 80.0f * 6.0f / 400.0f,
                           high / 40.0f,
                           high / 20.0f,
                           high / 10.0f};
    int sleep_list[] = {80, 160, 200, 220};
    while(1){
        if(i >= high){
            i = high;
            step = -slope_list[cursor];
            ThisThread::sleep_for(sleep_list[cursor] * 1ms);
        }
        if(i <= 0.0f){
            i = 0.0f;
            step = slope_list[cursor];
        }
        i += step;
        Aout = i;
        ThisThread::sleep_for(1ms / 20);
    }
}

void sampling(){
    int idx = 0;
    int sample = 1024;
    float ADCdata[1024];
    while(1){
        ADCdata[idx] = Ain;
        if(output == 1){
            idx++;
            if(idx == sample){
                output = 0;
                for(int i = 0; i < sample; i++){
                    printf("%f\n", ADCdata[i]);
                }
            }
        }
        ThisThread::sleep_for(480ms / 1024);
    }
}

int main()
{
    uLCD.color(GREEN);
    uLCD.reset();
    uLCD.locate(2, 0);
    uLCD.printf("  1");
    uLCD.locate(2, 1);
    uLCD.printf("1/2");
    uLCD.locate(2, 2);
    uLCD.printf("1/4");
    uLCD.locate(2, 3);
    uLCD.printf("1/8");
    uLCD.locate(0, 0);
    uLCD.printf("v");

    btnThread.start(callback(&btn, &EventQueue::dispatch_forever));
    up.mode(PullNone);
    down.mode(PullNone);
    enter.mode(PullNone);
    up.rise(ISR1);
    down.rise(ISR2);
    enter.rise(ISR3);

    waveThread.start(callback(&wave, &EventQueue::dispatch_forever));
    wave.call(genWave);
    
    //ADC_Thread.start(callback(&ADC, &EventQueue::dispatch_forever));
    //ADC.call(sampling);
    
    while(1);
}