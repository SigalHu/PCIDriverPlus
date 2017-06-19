# PCIDriverPlus
基于WinDriver的多设备PCI驱动程序及测试软件
## 开发工具
* VS2013
* WinDriver v10.00
## 介绍
* PCIDriverPlus工程：无界面后台软件，对PCI驱动相关功能的实现进行封装，支持多个PCI设备DMA并行传输
* PCIDriverHelper工程：对调用PCIDriverPlus软件的具体实现进行封装，生成dll（针对C++），简化用户操作
* PCIDriverTest工程：PCI驱动测试软件（MFC版本），对PCIDriverPlus驱动管理软件的功能进行测试
* PCIDriverHelper4CS工程：对调用PCIDriverPlus软件的具体实现进行封装，生成dll（针对C#），简化用户操作
* PCIDriverTest4CS工程：PCI驱动测试软件（WinForm版本），对PCIDriverPlus驱动管理软件的功能进行测试
## 实现功能
支持中断收发、寄存器读写与多设备DMA并行传输等操作
## 说明
该版本是[PCIDriver工程](https://github.com/SigalHu/PCIDriver)的升级版，全新架构，主要是为了解决上一版本不支持多设备DMA并行传输与反复申请、释放连续物理地址空间所导致的申请空间失败问题。
## DMA传输
![image](https://github.com/SigalHu/PCIDriverPlus/raw/master/img/DMA写操作.png)
