# EI-Agent
IoT/EI-PaaS Agent framework for IoT/EI-PaaS server.

## DESCRIPTION
----

EI-Agent – a software development framework to communicate between device and IoT/EI-PaaS Server.
Advantech provides a software development framework to communicate and exchange information between a device and IoT/EI-PaaS Server, called a EI-Agent framework.
The EI-Agent framework provides a rich set of user-friendly, intelligent and integrated interfaces, which speeds development, enhances security and makes agent application easier and simpler to communicate with IoT/EI-PaaS Server.
The framework has three benefits:

Standardization - The communication protocol is based on the MQTT protocol to communicate and exchange data with IoT/EI-PaaS Server. The IoT sensor data report format is following the IPSO Spec. in JSON format.
Portability - Whole framework is written in C language and follow the ANSI C Standard, that C compilers are available for most systems and are often the first compiler provided for a new system.
Scalability - The EI-Agent Framework is functional partitioning into discrete scalable, reusable modules, and plug & playable.

For scalability, Advantech implements several Software Modules to access sensor data or control target device, we called Plugins (or Handlers we called before).

Each plugin can be designed to handle specific jobs, such as:

Sensor Plugin: the plugin access sensor data through Sensor driver or 3rd party library, or
Remote Control Plugin:  the plugin execute remote command on target device.
Based on those Plugins, Advantech also defined a set of APIs, called Plugin (Handler) APIs, to support custom plugins implementation.

User can implement their own Handler with these Plugin (Handler) APIs to access their sensor data or control their devices and plugged by EI-Agent to communicate with IoT/EI-PaaS Server.

## OVERVIEW
----

 The EI-Agent includes:
 
 Library:
  * libWISECore.so
	- Core library of EI-Agent to handshake with IoT/EI-PaaS Server.
 
  * libSAClient.so
    - Manage the connection connected by WISE Core.
	 
  * libSAManager.so
    - Integrate whole EI-Agent library
	 
  * libSAHandlerLoader.so:
    - Dynamic load and manage Handlers

Application:     
  * Sample\SampleClient
    - Sample application for Windows only
     
  * Sample\SampleAgent
    - Sample application for both Windows and Linux
    
  * Sample\WISECoreSample:
    - Lightweight agent sample application for both Windows and Linux.
     
Plugins:
 * Sample\HandlerSample:
   - Sample Plugin to read a JSON file and upload to IoT/EI-PaaS Server for both Windows and Linux.

 * Sample\SampleHandler:
   - Sample Plugin to read a JSON file and upload to IoT/EI-PaaS Server for Windows only.
	 
 * Sample\Modbus_Handler:
   - Network Monitor Handler for network status report.
	 
Decuments:
 * doc\IoT/EI-PaaS_Agent_Command_Format_DG_V1.1.pdf:
   - document for EI-Agent Framework command format.
	 
 * doc\IoT/EI-PaaS_Agent_DG_V1.0.pdf:
   - document for EI-Agent Framework programming guide.
	 
 * doc\IoT/EI-PaaS_Agent_UM.pdf
   - document for EI-Agent User Manual. 
	 
## PATENTS
----

## COMPILATION
----
 * Windows - Visual Studio 2008 
   1. Open "WISEAgent.sln"
   2. In Solution Explore, Set "WISECoreSample" project as Startup Project
   3. Rebuild all soltion by click build->Rebuild Solution
   4. The executable binary file will be generated in Release folder. 
   
 * Linux - CentOS
   1. Execute pre-install-centos.sh as a super user 
   2. Execute build-wiseagent.sh
   3. The executable binary file will be generated in Release folder. 
   
 * Linux - Ubuntu
   1. Execute pre-install-ubuntu.sh as a super user 
   2. Execute build-wiseagent.sh
   3. The executable binary file will be generated in Release folder. 
  
 * To compile this package under other Unix systems, user need install or pre-compile the following libraries:
   - libXml2-2.7.8
   - openssl-1.0.1h
   - curl-7.37.1
   - mosquitto-1.3.2
   - autoconf
   - automake
   - make
   - libx86 
   
## Hardware requirements
----

* CPU architecture
  - x86
  - ARM
 
## OS
----

 * Windows
   - XP, 7, 8, 10

 * Linux
   - Ubuntu ( 14.04.2 X64)
 
## PROBLEMS
----

## SUPPORT
----

 * [Advantech IoT Developer Forum](http://iotforum.advantech.com/)
 * [WIKI Documents](http://ess-wiki.advantech.com.tw/view/WISE-PaaS_2.0)
 
## License
----

Copyright (c) Advantech Corporation. All rights reserved.

