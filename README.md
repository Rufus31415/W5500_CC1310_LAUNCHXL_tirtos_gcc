# TI RTOS driver for interfacing CC1310 CPU with W5500 ethernet chip

This project is a based on the W5500 C++ Arduino library (https://github.com/Wiznet/WIZ_Ethernet_Library). It has been converted to C, so that it can be compiled for CC1310.

This is a project for Code Composer Studio 7.2.0 (http://www.ti.com/tool/CCSTUDIO).

It runs on the real time operating system TI-RTOS (http://www.ti.com/tool/TI-RTOS-MCU).


![Schema W5500 and CC1310](https://raw.githubusercontent.com/Rufus31415/W5500_CC1310_LAUNCHXL_tirtos_gcc/master/SCHEMATIC_TI_CC1310_W5500.png)
      

# W5500 features
- Supports Hardwired TCP/IP Protocols : TCP, UDP, ICMP, IPv4, ARP, IGMP, PPPoE
- Supports 8 independent sockets simultaneously
- Supports Power down mode
- Supports Wake on LAN over UDP
- Supports High Speed Serial Peripheral Interface (SPI MODE 0, 3)
- Internal 32Kbytes Memory for TX/RXBuffers
- 10BaseT/100BaseTX Ethernet PHY embedded
- Supports Auto Negotiation (Full and half duplex, 10 and 100-based )
- 3.3V operation with 5V I/O signal tolerance
- LED outputs (Full/Half duplex, Link, Speed, Active)
- 48 Pin LQFP Lead-Free Package (7x7mm, 0.5mm pitch)


# CC1310 features
- Powerful Arm® Cortex® M3 Processor
- Ultra Low-Power Sensor Controller
- GPIO, SPI, UART, RTC, ADC, PWM
- Very low-power RF transceiver


