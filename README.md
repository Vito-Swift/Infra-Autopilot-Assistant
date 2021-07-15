Lamppost Autonomous Car Assistant
==================================

- [Introduction](#introduction)
- [Dependencies](#dependencies)
- [Getting Start](#getting-start)
    - [1. Launch Lamppost Host Program](#launch-lamppost-host-program)
    - [2. Use lmphostctl to manage the system](#use-lmphostctl-to-manage-the-system)
    - [3. Editing configuration files](#editing-configuration-files)
- [Backend](#backend)
    - [1. Lamppost Alive Status Table](#lamppost-alive-status-table)
    - [2. Detected Roadblocks Table](#detected-roadblocks-table)
    - [3. Route Table](#route-table)
    - [4. Grafana Frontend (ongoing)](#)
- [Developer Guide](#contribute)

-----------------------------------

# Introduction

This project is a demo system using BATS Transmission Protocol (BTP) to provide fast data transmission in an
edge-autocar system. The system is consists of multiple edge nodes installed on the roadside lampposts and an autonomous
vehicle (we use Dji Robomaster as a tentative solution). The system aims to utilize the sensors and cameras installed
equipped on the edge node for improving the self route planning of autonomous vehicles. For now, our system could
imparts the vehicles with the locations of the roadblocks and so on. We use optical SLAM technique (
in specific, ArUco marker library) to detect the roadblocks.

![aruco with lamppost](https://github.com/Vito-Swift/RefImgs/raw/main/WX20210630-182518%402x.png)

---

# Dependencies

Our program has dependencies to following packages / softwares, please ensure that they are correctly installed on the
machine so that cmake and other linking tools could find them.

- OS requirements: Ubuntu 18.04 or newer version
- BATS Transmission Protocol, version 1.0 (***currently undisclosed to public, sorry for that***)
    - Note that aside of installing the BATS Coding Library one should also have BATS Socket Interface installed in
      system level. One could refer to LINE 129 to LINE 141 of
      this [Dockerfile](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/blob/dev/Dockerfile), where we install
      the interfaces by adding a couple lines of code into the CMakeLists.txt when building the debug Docker image.
- OpenCV, version 4.1.0
- OpenCV-contrib
- ArUco marker mapper library
- MySQL Client, version 8.0
- MySQL C++ Connector

Alternatively, one could also refer to
this [Dockerfile](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/blob/dev/Dockerfile) for inspecting how we set
up the environment on a fresh bare.

---

# Getting Start

This section will answer following questions:

- [How we design the system and what are the role each program plays in it?](#system-architecture)
- [How to deploy the programs onto the lamppost system](#launch-lamppost-host-program)
- [How to setup the Robomaster for control and self-localization](#configure-robomaster-and-raspberrypi)
- [How to control and manage the lamppost system](#use-lmphostctl-to-manage-the-system)
- [How to configure the setup parameters](#editing-configuration-files)

## System Architecture

The entire system consists of 3 sub-programs:

- **Lamppost Host Program** (runs on the root node and slave node)
    - *Source files are placed under `<proj_root>/src/LPHost` directory*
- **Control Program** (runs on the root node to control the Robomaster)
    - *Source files are placed under `<proj_root>/src/Ctrl` directory*
- **lmphostctl** (runs on the root to control interact with the entire system)
    - *Source files are placed in `<proj_root>/src/lmphostctl.cc`*

The interaction behavior of each module illustrate in the diagram below.

![AutocarSysArch](https://github.com/Vito-Swift/RefImgs/raw/main/AutocarSysArch.svg)

## Launch Lamppost Host Program

Fetch the source code of this project and built them into binary:

```shell
> git clone https://github.com/Vito-Swift/Infra-Autopilot-Assistant.git
> cd Infra-Autopilot-Assistant && mkdir build && cd build
> cmake .. && make -j4
```

Two binary executables will be generated after the compiling, one is `LamppostHost` which will be deployed onto each
edge node, and another is `lmphostctl` which will be used as control interface for users to interact with the deployed
system.

```shell
> ./LamppostHost -h
Usage: ./LamppostHost [OPTIONS]

Options:

    --config_file [CONFIG_FILE]
                    Path to configuration file
    --mock_detection
                    Mock roadblock detection by
                    continually sending (0,0) to
                    root lamppost node


Example:
    > ./LamppostHost --config_file ../_config/lamp1.ini
```

`LamppostHost` program accepts two parameters as input:

- `--config`: path to a *.ini config file. Please refer to [this section](#editing-configuration-files) to learn how to
  write or edite configuration files;
- `--mock_detection`: mock the Road block detection thread, i.e. when this option is enabled, the program will keep
  sending (0,0) to the root lamppost at each single seconds.

## Configure RoboMaster and RaspberryPI

```
to be add on by Morris
```

## Use lmphostctl to manage the system

```text
Analyzing environments and generating road map
Generating Route
	 from: (5, 0) to (10, 10)
	 transformed map from: (50, 0) to (100, 100)
The destination cell is found

The Path is -> (50,0) -> (51,1) -> (52,2) -> (53,3) -> (54,4) -> (55,5) -> (56,6) -> (57,7) -> (58,8) -> (59,9) -> (59,10) -> (60,11) -> (61,12) -> (62,13) -> (63,14) -> (64,15) -> (65,16) -> (66,17) -> (67,18) -> (68,19) -> (69,20) -> (70,21) -> (71,22) -> (72,23) -> (72,24) -> (72,25) -> (72,26) -> (72,27) -> (72,28) -> (73,29) -> (74,30) -> (74,31) -> (74,32) -> (74,33) -> (74,34) -> (74,35) -> (75,36) -> (76,37) -> (77,38) -> (78,39) -> (79,40) -> (79,41) -> (79,42) -> (80,43) -> (81,44) -> (81,45) -> (82,46) -> (83,47) -> (84,48) -> (84,49) -> (85,50) -> (85,51) -> (86,52) -> (86,53) -> (86,54) -> (87,55) -> (87,56) -> (87,57) -> (87,58) -> (87,59) -> (87,60) -> (87,61) -> (87,62) -> (87,63) -> (87,64) -> (87,65) -> (87,66) -> (87,67) -> (88,68) -> (89,69) -> (89,70) -> (89,71) -> (89,72) -> (90,73) -> (90,74) -> (90,75) -> (90,76) -> (90,77) -> (90,78) -> (90,79) -> (91,80) -> (91,81) -> (92,82) -> (92,83) -> (93,84) -> (93,85) -> (94,86) -> (94,87) -> (95,88) -> (95,89) -> (95,90) -> (96,91) -> (96,92) -> (97,93) -> (97,94) -> (98,95) -> (98,96) -> (99,97) -> (99,98) -> (100,99) -> (100,100) 
Optimizing generated route

Optimized route is: -> (5.000000, 0.000000) -> (5.099978, 0.100000) -> (5.199973, 0.200000) -> (5.299929, 0.300000) -> (5.399787, 0.400000) -> (5.499418, 0.500000) -> (5.598462, 0.600000) -> (5.695969, 0.700000) -> (5.789444, 0.800000) -> (5.872362, 0.900000) -> (5.927635, 1.000000) -> (6.010583, 1.100000) -> (6.103995, 1.200000) -> (6.201551, 1.300000) -> (6.300595, 1.400000) -> (6.400206, 1.500000) -> (6.500001, 1.600000) -> (6.599924, 1.700000) -> (6.699665, 1.800000) -> (6.799011, 1.900000) -> (6.897546, 2.000000) -> (6.993542, 2.100000) -> (7.083040, 2.200000) -> (7.155554, 2.300000) -> (7.183765, 2.400000) -> (7.195595, 2.500000) -> (7.203078, 2.600000) -> (7.213658, 2.700000) -> (7.237918, 2.800000) -> (7.299969, 2.900000) -> (7.362097, 3.000000) -> (7.386377, 3.100000) -> (7.396906, 3.200000) -> (7.404264, 3.300000) -> (7.416201, 3.400000) -> (7.444100, 3.500000) -> (7.516155, 3.600000) -> (7.604244, 3.700000) -> (7.696898, 3.800000) -> (7.786060, 3.900000) -> (7.861619, 4.000000) -> (7.898524, 4.100000) -> (7.934086, 4.200000) -> (8.003865, 4.300000) -> (8.077200, 4.400000) -> (8.127953, 4.500000) -> (8.206639, 4.600000) -> (8.291919, 4.700000) -> (8.369117, 4.800000) -> (8.415445, 4.900000) -> (8.477223, 5.000000) -> (8.516224, 5.100000) -> (8.571445, 5.200000) -> (8.598110, 5.300000) -> (8.622891, 5.400000) -> (8.670520, 5.500000) -> (8.688781, 5.600000) -> (8.695716, 5.700000) -> (8.698314, 5.800000) -> (8.699414, 5.900000) -> (8.699797, 6.000000) -> (8.700087, 6.100000) -> (8.700217, 6.200000) -> (8.700800, 6.300000) -> (8.702156, 6.400000) -> (8.705610, 6.500000) -> (8.714677, 6.600000) -> (8.738386, 6.700000) -> (8.800656, 6.800000) -> (8.863265, 6.900000) -> (8.889517, 7.000000) -> (8.904936, 7.100000) -> (8.925620, 7.200000) -> (8.971579, 7.300000) -> (8.989462, 7.400000) -> (8.996468, 7.500000) -> (9.000280, 7.600000) -> (9.004084, 7.700000) -> (9.012099, 7.800000) -> (9.032352, 7.900000) -> (9.084604, 8.000000) -> (9.121844, 8.100000) -> (9.180594, 8.200000) -> (9.220271, 8.300000) -> (9.279870, 8.400000) -> (9.319680, 8.500000) -> (9.378831, 8.600000) -> (9.417152, 8.700000) -> (9.472316, 8.800000) -> (9.500016, 8.900000) -> (9.527644, 9.000000) -> (9.582911, 9.100000) -> (9.621103, 9.200000) -> (9.680402, 9.300000) -> (9.720101, 9.400000) -> (9.779900, 9.500000) -> (9.819598, 9.600000) -> (9.878894, 9.700000) -> (9.917085, 9.800000) -> (9.972362, 9.900000) -> (10.000000, 10.000000)
Route is generated successfully
Uploading planned path to the database
```

## Editing configuration files

### Lamppost Host Configuration file

When `LamppostHost` program launches, the program needs to read an ini file for configuration. This subsection
introduces each domain of the configuration file.

Sample configuration files are shown in `<proj_root>/_config` directory.

```ini
[Networking]
LocalBATSAddr = 0.0.1.0
RootBATSAddr = 0.0.1.0
RootBATSPort = 100
IsRootNode = 1

[RBDetection]
Camera1 = udp://0.0.0.0:8000/
Camera2 = udp://0.0.0.0:8001/
Cam1RefMarker = 10
Cam2RefMarker = 10
MarkerSize = 0.5
FrameIncrement = 100
CalibrationFile = _config/webcam_640x480.xml
GPSReferenceFile = _config/gpsref.xml

[Database]
Host = 10.26.1.16
User = lamppost-autopilot
Database = lamppost_runtime
Password = netcod99

[Arena]
Width = 20.0
Height = 20.0
GridSize = 0.1
```

#### \[Networking\] section:

This section describes the BATS Protocol level parameters, e.g. the BATS address of the machine to be launching program,
the BATS address of the root machine, etc.

Valid subdomains include:

- **LocalBATSAddr**: string of address
- **RootBATSAddr**: string of address
- **RootBATSPort**: port opened on root node for receiving RBCoordinates from slaves.
- **IsRootNode**: specifies whether the machine to be launching is the root node. Default to be false.

#### \[RBDetection\] section:

This section describes the basic parameters fo Roadblock Detection related routines, e.g. the address of video stream,
the ID of the ArUco marker used for reference (one per video stream), the size of the marker, etc.

Valid subdomains include:

- **Camera\[1,2\]**: address of the video stream
- **

This section describes the configuration of the

### GPS calibration file

### Camera calibration file

---

# Backend

## Lamppost Alive Status Table

## Detected Roadblocks Table

## Route Table

# Contribute

- Upcoming features and pending bugs: [here](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/issues)
- The architectural design of this
  system: [here](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/wiki/System-Architecture)
- Build development
  environment: [here](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/wiki/Build-dev-environment)
- Simulation tutorial: [here](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/wiki/Simulation)