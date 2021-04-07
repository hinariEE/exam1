#include "mbed.h"
#include "uLCD_4DGL.h"

InterruptIn up(D3);
InterruptIn down(D6);
InterruptIn enter(D5);

uLCD_4DGL uLCD(D1, D0, D2);

int cursor = 0;

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

void ISR1()
{
    if(cursor != 0){
        cursor--;
        cursor_update(cursor, 0);
    }
}

void ISR2()
{
    if(cursor != 3){
        cursor++;
        cursor_update(cursor, 0);
    }
}

void ISR3()
{
    cursor_update(cursor, 1);
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
    up.rise(&ISR1);
    down.rise(&ISR2);
    enter.rise(&ISR3);
    
    while(1);
}