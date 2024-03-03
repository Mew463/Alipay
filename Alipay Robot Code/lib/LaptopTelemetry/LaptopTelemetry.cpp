#include "LaptopTelemetry.h"

WiFiUDP udp;
// IPAddress laptopIpAddress(192, 168, 86, 25);
// IPAddress laptopIpAddress(192, 168, 111, 177);
IPAddress laptopIpAddress(192, 168, 175, 127);

LaptopTelemetry::LaptopTelemetry(const char* _ssid, const char* _pswrd, char* _packetBuffer) {
    ssid = _ssid;
    pswrd = _pswrd;
    packetBuffer = _packetBuffer;
}

void LaptopTelemetry::init() {
    WiFi.setMinSecurity(WIFI_AUTH_WEP);
    WiFi.begin(ssid, pswrd);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    wl_status_t wifistat = WiFi.status();
    while (wifistat != WL_CONNECTED) {
        wifistat = WiFi.status();
        switch (wifistat) {
    case WL_NO_SSID_AVAIL:
        USBSerial.println("[WiFi] SSID not found");
        break;
    case WL_CONNECT_FAILED:
        USBSerial.print("[WiFi] Failed - WiFi not connected! Reason: ");
        break;
    case WL_CONNECTION_LOST:
        USBSerial.println("[WiFi] Connection was lost");
        break;
    case WL_CONNECTED:
        USBSerial.println("[WiFi] WiFi is connected!");
        USBSerial.print("[WiFi] IP address: ");
        USBSerial.println(WiFi.localIP());
        udp.begin(12345); // Port that ESP32 will receive commands on 
        break;
    default:
        USBSerial.print(".");
        break;
        }
        delay(100);
    }
}

void LaptopTelemetry::send(const char* message) {
    udp.beginPacket(laptopIpAddress, PORT); // Send to port 
    udp.print(message);
    udp.endPacket();
}

void LaptopTelemetry::send(float value) {
    char conversionbuffer[5];
    sprintf(conversionbuffer, "%.2f", value);
    send(conversionbuffer);
}

bool LaptopTelemetry::isDisconnected() {
    return (millis() - lastTransmission > 500);
}

void LaptopTelemetry::receive() {
    size_t packetSize = udp.parsePacket();
    if (packetSize) { // receive incoming UDP packets
        // USBSerial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
        int len = udp.read(packetBuffer, packetSize);
        if (len > 0)
            packetBuffer[len] = 0;
        lastTransmission = millis();
        // USBSerial.printf("[CONTROLLER]: %s\n", packetBuffer);
    }
}



