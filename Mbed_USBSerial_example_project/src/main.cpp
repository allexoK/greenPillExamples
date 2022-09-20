#include "mbed.h"
#include "USBSerial.h"
int main()
{
    USBSerial usb;
    usb.connect();
    while(1){
        usb.send((uint8_t*)"Hello World!!!\n\r",17);
        ThisThread::sleep_for(1000ms);
        }
    return 0;
}