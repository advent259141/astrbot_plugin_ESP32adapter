#ifndef CONFIG_H
#define CONFIG_H

// WiFi配置
const char* WIFI_SSID = "jamyoung";
const char* WIFI_PASSWORD = "259259259";

// WebSocket服务器配置
const char* WEBSOCKET_SERVER = "192.168.137.1";  // 替换为运行AstrBot的电脑IP地址
const int WEBSOCKET_PORT = 8765;

// OLED显示屏配置
#define SCREEN_WIDTH 128 // OLED显示屏宽度，单位像素
#define SCREEN_HEIGHT 64 // OLED显示屏高度，单位像素
#define OLED_RESET    -1 // Reset引脚（-1表示共享Arduino的reset引脚）
#define SCREEN_ADDRESS 0x3C // OLED I2C地址，常见地址为0x3C或0x3D

#define I2C_SDA 18  // SDA引脚
#define I2C_SCL 17  // SCL引脚

// 引脚配置
#define LED_PIN 42
#define SERVO_LEFT_PIN 39   // 左腿舵机引脚（两条左腿并联）
#define SERVO_RIGHT_PIN 38  // 右腿舵机引脚（两条右腿并联）

// 舵机角度范围配置
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180
#define SERVO_CENTER_ANGLE 90

// 步行参数配置
#define WALK_STEP_DELAY 500    // 步行动作间隔时间（毫秒）
#define LEG_MOVE_DELAY 50      // 腿部移动平滑延迟（毫秒）

// 设备配置
#define DEVICE_ID "esp32s3_001"

// 时间配置
#define HEARTBEAT_INTERVAL 30000  // 心跳间隔（毫秒）

#endif
