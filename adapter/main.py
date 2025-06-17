import asyncio
import json
import websockets
from websockets.server import WebSocketServerProtocol
from typing import Set, Optional

from astrbot.api.event import filter, AstrMessageEvent, MessageEventResult
from astrbot.api.star import Context, Star, register
from astrbot.api import logger
import astrbot.api.message_components as Comp


@register("esp32s3_controller", "Jason.Joestar", "ESP32S3 WebSocket控制器插件", "1.0.0", "https://github.com/advent259141/astrbot_plugin_ESP32adapter")
class ESP32S3Plugin(Star):
    def __init__(self, context: Context):
        super().__init__(context)
        self.websocket_server = None
        self.connected_clients: Set[WebSocketServerProtocol] = set()
        self.server_host = "0.0.0.0"
        self.server_port = 8765
        
        # 启动WebSocket服务器
        asyncio.create_task(self.start_websocket_server())
    async def start_websocket_server(self):
        """启动WebSocket服务器"""
        try:
            logger.info(f"正在启动ESP32S3 WebSocket服务器，地址: {self.server_host}:{self.server_port}")
            self.websocket_server = await websockets.serve(
                self.handle_websocket_connection,
                self.server_host,
                self.server_port
            )
            logger.info(f"ESP32S3 WebSocket服务器已启动: ws://{self.server_host}:{self.server_port}")
        except Exception as e:
            logger.error(f"启动WebSocket服务器失败: {e}")
    
    async def handle_websocket_connection(self, websocket: WebSocketServerProtocol):
        """处理WebSocket连接"""
        client_addr = f"{websocket.remote_address[0]}:{websocket.remote_address[1]}"
        logger.info(f"ESP32S3设备已连接: {client_addr}")
        
        self.connected_clients.add(websocket)
        
        try:
            # 发送欢迎消息
            welcome_msg = {
                "type": "welcome",
                "message": "欢迎连接到AstrBot ESP32S3控制器",
                "timestamp": asyncio.get_event_loop().time()
            }
            await websocket.send(json.dumps(welcome_msg))
            
            # 持续监听客户端消息
            async for message in websocket:
                try:
                    data = json.loads(message)
                    await self.handle_esp32_message(websocket, data)
                except json.JSONDecodeError:
                    logger.error(f"收到无效JSON消息从 {client_addr}: {message}")
                except Exception as e:
                    logger.error(f"处理ESP32消息时出错: {e}")
                    
        except websockets.exceptions.ConnectionClosed:
            logger.info(f"ESP32S3设备断开连接: {client_addr}")
        except Exception as e:
            logger.error(f"WebSocket连接处理错误: {e}")        
        finally:
            self.connected_clients.discard(websocket)
    
    async def handle_esp32_message(self, websocket: WebSocketServerProtocol, data: dict):
        """处理来自ESP32的消息"""
        message_type = data.get("type", "unknown")
        client_addr = f"{websocket.remote_address[0]}:{websocket.remote_address[1]}"
        
        logger.info(f"收到ESP32消息 ({client_addr}): {data}")
        
        if message_type == "status":
            # 处理状态消息
            status = data.get("status", "unknown")
            logger.info(f"ESP32状态更新: {status}")
            
        elif message_type == "heartbeat":
            # 处理心跳消息
            response = {
                "type": "heartbeat_ack",
                "timestamp": asyncio.get_event_loop().time()
            }
            await websocket.send(json.dumps(response))
            
        else:
            logger.warning(f"未知的消息类型: {message_type}")

    async def send_to_esp32(self, message: dict) -> bool:
        """向所有连接的ESP32设备发送消息"""
        if not self.connected_clients:
            logger.warning("没有连接的ESP32设备")
            return False
        
        message_json = json.dumps(message)
        disconnected_clients = set()
        
        for client in self.connected_clients:
            try:
                await client.send(message_json)
            except websockets.exceptions.ConnectionClosed:
                disconnected_clients.add(client)
            except Exception as e:
                logger.error(f"发送消息到ESP32失败: {e}")
                disconnected_clients.add(client)
        
        # 清理断开的连接
        self.connected_clients -= disconnected_clients
        
        successful_sends = len(self.connected_clients) - len(disconnected_clients)
        return successful_sends > 0

    @filter.event_message_type(filter.EventMessageType.PRIVATE_MESSAGE)
    async def on_all_message(self, event: AstrMessageEvent):
        """监听所有消息并转发给ESP32设备"""
        try:
            # 构造要发送给ESP32的消息
            message_data = {
                "type": "astrbot_message",
                "platform": event.get_platform_name(),
                "sender_id": event.get_sender_id(),
                "sender_name": event.get_sender_name(),
                "message_text": event.message_str,
                "message_type": event.get_message_type().value,
                "group_id": event.get_group_id() if event.get_group_id() else None,
                "timestamp": asyncio.get_event_loop().time(),
                "is_private": event.is_private_chat(),
                "is_admin": event.is_admin()
            }
            
            # 添加消息组件信息
            message_components = []
            for comp in event.get_messages():
                if isinstance(comp, Comp.Plain):
                    message_components.append({
                        "type": "text",
                        "content": comp.text
                    })
                elif isinstance(comp, Comp.Image):
                    message_components.append({
                        "type": "image",
                        "url": comp.url if hasattr(comp, 'url') else "unknown"
                    })
                elif isinstance(comp, Comp.At):
                    message_components.append({
                        "type": "at",
                        "target": comp.qq,
                        "name": comp.name
                    })
            
            message_data["components"] = message_components
            
            # 发送消息到ESP32设备
            success = await self.send_to_esp32(message_data)
            
            if success:
                logger.debug(f"已将消息转发给ESP32设备: {event.message_str[:50]}...")
            else:
                logger.debug("没有ESP32设备连接，跳过消息转发")
                
        except Exception as e:
            logger.error(f"处理消息转发时出错: {e}")

    @filter.command("esp32")
    async def esp32_command(self, event: AstrMessageEvent):
        """ESP32设备控制指令"""
        if not self.connected_clients:
            yield event.plain_result("❌ 没有ESP32设备连接")
            return
            
        yield event.plain_result(f"✅ ESP32设备状态:\n连接设备数量: {len(self.connected_clients)}")

    @filter.command("esp32_send")
    async def esp32_send_command(self, event: AstrMessageEvent, message: str):
        """向ESP32设备发送自定义消息"""
        if not self.connected_clients:
            yield event.plain_result("❌ 没有ESP32设备连接")
            return
        
        try:
            custom_message = {
                "type": "custom_command",
                "command": message,
                "from_user": event.get_sender_name(),
                "timestamp": asyncio.get_event_loop().time()
            }
            
            success = await self.send_to_esp32(custom_message)
            
            if success:
                logger.info(f"✅ 已发送消息到ESP32设备: {message}")
            else:
                logger.error("❌ 发送消息失败")
                
        except Exception as e:
            logger.error(f"发送自定义消息失败: {e}")

    @filter.command("esp32_status")
    async def esp32_status_command(self, event: AstrMessageEvent):
        """查看ESP32设备连接状态"""
        if not self.connected_clients:
            yield event.plain_result("❌ 没有ESP32设备连接")
            return
        
        status_info = []
        status_info.append(f"🔗 WebSocket服务器: ws://{self.server_host}:{self.server_port}")
        status_info.append(f"📱 连接设备数量: {len(self.connected_clients)}")
        
        for i, client in enumerate(self.connected_clients, 1):
            client_addr = f"{client.remote_address[0]}:{client.remote_address[1]}"
            status_info.append(f"  设备{i}: {client_addr}")
        
        yield event.plain_result("\n".join(status_info))    
    @filter.llm_tool(name="control_esp32_led")
    async def control_esp32_led(self, event: AstrMessageEvent, action: str, brightness: int = 100):
        '''控制ESP32设备的LED灯开关和亮度。

        Args:
            action(string): 操作类型，可选值：on（开灯）、off（关灯）、toggle（切换状态）
            brightness(number): LED亮度，范围0-100，默认100
        '''
        if not self.connected_clients:
            return "没有ESP32设备连接，无法执行LED控制操作"
        
        # 验证参数
        valid_actions = ["on", "off", "toggle"]
        if action.lower() not in valid_actions:
            return f"无效的操作类型'{action}'，支持的操作：{', '.join(valid_actions)}"
        
        if not 0 <= brightness <= 100:
            return f"亮度值{brightness}超出范围，必须在0-100之间"
        
        try:
            # 构造控制命令
            control_message = {
                "type": "led_control",
                "action": action.lower(),
                "brightness": brightness,
                "from_user": event.get_sender_name(),
                "timestamp": asyncio.get_event_loop().time()
            }
            
            # 发送控制命令到ESP32设备
            success = await self.send_to_esp32(control_message)
            
            if success:
                # 返回成功信息给LLM
                action_results = {
                    "on": f"成功开启LED灯，亮度设置为{brightness}%",
                    "off": "成功关闭LED灯",
                    "toggle": f"成功切换LED灯状态，亮度设置为{brightness}%"
                }
                return action_results[action.lower()]
            else:
                return "发送LED控制指令失败，请检查ESP32设备连接状态"
                
        except Exception as e:
            logger.error(f"控制ESP32 LED灯失败: {e}")
            return f"LED控制操作发生错误: {str(e)}"
        
    @filter.llm_tool(name="control_esp32_oled")
    async def control_esp32_oled(self, event: AstrMessageEvent, action: str, content: str = ""):
        '''控制ESP32设备的OLED屏幕显示内容。

        Args:
            action(string): 操作类型，可选值：emotion（显示表情）、text（显示文本）、clear（清除屏幕）
            content(string): 显示内容。当action为emotion时，支持的表情：happy/开心、sad/伤心、angry/生气、surprised/惊讶、sleepy/困、love/爱心、cool/酷、thinking/思考；当action为text时，为要显示的文本内容
        '''
        if not self.connected_clients:
            return "没有ESP32设备连接，无法执行OLED控制操作"
        
        # 验证参数
        valid_actions = ["emotion", "text", "clear"]
        if action.lower() not in valid_actions:
            return f"无效的操作类型'{action}'，支持的操作：{', '.join(valid_actions)}"
        
        # 验证表情类型
        if action.lower() == "emotion":
            valid_emotions = [
                "happy", "开心", "高兴", "快乐",
                "sad", "伤心", "难过",
                "angry", "生气", "愤怒", 
                "surprised", "惊讶", "吃惊",
                "sleepy", "困", "睡觉",
                "love", "爱心", "喜欢",
                "cool", "酷", "帅",
                "thinking", "思考", "想"
            ]
            if content.lower() not in valid_emotions:
                return f"不支持的表情'{content}'，支持的表情：{', '.join(valid_emotions)}"
        
        try:
            # 构造控制命令
            control_message = {
                "type": "oled_control",
                "action": action.lower(),
                "content": content,
                "from_user": event.get_sender_name(),
                "timestamp": asyncio.get_event_loop().time()
            }
            
            # 发送控制命令到ESP32设备
            success = await self.send_to_esp32(control_message)
            
            if success:                # 返回成功信息给LLM
                if action.lower() == "emotion":
                    return f"成功在OLED屏幕上显示{content}表情"
                elif action.lower() == "text":
                    return f"成功在OLED屏幕上显示文本：{content}"
                elif action.lower() == "clear":
                    return "成功清除OLED屏幕内容"
            else:
                return "发送OLED控制指令失败，请检查ESP32设备连接状态"
        except Exception as e:
            logger.error(f"控制ESP32 OLED屏幕失败: {e}")
            return f"OLED控制操作发生错误: {str(e)}"    
    @filter.llm_tool(name="control_esp32_servo")
    async def control_esp32_servo(self, event: AstrMessageEvent, action: str, angle: str = "90"):
        '''控制ESP32设备的SG90舵机旋转角度。

        Args:
            action(string): 操作类型，可选值：rotate（旋转到指定角度）、center（回到中位90度）、sweep（扫描模式，左右摆动）
            angle(string): 目标角度，范围0-180度，默认"90"（仅在action为rotate时有效）
        '''
        if not self.connected_clients:
            return "没有ESP32设备连接，无法执行舵机控制操作"
        
        # 验证参数
        valid_actions = ["rotate", "center", "sweep"]
        if action.lower() not in valid_actions:
            return f"无效的操作类型'{action}'，支持的操作：{', '.join(valid_actions)}"
        
        # 将角度字符串转换为整数并验证范围
        try:
            angle_int = int(angle)
        except ValueError:
            return f"角度必须是数字，当前值：{angle}"
        if angle_int < 0 or angle_int > 180:
            return f"角度必须在0-180度范围内，当前值：{angle_int}度"
        
        try:
            # 构造控制命令
            control_message = {
                "type": "servo_control",
                "action": action.lower(),
                "angle": angle_int,
                "from_user": event.get_sender_name(),
                "timestamp": asyncio.get_event_loop().time()
            }
            
            # 发送控制命令到ESP32设备
            success = await self.send_to_esp32(control_message)
            
            if success:
                # 返回成功信息给LLM
                action_results = {
                    "rotate": f"成功将舵机旋转到{angle_int}度",
                    "center": "成功将舵机回到中位（90度）",
                    "sweep": "成功执行舵机扫描模式（左右摆动）"
                }
                return action_results[action.lower()]
            else:
                return "发送舵机控制指令失败，请检查ESP32设备连接状态"
        except Exception as e:
            logger.error(f"控制ESP32舵机失败: {e}")
            return f"舵机控制操作发生错误: {str(e)}"
    
    async def terminate(self):
        """插件卸载时的清理工作"""
        logger.info("正在关闭ESP32S3 WebSocket服务器...")
        
        # 关闭所有客户端连接
        if self.connected_clients:
            close_tasks = []
            for client in self.connected_clients:
                close_tasks.append(client.close())
            await asyncio.gather(*close_tasks, return_exceptions=True)
            self.connected_clients.clear()
        
        # 关闭WebSocket服务器
        if self.websocket_server:
            self.websocket_server.close()
            await self.websocket_server.wait_closed()
            
        logger.info("ESP32S3 WebSocket服务器已关闭")
