# stm8s-atomthreads-sdcc
SDCC编译
支持STM8的开源RTOS atomthreads (好不好用不清楚)
使用STVP下载程序到STM8S105K4最小系统板上，正常情况下LED一秒亮一秒灭交替闪烁（PE5），并且UART2每秒输出信息printf ("GPIO_WriteReverse(GPIOE, GPIO_PIN_5);\n");

貌似能够支持openocd调试，本人还没有实现。
