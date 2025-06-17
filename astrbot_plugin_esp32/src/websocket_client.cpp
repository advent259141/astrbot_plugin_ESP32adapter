#include "websocket_client.h"

WebSocketClientManager::WebSocketClientManager(String host, int port, String id, unsigned long interval)
    : serverHost(host), serverPort(port), deviceId(id), heartbeatInterval(interval),
      lastHeartbeat(0), messageCallback(nullptr), connectionCallback(nullptr) {
}

void WebSocketClientManager::setMessageCallback(void (*callback)(String)) {
    messageCallback = callback;
}

void WebSocketClientManager::setConnectionCallback(void (*callback)(bool)) {
    connectionCallback = callback;
}

void WebSocketClientManager::begin() {
    // 设置WebSocket事件回调
    client.onMessage([this](websockets::WebsocketsMessage message) {
        this->onMessage(message);
    });
    
    client.onEvent([this](websockets::WebsocketsEvent event, String data) {
        this->onEvent(event, data);
    });
    
    // 首次连接
    reconnect();
}

void WebSocketClientManager::loop() {
    client.poll();
    
    // 检查连接状态，如果断开则重连
    if (!client.available()) {
        Serial.println("WebSocket连接断开，尝试重连...");
        reconnect();
        return;
    }
    
    // 定期发送心跳
    if (millis() - lastHeartbeat > heartbeatInterval) {
        sendHeartbeat();
        lastHeartbeat = millis();
    }
}

bool WebSocketClientManager::isConnected() {
    return client.available();
}

void WebSocketClientManager::sendMessage(String message) {
    if (client.available()) {
        client.send(message);
    } else {
        Serial.println("WebSocket未连接，无法发送消息");
    }
}

void WebSocketClientManager::sendStatusUpdate(String status) {
    JsonDocument doc;
    doc["type"] = "status";
    doc["status"] = status;
    doc["device_id"] = deviceId;
    doc["timestamp"] = millis();
    
    String message;
    serializeJson(doc, message);
    
    if (client.available()) {
        client.send(message);
        Serial.println("发送状态更新: " + status);
    } else {
        Serial.println("WebSocket未连接，无法发送状态更新");
    }
}

void WebSocketClientManager::sendHeartbeat() {
    JsonDocument doc;
    doc["type"] = "heartbeat";
    doc["device_id"] = deviceId;
    doc["timestamp"] = millis();
    
    String message;
    serializeJson(doc, message);
    
    if (client.available()) {
        client.send(message);
        // 心跳发送，静默处理
    } else {
        Serial.println("WebSocket未连接，无法发送心跳");
    }
}

void WebSocketClientManager::onMessage(websockets::WebsocketsMessage message) {
    Serial.printf("收到消息: %s\n", message.data().c_str());
    
    if (messageCallback) {
        messageCallback(message.data());
    }
}

void WebSocketClientManager::onEvent(websockets::WebsocketsEvent event, String data) {
    if (event == websockets::WebsocketsEvent::ConnectionOpened) {
        Serial.println("WebSocket连接已建立");
        if (connectionCallback) {
            connectionCallback(true);
        }
        sendStatusUpdate("connected");
    } else if (event == websockets::WebsocketsEvent::ConnectionClosed) {
        Serial.println("WebSocket连接已关闭");
        if (connectionCallback) {
            connectionCallback(false);
        }
    } else if (event == websockets::WebsocketsEvent::GotPing) {
        Serial.println("收到Ping");
    } else if (event == websockets::WebsocketsEvent::GotPong) {
        Serial.println("收到Pong");
    }
}

void WebSocketClientManager::reconnect() {
    String websocket_url = "ws://" + serverHost + ":" + String(serverPort) + "/";
    Serial.println("正在连接到AstrBot WebSocket服务器: " + websocket_url);
    
    bool connected = client.connect(websocket_url);
    if(connected) {
        Serial.println("WebSocket连接成功!");
        if (connectionCallback) {
            connectionCallback(true);
        }
        sendStatusUpdate("connected");
    } else {
        Serial.println("WebSocket连接失败，5秒后再试...");
        delay(5000);
    }
}
