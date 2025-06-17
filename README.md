# ESP32S3 WebSocket控制器插件

这是一个为AstrBot开发的插件，用于与ESP32S3设备建立WebSocket连接，实现双向通信和消息转发功能。

## 功能特性

- 🔗 **WebSocket服务器**: 在AstrBot中启动WebSocket服务器，监听ESP32设备连接
- 📱 **消息转发**: 将平台收到的所有消息实时转发给连接的ESP32设备
- 🎮 **设备控制**: 通过指令向ESP32设备发送控制命令
- 📊 **状态监控**: 实时监控ESP32设备连接状态和传感器数据
- 💓 **心跳机制**: 保持连接稳定性，自动处理断线重连

## 安装步骤

1. 将本插件文件夹复制到AstrBot的 `data/plugins/` 目录下
2. 重启AstrBot或在管理面板中重载插件
3. 插件会自动启动WebSocket服务器（默认端口8765）

## 使用方法

### AstrBot端指令

| 指令 | 说明 | 示例 |
|------|------|------|
| `/esp32` | 查看ESP32设备连接状态 | `/esp32` |
| `/esp32_status` | 详细的连接状态信息 | `/esp32_status` |
| `/esp32_send <消息>` | 向ESP32设备发送自定义消息 | `/esp32_send led_on` |

### ESP32端开发

#### 硬件要求
- ESP32S3开发板
- 支持WiFi连接

#### 软件依赖
在Arduino IDE中安装以下库：
- `WiFi` (ESP32自带)
- `ArduinoWebsockets` by Gil Maimon
- `ArduinoJson` by Benoit Blanchon

或者如果使用PlatformIO，在`platformio.ini`中添加：
```ini
lib_deps = 
    bblanchon/ArduinoJson@^7.4.1
    gilmaimon/ArduinoWebsockets@^0.5.4
```

#### 示例代码
参考 `esp32_example.ino` 文件，该文件包含：
- WiFi连接配置
- WebSocket客户端初始化
- 消息处理函数
- 传感器数据发送
- 心跳机制实现

## 配置说明

### WebSocket服务器配置
默认配置：
- 主机: `0.0.0.0` (监听所有网络接口)
- 端口: `8765`

如需修改，请编辑 `main.py` 文件中的相关配置：

```python
self.server_host = "0.0.0.0"
self.server_port = 8765
```

### ESP32连接配置
在ESP32代码中修改以下配置：

```cpp
// WiFi配置
const char* ssid = "你的WiFi名称";
const char* password = "你的WiFi密码";

// WebSocket服务器配置
const char* websocket_server = "192.168.1.100";  // AstrBot所在电脑的IP地址
const int websocket_port = 8765;
```

## 消息协议

### ESP32发送给AstrBot的消息格式

#### 状态更新
```json
{
  "type": "status",
  "status": "connected",
  "device_id": "esp32s3_001",
  "timestamp": 12345
}
```

#### 传感器数据
```json
{
  "type": "sensor_data",
  "device_id": "esp32s3_001",
  "data": {
    "temperature": 25.6,
    "humidity": 45.2,
    "light": 512
  },
  "timestamp": 12345
}
```

#### 心跳消息
```json
{
  "type": "heartbeat",
  "device_id": "esp32s3_001",
  "timestamp": 12345
}
```

### AstrBot发送给ESP32的消息格式

#### 平台消息转发
```json
{
  "type": "astrbot_message",
  "platform": "qq",
  "sender_id": "123456789",
  "sender_name": "用户名",
  "message_text": "开灯",
  "message_type": "GroupMessage",
  "group_id": "987654321",
  "timestamp": 12345,
  "is_private": false,
  "is_admin": false,
  "components": [
    {
      "type": "text",
      "content": "开灯"
    }
  ]
}
```

#### 自定义命令
```json
{
  "type": "custom_command",
  "command": "led_on",
  "from_user": "管理员",
  "timestamp": 12345
}
```

## 应用场景

1. **智能家居控制**: 通过聊天软件控制ESP32连接的智能设备
2. **传感器监控**: 实时获取ESP32传感器数据并推送到聊天群
3. **远程设备管理**: 通过聊天指令远程重启、配置ESP32设备
4. **物联网数据收集**: 将ESP32设备数据汇聚到AstrBot进行处理

## 故障排除

### 常见问题

1. **ESP32无法连接WebSocket服务器**
   - 检查WiFi连接是否正常
   - 确认AstrBot所在电脑的IP地址和端口
   - 检查防火墙设置是否阻止了8765端口

2. **消息无法正常转发**
   - 确认插件已正确加载
   - 检查WebSocket连接状态
   - 查看AstrBot日志中的错误信息

3. **设备频繁断线**
   - 检查WiFi信号强度
   - 调整心跳间隔时间
   - 确认网络稳定性

### 调试方法

1. 查看AstrBot日志：关注ESP32相关的日志输出
2. 使用串口监视器：查看ESP32的调试信息
3. 网络工具：使用websocket客户端工具测试连接

## 开发扩展

### 添加新的消息类型
在 `handle_esp32_message` 方法中添加新的消息类型处理：

```python
elif message_type == "your_custom_type":
    # 处理自定义消息类型
    custom_data = data.get("custom_data", {})
    # 你的处理逻辑
```

### 增加新的控制指令
添加新的command装饰器方法：

```python
@filter.command("your_command")
async def your_command(self, event: AstrMessageEvent, param: str):
    # 你的指令处理逻辑
    pass
```

## 许可证

本插件基于MIT许可证开源，欢迎贡献代码和提出改进建议。

## 更新日志

### v1.0.0 (2024-06-10)
- 初始版本发布
- 实现基础WebSocket服务器功能
- 支持消息转发和设备控制
- 提供完整的ESP32示例代码
