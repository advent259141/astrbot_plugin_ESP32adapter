#include "message_handler.h"

MessageHandler::MessageHandler(LedController* led, ServoController* servo, OledDisplay* oled, WebSocketClientManager* ws)
    : ledController(led), servoController(servo), oledDisplay(oled), wsClient(ws) {
}

void MessageHandler::handleMessage(String message) {
    // 解析JSON消息
    JsonDocument doc;
    deserializeJson(doc, message);
    
    String messageType = doc["type"];
    
    if (messageType == "welcome") {
        handleWelcomeMessage(doc);
    } else if (messageType == "led_control") {
        handleLedControl(doc);
    } else if (messageType == "oled_control") {
        handleOledControl(doc);
    } else if (messageType == "servo_control") {
        handleServoControl(doc);
    } else if (messageType == "astrbot_message") {
        handleAstrBotMessage(doc);
    } else if (messageType == "custom_command") {
        handleCustomCommand(doc);
    } else if (messageType == "heartbeat_ack") {
        handleHeartbeatAck(doc);
    }
}

void MessageHandler::handleWelcomeMessage(JsonDocument& doc) {
    Serial.println("收到欢迎消息: " + doc["message"].as<String>());
}

void MessageHandler::handleAstrBotMessage(JsonDocument& doc) {
    String platform = doc["platform"];
    String senderName = doc["sender_name"];
    String messageText = doc["message_text"];
    bool isPrivate = doc["is_private"];
    
    Serial.println("=== AstrBot消息 ===");
    Serial.println("平台: " + platform);
    Serial.println("发送者: " + senderName);
    Serial.println("消息: " + messageText);
    Serial.println("私聊: " + String(isPrivate ? "是" : "否"));
    Serial.println("==================");
    
    // 处理文本命令
    processTextCommands(messageText);
}

void MessageHandler::handleCustomCommand(JsonDocument& doc) {
    String command = doc["command"];
    String fromUser = doc["from_user"];
    
    Serial.println("收到自定义命令: " + command + " (来自: " + fromUser + ")");
    processCustomCommand(command);
}

void MessageHandler::handleLedControl(JsonDocument& doc) {
    String action = doc["action"];
    int brightness = doc["brightness"] | 100;  // 默认100%亮度
    String fromUser = doc["from_user"];
    
    Serial.println("=== LED控制指令 ===");
    Serial.println("操作: " + action);
    Serial.println("亮度: " + String(brightness) + "%");
    Serial.println("来自用户: " + fromUser);
    Serial.println("==================");
    
    if (action == "on") {
        ledController->setState(true);
        ledController->setBrightness(brightness);
        wsClient->sendStatusUpdate("LED已开启，亮度" + String(brightness) + "%");
    } else if (action == "off") {
        ledController->setState(false);
        wsClient->sendStatusUpdate("LED已关闭");
    } else if (action == "toggle") {
        ledController->toggle();
        if (ledController->getState()) {
            ledController->setBrightness(brightness);
            wsClient->sendStatusUpdate("LED已开启，亮度" + String(brightness) + "%");
        } else {
            wsClient->sendStatusUpdate("LED已关闭");
        }
    } else {
        Serial.println("未知的LED操作: " + action);
        wsClient->sendStatusUpdate("未知的LED操作: " + action);
    }
}

void MessageHandler::handleServoControl(JsonDocument& doc) {
    String action = doc["action"];
    int leftAngle = doc["left_angle"] | 90;   // 左腿角度，默认90度
    int rightAngle = doc["right_angle"] | 90; // 右腿角度，默认90度
    int angle = doc["angle"] | 90;            // 通用角度，用于向后兼容
    String fromUser = doc["from_user"];
    
    Serial.println("=== 舵机腿部控制指令 ===");
    Serial.println("操作: " + action);
    Serial.println("左腿角度: " + String(leftAngle) + "度");
    Serial.println("右腿角度: " + String(rightAngle) + "度");
    Serial.println("来自用户: " + fromUser);
    Serial.println("========================");
    
    if (action == "walk_forward") {
        servoController->walkForward();
        wsClient->sendStatusUpdate("机器人开始前进步态");
    } else if (action == "walk_backward") {
        servoController->walkBackward();
        wsClient->sendStatusUpdate("机器人开始后退步态");  
    } else if (action == "stand_up") {
        servoController->standUp();
        wsClient->sendStatusUpdate("机器人站立完成");
    } else if (action == "stop") {
        servoController->stopWalk();
        wsClient->sendStatusUpdate("机器人停止步行，回到站立位置");
    } else if (action == "left_forward") {
        servoController->leftLegForward();
        wsClient->sendStatusUpdate("左腿前进动作完成");
    } else if (action == "left_backward") {
        servoController->leftLegBackward();
        wsClient->sendStatusUpdate("左腿后退动作完成");
    } else if (action == "right_forward") {
        servoController->rightLegForward();
        wsClient->sendStatusUpdate("右腿前进动作完成");
    } else if (action == "right_backward") {
        servoController->rightLegBackward();
        wsClient->sendStatusUpdate("右腿后退动作完成");
    } else if (action == "move_legs") {
        servoController->moveLegs(leftAngle, rightAngle);
        wsClient->sendStatusUpdate("腿部移动到指定角度：左腿" + String(leftAngle) + "度，右腿" + String(rightAngle) + "度");
    } else if (action == "move_left") {
        servoController->moveLeftLeg(leftAngle);
        wsClient->sendStatusUpdate("左腿移动到" + String(leftAngle) + "度");
    } else if (action == "move_right") {
        servoController->moveRightLeg(rightAngle);
        wsClient->sendStatusUpdate("右腿移动到" + String(rightAngle) + "度");
    } else {
        Serial.println("未知的舵机操作: " + action);
        wsClient->sendStatusUpdate("未知的舵机腿部操作: " + action);
    }
}

