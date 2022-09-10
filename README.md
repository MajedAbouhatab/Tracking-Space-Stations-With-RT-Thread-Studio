# Tracking Space Stations With RT-Thread Studio
![](https://hackster.imgix.net/uploads/attachments/1493533/ezgif_com-gif-maker_(1)_ng6xGzwt4h.gif?auto=format%2Ccompress&gifq=35&w=900&h=675&fit=min&fm=mp4)
## Things used in this project
### Hardware components
STM32F469 DISCOVERY  
Espressif ESP8266 ESP-01  
Arduino Mini USB serial adapter  
### Software apps and online services
RT-Thread IoT OS  
STMicroelectronics STM32CubeMX  
Microsoft VS Code  
ezgif.com  
LVGL  
### Hand tools and fabrication machines
Soldering iron (generic)  
Solder Wire, Lead Free  
## Story

### Background

Many of my fellow makers used Single Board Computers (SBC) and paid Application Programming Interface to tack satellites in Earth orbit. This project is a simple and low cost method to track Space Stations. We used knowledge gained from my [previous project](https://www.hackster.io/abouhatab/using-rt-thread-studio-to-control-esp32-with-stm32f469-disco-117bdf) and data provided to our web browser for free by n2yo.com. Furthermore, we displayed the names of people currently in space using information gathered by open-notify.org.
### Game Plan

![](https://hackster.imgix.net/uploads/attachments/1493963/20220909_202853_VOpuTZfaTx.jpg?auto=compress%2Cformat&w=740&h=555&fit=max)

Just like in the previous project, we used UART to communicate between STM32F469 DISCOVERY board and the Wi-Fi Dev board. To add an extra challenge and reduce the cost of implementation we replaced ESP32 with ESP-01 which required a programmer because there is no USB port.

We got the URLs we needed to use by pressing F12 on the page we need to get data from. Studying JavaScrips reveled that we have to use the following format:

```
https://www.n2yo.com/sat/instant-tracking.php?s=<NORAD ID>&d=1
```

The North American Aerospace Defense (NORAD) ID is 25544 for ISS and for Tiangong it is 48274. Making a GET request to the URL above will return a JSON formatted string that contains the GPS coordinates to be displayed. That string and the one received from [http://api.open-notify.org/astros.json](http://api.open-notify.org/astros.json) are so simple we didn't have to use a library to parse them.
### Board Support Package (BSP)

We started by downloading and extracting [https://github.com/RT-Thread/rt-thread/archive/refs/heads/master.zip](https://github.com/RT-Thread/rt-thread/archive/refs/heads/master.zip) to get the latest BSP. According to the [User Manual,](https://www.st.com/resource/en/user_manual/um1932-discovery-kit-with-stm32f469ni-mcu-stmicroelectronics.pdf) we have four USART ports and, four UART ports on that board, however, only USART3 is enabled by default. Using STM32 CubeMX, we opened %userprofile%\Downloads\rt-thread-master\rt-thread-master\bsp\stm32\stm32f469-st-disco\board\CubeMX_Config\CubeMX_Config.ioc

![](https://hackster.imgix.net/uploads/attachments/1493474/image_iWCATBc1WD.png?auto=compress%2Cformat&w=740&h=555&fit=max)

After clicking Continue, we expanded connectivity on the left pane and clicked USART6.

![](https://hackster.imgix.net/uploads/attachments/1493476/image_gV307N3nFL.png?auto=compress%2Cformat&w=740&h=555&fit=max)

We replaced Disabled with Asynchronous in the Mode drop down menu. We then clicked Generate Code in the top right corner and closed the the application.

![](https://hackster.imgix.net/uploads/attachments/1493478/image_EBriBeYqBu.png?auto=compress%2Cformat&w=740&h=555&fit=max)

We located Kconfig (one folder up) and duplicated UART3 text then replaced with UART6. Now, it looks like this.

![](https://hackster.imgix.net/uploads/attachments/1493480/image_ODG0ZcYt7V.png?auto=compress%2Cformat&w=740&h=555&fit=max)

BSP is ready to be used after saving this file.

### RT-Thread Studio

One minute into this tutorial shows how to import BSP that we just modified.

This is how our import looked.

![](https://hackster.imgix.net/uploads/attachments/1493517/image_5JcVIetR4y.png?auto=compress%2Cformat&w=740&h=555&fit=max)

We opened RT-Thread Settings from the left pane then clicked on the chevrons on the right.

![](https://hackster.imgix.net/uploads/attachments/1493522/image_Mh90eN0Op1.png?auto=compress%2Cformat&w=740&h=555&fit=max)

We came here to Enable LVGL for LCD which will automatically trigger all the needed toggles.

![](https://hackster.imgix.net/uploads/attachments/1493523/image_3S01mgFIOR.png?auto=compress%2Cformat&w=740&h=555&fit=max)

We needed to also turn on UART6 we just enabled.

![](https://hackster.imgix.net/uploads/attachments/1493525/image_v36wDnKT7A.png?auto=compress%2Cformat&w=740&h=555&fit=max)

We had to increase Rx buffer size from 64 to 1024.

![](https://hackster.imgix.net/uploads/attachments/1493526/image_cYCzltFjMT.png?auto=compress%2Cformat&w=740&h=555&fit=max)

We deleted all files in applications folder then copied our files from Github.

![](https://hackster.imgix.net/uploads/attachments/1493772/image_ya93kb7RzP.png?auto=compress%2Cformat&w=740&h=555&fit=max)

To allow auto reset after code download we needed to make this change.

![](https://hackster.imgix.net/uploads/attachments/1493527/image_AZQoMR27L7.png?auto=compress%2Cformat&w=740&h=555&fit=max)

Please note that this tutorial is using LVGL v8.3.1 and if you have to look something up on their website, make sure you are looking at the correct version.

![](https://hackster.imgix.net/uploads/attachments/1493498/image_ZEiPbSuwp0.png?auto=compress%2Cformat&w=740&h=555&fit=max)

### Visual Studio Code

We used [tzapu/WiFiManager](https://github.com/tzapu/WiFiManager) to connect ESP01 to Wi-Fi without hard coding credentials. Once we are online we can wait to URL to come from STM32F469 DISCOVERY via UART then send back the unprocessed response.

### Hardware

![](https://hackster.imgix.net/uploads/attachments/1493964/20220909_112949_7RjbrdlW54.jpg?auto=compress%2Cformat&w=740&h=555&fit=max)

We used the standard UART connection to make the boards communicate. Tx of one board is connected to Rx of the other and vice versa. We are powering STM32F469 DISCOVERY with USB cable, and powering ESP-01 from it.

The breakout board we bought from AliExpress had one small problem; see if you can spot it.

![](https://hackster.imgix.net/uploads/attachments/1493628/image_rThL588BW3.png?auto=compress%2Cformat&w=740&h=555&fit=max)

Luckily the swapped pins didn't cause permanent damage to the programmer or ESP-01.

We soldered two switches on the bottom of the breakout board then connected it to a USB Serial Adapter. One switch is the reset switch that connects RST to ground. The second switch keeps GPIO0 grounded while resetting the board then it can be release right after that.

![](https://hackster.imgix.net/uploads/attachments/1493627/image_VaIFXOHZhR.png?auto=compress%2Cformat&w=740&h=555&fit=max)

Before we plug to the laptop we ensured the programmer's jumper is on 3.3V position not 5V. We used VS Code to program ESP-01, but Arduino IDE could be used, too. The red jumper between Vcc and CHPD is needed regardless of the mode.

![](https://hackster.imgix.net/uploads/attachments/1493778/image_ZM1WaLc4kP.png?auto=compress%2Cformat&w=740&h=555&fit=max)

### Wiring
![](https://hackster.imgix.net/uploads/attachments/1493967/wiring_3PbbgH9o3S.jpg?auto=compress%2Cformat&w=1280&h=960&fit=max)
### Demo
https://www.youtube.com/watch?v=mwpWGCFDQZM
