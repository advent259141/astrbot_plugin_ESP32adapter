#include "oled_display.h"

OledDisplay::OledDisplay(int width, int height, int sda, int scl, int address)
    : screenWidth(width), screenHeight(height), sdaPin(sda), sclPin(scl), 
      screenAddress(address), initialized(false),
      display(width, height, &Wire, -1) {
}

bool OledDisplay::init() {
    // 初始化I2C通信
    Wire.begin(sdaPin, sclPin);
    
    // 尝试初始化OLED显示屏
    if(!display.begin(SSD1306_SWITCHCAPVCC, screenAddress)) {
        Serial.println("OLED初始化失败，请检查I2C连接和地址!");
        initialized = false;
        return false;
    }
    
    Serial.println("OLED初始化成功!");
    initialized = true;
    
    // 清除显示缓冲区并显示启动信息
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 15);
    display.println("AstrBot ESP32S3");
    display.setCursor(0, 30);
    display.println("OLED Ready!");
    display.display();
    
    delay(2000);
    clear();
    
    return true;
}

void OledDisplay::displayEmotion(String emotion) {
    if (!initialized) return;
    
    clear();
    
    emotion.toLowerCase();
    
    if (emotion == "happy" || emotion == "开心" || emotion == "高兴" || emotion == "快乐") {
        drawHappyFace();
    } else if (emotion == "sad" || emotion == "伤心" || emotion == "难过") {
        drawSadFace();
    } else if (emotion == "angry" || emotion == "生气" || emotion == "愤怒") {
        drawAngryFace();
    } else if (emotion == "surprised" || emotion == "惊讶" || emotion == "吃惊") {
        drawSurprisedFace();
    } else if (emotion == "sleepy" || emotion == "困" || emotion == "睡觉") {
        drawSleepyFace();
    } else if (emotion == "love" || emotion == "爱心" || emotion == "喜欢") {
        drawHeartEyes();
    } else if (emotion == "cool" || emotion == "酷" || emotion == "帅") {
        drawCoolFace();
    } else if (emotion == "thinking" || emotion == "思考" || emotion == "想") {
        drawThinkingFace();
    } else {
        // 默认显示疑问表情
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(45, 20);
        display.println("?_?");
        display.setTextSize(1);
        display.setCursor(20, 45);
        display.println("Unknown emotion");
    }
    
    display.display();
}

void OledDisplay::displayText(String text) {
    if (!initialized) return;
    
    clear();
    
    // 使用Adafruit库显示文本
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    
    // 简单的文本换行处理
    int lineHeight = 10;
    int currentY = 0;
    int maxWidth = 21;  // 每行大约21个字符 (128像素 / 6像素每字符)
    
    String currentLine = "";
    for (int i = 0; i < text.length(); i++) {
        currentLine += text[i];
        
        // 检查当前行长度或遇到换行符
        if (currentLine.length() >= maxWidth || text[i] == '\n' || i == text.length() - 1) {
            if (text[i] == '\n') {
                currentLine.remove(currentLine.length() - 1);  // 移除换行符
            }
            
            display.setCursor(0, currentY);
            display.println(currentLine);
            currentY += lineHeight;
            currentLine = "";
            
            // 防止超出屏幕
            if (currentY > 54) break;  // 64 - 10 = 54
            
            // 如果是因为长度换行，保留当前字符
            if (text[i] != '\n' && i != text.length() - 1) {
                currentLine = text[i];
            }
        }
    }
    
    display.display();
}

void OledDisplay::clear() {
    if (!initialized) return;
    
    display.clearDisplay();
    display.display();
}

bool OledDisplay::isInitialized() const {
    return initialized;
}

// 画开心表情 ^_^
void OledDisplay::drawHappyFace() {
    // 眉毛（弯曲的开心眉毛）
    for(int i = 0; i < 2; i++) {
        display.drawCircle(45, 25, 8 + i, SSD1306_WHITE);
        display.drawCircle(83, 25, 8 + i, SSD1306_WHITE);
    }
    // 遮盖下半部分形成弯眉，同时消除中间连接
    display.fillRect(37, 25, 16, 10, SSD1306_BLACK);
    display.fillRect(75, 25, 16, 10, SSD1306_BLACK);
    // 消除两眼中间的弧
    display.fillRect(37, 25, 42, 10, SSD1306_BLACK);
    
    // 眼睛（大的圆角正方形）
    display.fillRoundRect(35, 35, 16, 16, 4, SSD1306_WHITE);
    display.fillRoundRect(77, 35, 16, 16, 4, SSD1306_WHITE);
    
    // 嘴巴（开心的微笑弧形）
    for(int i = 0; i < 3; i++) {
        display.drawCircle(64, 45, 15 + i, SSD1306_WHITE);
    }
    // 遮盖上半部分形成微笑弧
    display.fillRect(49, 45, 30, 15, SSD1306_BLACK);
}

