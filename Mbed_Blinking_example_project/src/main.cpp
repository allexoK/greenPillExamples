#include "mbed.h"
int main()
{
    DigitalOut isAliveLed(P0_14);
    while(1){
        isAliveLed=1;
        ThisThread::sleep_for(100ms);
        isAliveLed=0;
        ThisThread::sleep_for(100ms);
        }
    return 0;
}