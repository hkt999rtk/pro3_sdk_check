# WIFI-INIC
Here provide supported interfaces for communication between application and wifi driver.

## Introduction

* folder ipc for all code needed by ipc, and used by interfaces ipc only.
* folder none_ipc_dev for common code for wifi driver, used by interfaces like spi/sdio/usb and etc.
* folder none_ipc_host for common code for application, used by interfaces like spi/sdio/usb and etc.
* folder sdio/spi/usb contains the drivers for different interface. 
* folder none_ipc_rtos, include all rtos func needed by inic host, and user need to porting all files in this folder if not using rtk as rtos host.