// 画伤心表情 T_T
void OledDisplay::drawSadFace() {
    // 眉毛（下垂的伤心眉毛）
    display.drawLine(35, 18, 50, 25, SSD1306_WHITE);
    display.drawLine(36, 19, 51, 26, SSD1306_WHITE);
    display.drawLine(78, 25, 93, 18, SSD1306_WHITE);
    display.drawLine(77, 26, 92, 19, SSD1306_WHITE);
    
    // 眼睛（大的圆角正方形）
    display.fillRoundRect(35, 35, 16, 16, 4, SSD1306_WHITE);
    display.fillRoundRect(77, 35, 16, 16, 4, SSD1306_WHITE);
    
    // 眼泪
    display.drawLine(43, 51, 43, 58, SSD1306_WHITE);
    display.drawLine(85, 51, 85, 58, SSD1306_WHITE);
    display.fillCircle(43, 59, 2, SSD1306_WHITE);
    display.fillCircle(85, 59, 2, SSD1306_WHITE);
    
    // 嘴巴（倒弧，伤心）
    for(int i = 0; i < 3; i++) {
        display.drawCircle(64, 75, 15 + i, SSD1306_WHITE);
    }
    // 遮盖下半部分形成倒弧
    display.fillRect(49, 60, 30, 15, SSD1306_BLACK);
}

// 画生气表情 >:(
void OledDisplay::drawAngryFace() {
    // 眉毛（愤怒的倾斜眉毛）
    display.fillTriangle(30, 20, 50, 30, 30, 25, SSD1306_WHITE);
    display.fillTriangle(98, 20, 78, 30, 98, 25, SSD1306_WHITE);
    
    // 眼睛（大的圆角正方形，稍微倾斜）
    display.fillRoundRect(35, 38, 16, 12, 3, SSD1306_WHITE);
    display.fillRoundRect(77, 38, 16, 12, 3, SSD1306_WHITE);
    
    // 鼻孔（愤怒时的鼻孔）
    display.fillCircle(62, 50, 1, SSD1306_WHITE);
    display.fillCircle(66, 50, 1, SSD1306_WHITE);
    
    // 嘴巴（愤怒的波浪线）
    display.drawLine(45, 58, 55, 55, SSD1306_WHITE);
    display.drawLine(55, 55, 65, 58, SSD1306_WHITE);
    display.drawLine(65, 58, 75, 55, SSD1306_WHITE);
    display.drawLine(75, 55, 85, 58, SSD1306_WHITE);
}

// 画惊讶表情 O_O
void OledDisplay::drawSurprisedFace() {
    // 眉毛（高高扬起的惊讶眉毛）
    for(int i = 0; i < 2; i++) {
        display.drawCircle(45, 15, 12 + i, SSD1306_WHITE);
        display.drawCircle(83, 15, 12 + i, SSD1306_WHITE);
    }
    // 遮盖下半部分形成高眉
    display.fillRect(33, 15, 24, 12, SSD1306_BLACK);
    display.fillRect(71, 15, 24, 12, SSD1306_BLACK);
    
    // 眼睛（非常大的圆角正方形）
    display.fillRoundRect(30, 30, 20, 20, 5, SSD1306_WHITE);
    display.fillRoundRect(78, 30, 20, 20, 5, SSD1306_WHITE);
    // 瞳孔
    display.fillCircle(40, 40, 3, SSD1306_BLACK);
    display.fillCircle(88, 40, 3, SSD1306_BLACK);
    
    // 嘴巴（大的惊讶圆圈）
    display.drawCircle(64, 58, 8, SSD1306_WHITE);
    display.drawCircle(64, 58, 7, SSD1306_WHITE);
}

