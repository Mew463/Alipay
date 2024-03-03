#include <esp_now.h>
#include <WiFi.h>


typedef struct struct_message {
    char a[6];
} struct_message; 

class ESP_NOW_TXRX {
    public:
        ESP_NOW_TXRX(uint8_t *_receiver_address, int _packetSize);
        void init(esp_now_recv_cb_t receiv_cb);
        void getMyMacAddress();
        void send(struct_message dat);
        bool isDisconnected();
        void keepAlive();
    private:
        int packetSize;
        uint8_t *receiver_address;
        unsigned long lastMessage = millis();
};