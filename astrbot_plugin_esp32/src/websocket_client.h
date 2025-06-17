#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

class WebSocketClientManager {
private:
    websockets::WebsocketsClient client;
    String serverHost;
    int serverPort;
    String deviceId;
    unsigned long lastHeartbeat;
    unsigned long heartbeatInterval;
    
    // 回调函数指针
    void (*messageCallback)(String message);
    void (*connectionCallback)(bool connected);

public:
    WebSocketClientManager(String host, int port, String id, unsigned long interval = 30000);
    void setMessageCallback(void (*callback)(String));
    void setConnectionCallback(void (*callback)(bool));
    void begin();
    void loop();
    bool isConnected();
    void sendMessage(String message);
    void sendStatusUpdate(String status);
    void sendHeartbeat();
    
private:
    void onMessage(websockets::WebsocketsMessage message);
    void onEvent(websockets::WebsocketsEvent event, String data);
    void reconnect();
};

#endif
