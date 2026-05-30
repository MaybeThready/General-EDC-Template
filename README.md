# General-EDC-Template

通用电子设计竞赛软件开发模板，面向电子设计竞赛类项目，硬件支持 Cyclone IV FPGA 与 MSPM0G3519 MCU。

## 项目简介

- 目标: 提供软硬件协同开发的基础工程与目录结构，便于快速落地赛题需求。
- 硬件支持: Cyclone IV FPGA 与 MSPM0G3519。
- 代码范围: FPGA 工程、MCU 工程、板级支持与 UI 配置。

## 目录结构

- fpga/: Quartus 工程与输出产物
  - src/: FPGA 源码
  - output_files/: 编译输出文件
  - simulation/: 仿真相关文件
- mcu/: MCU 工程
  - main.c: 主程序入口
  - ti_msp_dl_config.c/h: DriverLib 配置
  - ui_config.h: UI/交互配置
  - bsp/: 板级支持
  - ui/: UI 相关实现

## 环境与工具链

- FPGA: Intel Quartus (Cyclone IV 系列支持)
- MCU: TI MSPM0 SDK (MSPM0G3519)
- 其他: 需配套的下载器与驱动

## 快速开始

1. 打开 FPGA 工程
 - 使用 Quartus 打开 [fpga/main.qpf](fpga/main.qpf)。
2. 打开 MCU 工程
 - 使用对应 IDE 或构建系统打开 [mcu/main.c](mcu/main.c) 所在工程。
3. 配置与编译
 - 根据硬件连接修改引脚/外设配置。
 - 分别编译 FPGA 与 MCU 工程，完成下载与联调。
