
/** @file globalVariables.h
 *  @brief     Global variables to be used in main.cpp, threads.h and helpFunctions.h.
 *  @author    Aleskei Karavaev
 *  @version   0.0.1
 *  @date      2019/11/3
 *  @bug
 */
#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <events/mbed_events.h>
#include <mbed.h>
#include "ble/BLE.h"
#include "ble/Gap.h"

Thread boardIsAliveThread(osPriorityLow,1*1024,NULL,"is alive thread");
Thread fillBufferThread(osPriorityLow,1*1024,NULL,"fill buffer thread");

static events::EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);
Mutex m;

#endif