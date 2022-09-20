#include "globalVariables.h"
#include "helpFunctions.h"
#include "bleApplication.h"
#include "threads.h"
#include "ServicesAndCharsIds.h"

using namespace std::literals::chrono_literals;
int main()
{
    printf("\nTEST device init\n\r");
    BLE &ble = BLE::Instance();

    ble.onEventsToProcess(schedule_ble_events);
    uint8_t a = MBED_CONF_CORDIO_DESIRED_ATT_MTU;
    char uid0[17];
    sprintf(&uid0[0], "%08x",NRF_FICR->DEVICEID[0]);
    sprintf(&uid0[8], "%08x",NRF_FICR->DEVICEID[1]);
    uid0[16] = 0;

    BleApp demo(ble, event_queue,"Your company","Device model",uid0,"0.0","0.0","0.0",value,UUID_CUSTOM_UUID,UUID_CUSTOM_DATA_CHAR);
    boardIsAliveThread.start(boardIsAlive);
    fillBufferThread.start(fillBuffer);
    demo.start();
    return 0;
}