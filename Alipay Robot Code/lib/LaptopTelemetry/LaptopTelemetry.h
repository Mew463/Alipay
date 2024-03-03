#ifdef IR_BEACON
    #define PORT 12347
#else
    #define PORT 12346
#endif

#include <WiFiUdp.h>
#include <WiFi.h>


class LaptopTelemetry {
    public: 
        LaptopTelemetry(const char* _ssid, const char* _pswrd, char* _packetBuffer);
        void receive();
        void init();
        void send(const char* message);
        void send(float value);
        bool isDisconnected();
    private:
        const char* ssid;
        const char* pswrd;
        char* packetBuffer;
        unsigned long lastTransmission = millis();

};
