#ifndef VIPER_BLE_SERVICE_H
#define VIPER_BLE_SERVICE_H
#if !defined(LITE_VERSION)
#include <NimBLEServer.h>

class ViperBLEService {
protected:
    NimBLEService *pService = nullptr;
    uint16_t mtu = 23; // default MTU size
public:
    virtual ~ViperBLEService() = default;
    virtual void setup(NimBLEServer *pServer) = 0;
    virtual void end() = 0;
    void setMTU(int new_mtu) { mtu = new_mtu; }
};
#endif
#endif // VIPER_BLE_SERVICE_H
