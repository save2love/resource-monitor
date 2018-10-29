# Resource Monitor for PC

![Device view](/img/device.jpg "Device")

Simple and useful project, made to monitor system resources without opening Task Manager. It can show you CPU and RAM usage, HDD and network activity. All data are visualize with charts. On host system uses Python script for collecting resources utilization that makes monitor is crossplatform.

## Getting Started

### Prerequisites

#### Software

* [Arduino IDE](https://www.arduino.cc/en/Main/Software "Arduino IDE")
* [Python 3.5+](https://www.python.org/downloads/ "Python 3.5")

#### Arduino libraries
* [LiquidCrystal I2C by Marco Schwartz](https://github.com/marcoschwartz/LiquidCrystal_I2C "LiquidCrystal I2C")
* [Bounce 2 by Thomas O Fredericks](https://github.com/thomasfredericks/Bounce2 "Bounce 2")

#### Python libraries
* [PSUtil](https://pypi.org/project/psutil/ "PSUtil")
```
pip install psutil
```
* [pyserial](https://pypi.org/project/pyserial/ "pyserial")
```
pip install pyserial
```

#### Hardware

* Arduino Nano or another small board
* PC2004A 20x4 LCD Display with IIC/I2C Serial Interface Adapter
* Push button

![Device schema](/img/schema.png "Device schema")
![Device back view](/img/device_back.jpg "Device back view")

### Configuring

All you need is change COM port on corresponding port number in [config.py](/agent/config.py "config.py")
To test communication you need start [agent_console.py](/agent/agent_console.py "agent_console.py"). Console will show you actually transferred data in table view.

### Autostart

You can setup to auto start python script in Windows using Task Scheduler and in Linux using cron

#### Windows

Here is [ResourceMonitorTask.xml](agent/ResourceMonitorTask.xml "ResourceMonitorTask.xml") for importing to Task Scheduler. All you need is change the path for agent.pyw script in Action tab.

#### Linux

You can use *cron* for autostart agent-script every minute but it was not tested by me.

### Screens

  - Default screen (full information about CPU, RAM, Network and HDD)
  
  ![Default screen](/img/1_default.png "Default screen")
  
  - CPU and RAM
  
  ![CPU and RAM](/img/2_cpu_and_ram.png "CPU and RAM")
  
  - CPU on full screen
  
  ![CPU on full screen](/img/3_cpu_detailed.png "CPU on full screen")
  
  - Network and HDD
  
  ![Network and HDD](/img/4_net_and_hdd.png "Network and HDD")
  
  - Network and HDD detailed
  
  ![Network and HDD detailed](/img/5_net_and_hdd_detailed.png "Network and HDD detailed")
  
  - Network on full screen 
  
  ![Network on full screen](/img/6_net_detailed.png "Network on full screen")
  
  - HDD on full screen
  
  ![HDD on full screen](/img/7_hdd_detailed.png "HDD on full screen")
  
Display screenshots were generated by [LCD Display Screenshot Generator](http://avtanski.net/projects/lcd/ "LCD Display Screenshot Generator") by Alexander Avtanski

### The button

* Single press - switching between screens
* Long press - enter to menu mode that causes the switching of menu items every second
* Release after long press - apply menu item and exit from menu

## Authors

* **Alexander Savychev** - *Initial work* - [save2love](https://github.com/save2love)

See also the list of [contributors](https://github.com/save2love/resource-monitor/graphs/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
