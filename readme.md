### 仓库说明
本仓库中的内容均以STM32f411RE的ST官方核心开发版nucleo-f411为基础开发，使用cubeMX配置代码，项目多数使用cubeMX配置的freeRTOS操作系统。(Keil工程在MDK-ARM文件夹内)
项目包括：
1. 可自定义使用的串口通信
2. OLED屏幕的硬件IIC通信（屏幕为四针IIC）
3. AD5933的通信针（IIC）（移植并改编了用于f4xx位带操作的sys.h文件，因此可以进行软件IIC的读写）
4. adg708开关选择电路的并行写法（利用arm架构的特性，并行口可同步写32位）
