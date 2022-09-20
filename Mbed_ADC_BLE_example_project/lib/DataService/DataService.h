/* mbed Microcontroller Library
 * Copyright (c) 2006-2020 ARM Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __BLE_DATA_SERVICE_H__
#define __BLE_DATA_SERVICE_H__

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "ble/GattServer.h"
#include "ServicesAndCharsIds.h"
#include "platform/CircularBuffer.h"

#define SERVICE_MAX_VALUE_BYTES                     251
#define SERVICE_BUFFER_VALUE_BYTES                  248
#define BLE_SERVICE_INPUT_BUFFER_SIZE               468

class DataService {
public:
    uint8_t ctr;
    uint8_t valueBytes[SERVICE_BUFFER_VALUE_BYTES];
    float * value;
    DataService(BLE& _ble,float* value,uint16_t serviceUUID, uint16_t notifyCharacteristicUUID) :
        ble(_ble),
        dataCharacteristic(notifyCharacteristicUUID,(uint8_t *)valueBytes, SERVICE_BUFFER_VALUE_BYTES,SERVICE_BUFFER_VALUE_BYTES,GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY)
    {
        static bool serviceAdded = false; /* We should only ever need to add the information service once. */
        if (serviceAdded) {
            return;
        }

        GattCharacteristic *charTable[] = {&dataCharacteristic};

        GattService gyroaccelerometerService(serviceUUID, charTable, sizeof(charTable) / sizeof(charTable[0]));

        ble.gattServer().addService(gyroaccelerometerService);
        serviceAdded = true;
        ctr=0;
        this->value = value;
    }

    /**
     * @brief   Update humidity characteristic.
     * @param   newHumidityVal New humidity measurement.
     */
    void fromInputBufferToBle()
    {
        std::string val = std::to_string(value[0]);
        int32_t err = ble.gattServer().write(dataCharacteristic.getValueHandle(), (uint8_t *) val.c_str(),val.size());
    }

private:
    BLE& ble;
    GattCharacteristic dataCharacteristic;

protected:

};


#endif /* #ifndef __BLE_ENVIRONMENTAL_SERVICE_H__*/
