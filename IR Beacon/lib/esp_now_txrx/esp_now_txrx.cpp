#include <esp_now_txrx.h>

esp_now_peer_info_t peerInfo;

ESP_NOW_TXRX::ESP_NOW_TXRX(uint8_t* _receiver_address, int _packetSize) {
    memcpy(peerInfo.peer_addr, _receiver_address, 6);
    receiver_address = _receiver_address;
    packetSize = _packetSize;
}

void ESP_NOW_TXRX::getMyMacAddress() {
    USBSerial.println(WiFi.macAddress());
}

void ESP_NOW_TXRX::send(struct_message dat) {
    esp_now_send(receiver_address, (uint8_t *) &dat, packetSize);
}

void ESP_NOW_TXRX::init(esp_now_recv_cb_t receiv_cb) {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        USBSerial.println("Error initializing ESP-NOW");
        return;
    }
    
    esp_now_register_recv_cb(receiv_cb); 
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        USBSerial.println("Failed to add peer");
        return;
    }
}

bool ESP_NOW_TXRX::isDisconnected() {
    return millis() - lastMessage > 500;
}

void ESP_NOW_TXRX::keepAlive() {
    lastMessage = millis();
}