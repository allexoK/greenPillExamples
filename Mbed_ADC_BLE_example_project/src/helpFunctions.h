/** @file helpFunctions.h
 *  @brief     Functions to be used in main.cpp and threads.h.
 *  @author    Aleskei Karavaev
 *  @version   0.0.1
 *  @date      2019/11/3
 *  @bug
 */
#ifndef HELP_FUNCTIONS_H
#define HELP_FUNCTIONS_H
#include "globalVariables.h"

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context)
{
    event_queue.call(mbed::Callback<void()>(&context->ble, &BLE::processEvents));
}

#endif