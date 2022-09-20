/** @file threads.h
 *  @brief     Threads.
 *  @author    Aleskei Karavaev
 *  @version   0.0.1
 *  @date      2019/11/3
 *  @bug
 */

#ifndef THREADS_H
#define THREADS_H
#include "globalVariables.h"
#include "platform/CircularBuffer.h"

float value[1];
void fillBuffer(void){
    AnalogIn ai0(P0_2);

    float val;
    while(1){
        value[0] = ai0.read();

        ThisThread::sleep_for(100ms);
    }
}

void boardIsAlive(void){
    DigitalOut isAliveLed(P0_14);
    while(1){
        isAliveLed=1;
        ThisThread::sleep_for(100ms);
        isAliveLed=0;
        ThisThread::sleep_for(100ms);
        }
}

#endif