void MessageHandler::handleOledControl(JsonDocument& doc) {
    String action = doc["action"];
    String content = doc["content"] | "";
    String fromUser = doc["from_user"];
    
    Serial.println("=== OLED控制指令 ===");
    Serial.println("操作: " + action);
    Serial.println("内容: " + content);
    Serial.println("来自用户: " + fromUser);
    Serial.println("==================");
    
    if (action == "emotion") {
        oledDisplay->displayEmotion(content);
        wsClient->sendStatusUpdate("OLED显示表情: " + content);
    } else if (action == "text") {
        oledDisplay->displayText(content);
        wsClient->sendStatusUpdate("OLED显示文本: " + content);
    } else if (action == "clear") {
        oledDisplay->clear();
        wsClient->sendStatusUpdate("OLED屏幕已清除");
    } else {
        Serial.println("未知的OLED操作: " + action);
        wsClient->sendStatusUpdate("未知的OLED操作: " + action);
    }
}

void MessageHandler::handleHeartbeatAck(JsonDocument& doc) {
    // 心跳确认，静默处理
}

void MessageHandler::processCustomCommand(String command) {
    if (command == "restart") {
        Serial.println("执行重启命令");
        ESP.restart();
    } else if (command == "status") {
        wsClient->sendStatusUpdate("设备运行正常");
    } else if (command.startsWith("led_")) {
        if (command == "led_on") {
            ledController->setState(true);
            wsClient->sendStatusUpdate("LED已开启");
        } else if (command == "led_off") {
            ledController->setState(false);
            wsClient->sendStatusUpdate("LED已关闭");
        }
    } else {
        Serial.println("未知命令: " + command);
    }
}

void MessageHandler::processTextCommands(String messageText) {
    // LED控制命令
    if (messageText.indexOf("亮灯") != -1 || messageText.indexOf("开灯") != -1 || messageText.indexOf("点亮") != -1) {
        Serial.println("执行点亮LED操作");
        ledController->setState(true);
        wsClient->sendStatusUpdate("LED已点亮");
    } else if (messageText.indexOf("关灯") != -1 || messageText.indexOf("熄灭") != -1 || messageText.indexOf("关闭") != -1) {
        Serial.println("执行关闭LED操作");
        ledController->setState(false);
        wsClient->sendStatusUpdate("LED已关闭");
    } else if (messageText.indexOf("LED状态") != -1 || messageText.indexOf("灯状态") != -1) {
        String status = ledController->getStatusString();
        Serial.println(status);
        wsClient->sendStatusUpdate(status);
    }
    
    // 机器人步行控制命令
    else if (messageText.indexOf("前进") != -1 || messageText.indexOf("向前") != -1 || messageText.indexOf("走前") != -1) {
        Serial.println("执行前进步态");
        servoController->walkForward();
        wsClient->sendStatusUpdate("机器人开始前进");
    } else if (messageText.indexOf("后退") != -1 || messageText.indexOf("向后") != -1 || messageText.indexOf("倒退") != -1) {
        Serial.println("执行后退步态");
        servoController->walkBackward();
        wsClient->sendStatusUpdate("机器人开始后退");
    } else if (messageText.indexOf("站立") != -1 || messageText.indexOf("站起") != -1 || messageText.indexOf("起立") != -1) {
        Serial.println("执行站立动作");
        servoController->standUp();
        wsClient->sendStatusUpdate("机器人站立完成");
    } else if (messageText.indexOf("停止") != -1 || messageText.indexOf("停下") != -1 || messageText.indexOf("不动") != -1) {
        Serial.println("执行停止动作");
        servoController->stopWalk();
        wsClient->sendStatusUpdate("机器人停止移动");
    } else if (messageText.indexOf("左腿前") != -1 || messageText.indexOf("左脚前") != -1) {
        Serial.println("左腿前进动作");
        servoController->leftLegForward();
        wsClient->sendStatusUpdate("左腿前进完成");
    } else if (messageText.indexOf("左腿后") != -1 || messageText.indexOf("左脚后") != -1) {
        Serial.println("左腿后退动作");
        servoController->leftLegBackward();
        wsClient->sendStatusUpdate("左腿后退完成");
    } else if (messageText.indexOf("右腿前") != -1 || messageText.indexOf("右脚前") != -1) {
        Serial.println("右腿前进动作");
        servoController->rightLegForward();
        wsClient->sendStatusUpdate("右腿前进完成");
    } else if (messageText.indexOf("右腿后") != -1 || messageText.indexOf("右脚后") != -1) {
        Serial.println("右腿后退动作");
        servoController->rightLegBackward();
        wsClient->sendStatusUpdate("右腿后退完成");
    } else if (messageText.indexOf("舵机状态") != -1 || messageText.indexOf("腿部状态") != -1) {
        String status = servoController->getStatusString();
        Serial.println(status);
        wsClient->sendStatusUpdate(status);
    }
}
