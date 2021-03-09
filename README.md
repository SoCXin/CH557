# [CH557](https://github.com/SoCXin/CH557)

[![sites](http://182.61.61.133/link/resources/SoC.png)](http://www.SoC.Xin)

#### [Vendor](https://github.com/SoCXin/Vendor)：[WCH](http://www.wch.cn/)
#### [Core](https://github.com/SoCXin/8051)：[E8051](https://github.com/SoCXin/8051)
#### [Level](https://github.com/SoCXin/Level)：48MHz

## [CH547简介](https://github.com/SoCXin/CH557/wiki)

[CH557](https://github.com/SoCXin/CH557) 兼容MCS51的增强型E8051内核，79%指令是单字节单周期指令，3KB BootLoader + 60KB CodeFlash，8K xRAM + 256B iRAM，1K DataFlash。

内置了USB主机控制器和收发器，内置4端口USB root-hub根集线器，支持USB 2.0全速12Mbps或者低速1.5Mbps，支持USB Host主机模式和USB Device设备模式。支持最大64字节数据包，内置FIFO，支持DMA。
CH557提供丰富的接口资源，包括2组异步串口、6路PWM、14通道电容触摸按键、128组RGB三色LED控制器，其他包括2路SPI、IIC主机和IIC从机、14路12位ADC，支持电压比较；内置3组定时器和2路信号捕捉；支持最多58个GPIO。

[![sites](docs/CH557.png)](http://www.wch.cn/products/CH557.html)

#### 关键特性

* 内置4端口USB root-hub集线器
* UART x 2 + SPI x 2

### [资源收录](https://github.com/SoCXin/CH557)

* [参考文档](docs/)
* [参考资源](src/)
* [参考工程](project/)

### [选型建议](https://github.com/SoCXin)


CH556是[CH557](https://github.com/SoCXin/CH557)的简化版，去掉了RGB三色LED的PWM模块，仅提供2端口USB root-hub根集线器，其它的与CH557相同，可直接参考CH557手册和资料。

###  [芯云 www.SoC.xin](http://www.SoC.Xin)
