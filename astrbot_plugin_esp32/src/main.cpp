#include <WiFi.h>
#include "config.h"
#include "led_controller.h"
#include "servo_controller.h"
#include "oled_display.h"
#include "websocket_client.h"
#include "message_handler.h"

// 创建模块对象
LedController ledController(LED_PIN);
ServoController servoController;  // 不再需要构造函数参数
OledDisplay oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, I2C_SDA, I2C_SCL, SCREEN_ADDRESS);
WebSocketClientManager wsClient(WEBSOCKET_SERVER, WEBSOCKET_PORT, DEVICE_ID, HEARTBEAT_INTERVAL);
MessageHandler messageHandler(&ledController, &servoController, &oledDisplay, &wsClient);

// 回调函数
void onWebSocketMessage(String message) {
    messageHandler.handleMessage(message);
}

void onWebSocketConnection(bool connected) {
    if (connected) {
        Serial.println("WebSocket连接成功!");
    } else {
        Serial.println("WebSocket连接断开!");
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("ESP32S3 启动中...");
    
    // 初始化各个模块
    Serial.println("初始化LED控制器...");
    ledController.init();
      Serial.println("初始化舵机腿部控制器...");
    servoController.init();  // 使用默认引脚39和38
    
    Serial.println("初始化OLED显示屏...");
    if (!oledDisplay.init()) {
        Serial.println("OLED初始化失败，继续运行但没有显示功能");
    }
    
    // 连接WiFi
    Serial.println("连接WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("正在连接WiFi...");
    }
    Serial.println("WiFi连接成功!");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP());
    
    // 设置WebSocket回调函数
    wsClient.setMessageCallback(onWebSocketMessage);
    wsClient.setConnectionCallback(onWebSocketConnection);
    
    // 初始化WebSocket客户端
    Serial.println("初始化WebSocket客户端...");
    wsClient.begin();
    
    Serial.println("系统初始化完成!");
}

void loop() {
    // 处理WebSocket通信
    wsClient.loop();
    
    // 检查WiFi连接状态
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi连接丢失，尝试重连...");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("重连WiFi中...");
        }
        Serial.println("WiFi重连成功!");
    }
    
    delay(100);
}