// 画困倦表情 -_-
void OledDisplay::drawSleepyFace() {
    // 眉毛（下垂的困倦眉毛）
    display.drawLine(35, 22, 50, 18, SSD1306_WHITE);
    display.drawLine(36, 23, 51, 19, SSD1306_WHITE);
    display.drawLine(78, 18, 93, 22, SSD1306_WHITE);
    display.drawLine(77, 19, 92, 23, SSD1306_WHITE);
    
    // 眼睛（扁平的圆角矩形，表示闭眼）
    display.fillRoundRect(35, 40, 16, 4, 2, SSD1306_WHITE);
    display.fillRoundRect(77, 40, 16, 4, 2, SSD1306_WHITE);
    
    // 嘴巴（小的困倦弧）
    for(int i = 0; i < 2; i++) {
        display.drawCircle(64, 52, 6 + i, SSD1306_WHITE);
    }
    display.fillRect(58, 52, 12, 6, SSD1306_BLACK);
    
    // ZZZ睡觉符号
    display.setTextSize(2);
    display.setCursor(95, 10);
    display.println("Z");
    display.setTextSize(1);
    display.setCursor(105, 20);
    display.println("Z");
    display.setCursor(110, 25);
    display.println("z");
}

// 画爱心眼表情
void OledDisplay::drawHeartEyes() {
    // 眉毛（开心的弯眉）
    for(int i = 0; i < 2; i++) {
        display.drawCircle(45, 22, 10 + i, SSD1306_WHITE);
        display.drawCircle(83, 22, 10 + i, SSD1306_WHITE);
    }
    display.fillRect(35, 22, 20, 10, SSD1306_BLACK);
    display.fillRect(73, 22, 20, 10, SSD1306_BLACK);
    
    // 爱心眼睛（在圆角正方形框架内）
    display.drawRoundRect(32, 32, 20, 20, 5, SSD1306_WHITE);
    display.drawRoundRect(76, 32, 20, 20, 5, SSD1306_WHITE);
    
    // 左爱心
    display.fillCircle(38, 38, 3, SSD1306_WHITE);
    display.fillCircle(46, 38, 3, SSD1306_WHITE);
    display.fillTriangle(35, 41, 49, 41, 42, 48, SSD1306_WHITE);
    
    // 右爱心
    display.fillCircle(82, 38, 3, SSD1306_WHITE);
    display.fillCircle(90, 38, 3, SSD1306_WHITE);
    display.fillTriangle(79, 41, 93, 41, 86, 48, SSD1306_WHITE);
    
    // 微笑
    for(int i = 0; i < 3; i++) {
        display.drawCircle(64, 45, 15 + i, SSD1306_WHITE);
    }
    display.fillRect(49, 45, 30, 15, SSD1306_BLACK);
}

// 画酷表情（戴墨镜）
void OledDisplay::drawCoolFace() {
    // 眉毛（平直的酷眉）
    display.fillRect(35, 18, 18, 3, SSD1306_WHITE);
    display.fillRect(75, 18, 18, 3, SSD1306_WHITE);
    
    // 墨镜（连体的大墨镜）
    display.fillRoundRect(25, 30, 78, 20, 8, SSD1306_WHITE);
    display.fillRoundRect(30, 34, 25, 12, 6, SSD1306_BLACK);
    display.fillRoundRect(73, 34, 25, 12, 6, SSD1306_BLACK);
    display.fillRect(55, 38, 18, 4, SSD1306_WHITE); // 鼻梁
    
    // 嘴巴（酷酷的直线，稍微上扬）
    display.drawLine(50, 58, 78, 56, SSD1306_WHITE);
    display.drawLine(50, 59, 78, 57, SSD1306_WHITE);
}

// 画思考表情
void OledDisplay::drawThinkingFace() {
    // 眉毛（一高一低，表示疑惑）
    for(int i = 0; i < 2; i++) {
        display.drawCircle(45, 20, 8 + i, SSD1306_WHITE);
    }
    display.fillRect(37, 20, 16, 8, SSD1306_BLACK);
    display.drawLine(78, 18, 93, 25, SSD1306_WHITE);
    display.drawLine(77, 19, 92, 26, SSD1306_WHITE);
    
    // 眼睛（一个正常，一个眯眼）
    display.fillRoundRect(35, 35, 16, 16, 4, SSD1306_WHITE);
    display.fillRoundRect(77, 40, 16, 6, 3, SSD1306_WHITE);
    
    // 瞳孔（向上看）
    display.fillCircle(43, 38, 2, SSD1306_BLACK);
    
    // 嘴巴（歪嘴，表示思考）
    display.drawLine(55, 58, 70, 56, SSD1306_WHITE);
    display.drawLine(55, 59, 70, 57, SSD1306_WHITE);
    
    // 思考气泡
    display.drawCircle(100, 15, 10, SSD1306_WHITE);
    display.fillCircle(90, 25, 3, SSD1306_WHITE);
    display.fillCircle(85, 30, 2, SSD1306_WHITE);
    display.fillCircle(82, 33, 1, SSD1306_WHITE);
    
    // 问号
    display.setTextSize(1);
    display.setCursor(96, 11);
    display.println("?");
}
