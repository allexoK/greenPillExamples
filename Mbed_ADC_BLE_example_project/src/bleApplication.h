#include "globalVariables.h"
#include "DataService.h"
#include "DeviceInformationService.h"


const static char DEVICE_NAME[] = "PROKYBER GREENPILL";

class BleApp : 
    ble::Gap::EventHandler,
    public SecurityManager::EventHandler {
public:
    uint8_t ctr;
    BleApp(
        BLE &ble,
        events::EventQueue &event_queue,
        const char *manufacturersName,
        const char *modelNumber,
        const char *serialNumber,
        const char *hardwareRevision,
        const char *firmwareRevision,
        const char *softwareRevision,
        float* cb,
        uint16_t data_service_uuid,
        uint16_t data_char_uuid
        ) :
        _ble(ble),
        _event_queue(event_queue),
        _dataservice_uuid(data_service_uuid),
        _dis_uuid(GattService::UUID_DEVICE_INFORMATION_SERVICE),
        _data_service(ble,cb,data_service_uuid,data_char_uuid),
        _dis(ble,manufacturersName,modelNumber,serialNumber,hardwareRevision,firmwareRevision,softwareRevision),
        _adv_data_builder(_adv_buffer)
    {
        ctr=0;
        clientConnectfb = NULL;
    }

    void start()
    {
        _ble.init(this, &BleApp::on_init_complete);
        _event_queue.dispatch_forever();
    }

    void onClientConnect(void (*clientConnectfb) (void)){
        this->clientConnectfb=clientConnectfb;

    }

private:
    void (*clientConnectfb) (void);
    void (*sendorUpdate) (void);

    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params)
    {
        if (params->error != BLE_ERROR_NONE) {
            printf("Ble initialization failed.");
            return;
        }
         
        const char* db_path = "/fs/bt_sec_db";

        ble_error_t err = _ble.securityManager().init(
                  /* enableBonding */ true,
                  /* requireMITM */ false,
                  /* iocaps */ SecurityManager::IO_CAPS_NONE,
                  /* passkey */ nullptr,
                  /* signing */ false,
                  /* dbFilepath */ db_path
              );


        err = _ble.securityManager().preserveBondingStateOnReset(true);
 
        if (err) {
            printf("Error during preserveBondingStateOnReset %d\r\n", err);
        }

       _ble.securityManager().setSecurityManagerEventHandler(this);
 
        /* this allows us to receive events like onConnectionComplete() */
        _ble.gap().setEventHandler(this);

        ble::phy_set_t tx_phys(/* 1M */ false, /* 2M */ true, /* coded */ false);
        ble::phy_set_t rx_phys(/* 1M */ false, /* 2M */ true, /* coded */ false);
        err = _ble.gap().setPreferredPhys(&tx_phys, &rx_phys);
        if (err) {
            printf("INFO: GAP::setPreferedPhys failed with error code %s", BLE::errorToString(err));
        }

        /* heart rate value updated every second */
        _event_queue.call_every(
            100ms,
            [this] {
                update_sensor_value();
            }
        );

        start_advertising();
    }

    void start_advertising()
    {
        /* Create advertising parameters and payload */
        ble::AdvertisingParameters adv_parameters(
            ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(100))
        );

        _adv_data_builder.setFlags();
        _adv_data_builder.setAppearance(ble::adv_data_appearance_t::UNKNOWN);
        _adv_data_builder.setLocalServiceList({&_dataservice_uuid, 1});
        _adv_data_builder.setLocalServiceList({&_dis_uuid,2});
        _adv_data_builder.setLocalServiceList({&_adis_uuid,3});
        _adv_data_builder.setName(DEVICE_NAME);
        // _adv_data_builder.setAdvertisingInterval((ble::adv_interval_t )0x20);

        /* Setup advertising */

        ble_error_t error = _ble.gap().setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            adv_parameters
        );

        if (error) {
            printf("_ble.gap().setAdvertisingParameters() failed\r\n");
            return;
        }

        error = _ble.gap().setAdvertisingPayload(ble::LEGACY_ADVERTISING_HANDLE, _adv_data_builder.getAdvertisingData());

        if (error) {
            printf("_ble.gap().setAdvertisingPayload() failed\r\n");
            return;
        }

        error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

        if (error) {
            printf("_ble.gap().startAdvertising() failed\r\n");
            return;
        }

       _ble.securityManager().setPairingRequestAuthorisation(false);
        printf("Device advertising, please connect\r\n");
    }

    void update_sensor_value()
    {
        _data_service.fromInputBufferToBle();
    }
private:
    /* when we connect we stop advertising, restart advertising so others can connect */
    virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event)
    {
        if (event.getStatus() == ble_error_t::BLE_ERROR_NONE) {
            printf("Client connected, you may now subscribe to updates\r\n");
        }
    }

    /* when we connect we stop advertising, restart advertising so others can connect */
    virtual void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event)
    {
        printf("Client disconnected, restarting advertising\r\n");

        ble_error_t error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

        if (error) {
            printf("_ble.gap().startAdvertising() failed\r\n");
            return;
        }
    }
    
    virtual void pairingRequest(
        ble::connection_handle_t connectionHandle
    ) {
        printf("Pairing requested - authorising\r\n");
        _ble.securityManager().acceptPairingRequest(connectionHandle);
    }
 
    /** Inform the application of a successful pairing. Terminate the demonstration. */
    virtual void pairingResult(
        ble::connection_handle_t connectionHandle,
        SecurityManager::SecurityCompletionStatus_t result
    ) {
        if (result == SecurityManager::SEC_STATUS_SUCCESS) {
            printf("Pairing successful\r\n");
        } else {
            printf("Pairing failed\r\n");
        }
    }
 
    void linkEncryptionResult(ble::connection_handle_t connectionHandle, ble::link_encryption_t result) override
       {
           if (result == ble::link_encryption_t::ENCRYPTED) {
               printf("Link ENCRYPTED\r\n");
           } else if (result == ble::link_encryption_t::ENCRYPTED_WITH_MITM) {
               printf("Link ENCRYPTED_WITH_MITM\r\n");
           } else if (result == ble::link_encryption_t::NOT_ENCRYPTED) {
               printf("Link NOT_ENCRYPTED\r\n");
           }
       }


private:
    BLE &_ble;
    events::EventQueue &_event_queue;

    UUID _dataservice_uuid;
    UUID _dis_uuid;
    UUID _adis_uuid;

    DataService _data_service;
    DeviceInformationService _dis;

    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;


};
