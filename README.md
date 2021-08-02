Lamppost Autonomous Car Assistant
==================================

- [Introduction](#introduction)
- [Dependencies](#dependencies)
- [Getting Start](#getting-start)
    - [1. Launch Lamppost Host Program](#launch-lamppost-host-program)
    - [2. Use lmphostctl to manage the system](#use-lmphostctl-to-manage-the-system)
    - [3. Editing configuration files](#editing-configuration-files)
    - [4. Start DEMO A-Z](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/wiki/Run-Demo-A-Z)
- [Backend](#backend)
    - [1. Lamppost Alive Status Table](#lamppost-alive-status-table)
    - [2. Detected Roadblocks Table](#detected-roadblocks-table)
    - [3. Route Table](#route-table)
    - [4. Grafana Frontend (ongoing)](#)
- [Developer Guide](#contribute)

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

Please secure the wiring of the raspberry pi before starting the system
 - connect to power via an USB type-c cable. 
 - connect the GPS receiver with USB
 - connect to the RoboMaster ep1 controller with USB (to robomaster ep1's micro USB interface)

When both the robomaster and raspberry pi are powered up, raspberry pi will become an AP with following spec:

```text
SSID: ep1-pi
PASSWD: netcod99
```

**PLEASE only connect to this AP (and ignore the AP hosted by robomaster, because the control of robomaster are already proxied through the raspberry pi)**

The Raspberry pi has static ip 192.168.57.1, if you can connect to the AP, you will obtain an 192.168.57.0/24 ip. 

**If you have multiple network adapter, PLEASE NOT let 192.168.57.1 to be your default route, instead, add a custom route to 192.168.42.0/24 via 192.168.57.1
** This is for control the robomaster ep1.

The raspberry pi will provides the following service:

```
SSH @ port 22 
	username: pi
	password: raspberry
EXAMPLE:
	ssh pi@192.168.57.1
	
	
GPS NMEA message @ port 6001
	please use an NMEA parser to read the message
EXAMPLE:
	// the following command will print the raw message of GPS NMEA data
	telnet 192.168.57.1 6001
	
	
Robomaster at RNDIS mode @ 192.168.42.2
	please read the robomaster SDK document, and make sure you has setup route to 192.168.42.0/24 via 192.168.57.1
	you may test the reachibility of robomaster:
	ping 192.168.42.2
	PLEASE also make sure add the following line before using python SDK
	robot.config.LOCAL_IP_STR = "192.168.57.XXX"
	where "192.168.57.XXX" need to be replace by your ip.
```

Currently the following utility are useful:

```text
You may run the following on a device connected to raspberry pi's wifi

ep.py	may connect the robomaster ep, and make it follows a certain path
parseXML.py	read the xml GPS file and initialize the coordinate transform
db_service.py	read the path info from the mysql server
gps_read.py	read and print the GPS infomation
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
- **Cam\[1,2\]RefMarker**: ID of the ArUco marker used for reference
- **MarkerSize**: actual size of the ArUco board (unit: meter)
- **FrameIncrement**: frame count between two single ArUco detections, default to be 100 (frames).
- **CalibrationFile**: file to store the camera calibration result (camera matrix and distortion matrix). Detailed
  description sees [section: Camera calibration file](#camera-calibration-file)
- **GPSReferenceFile**: file to store the GPS of reference markers, GPS of origin point and the geographical metrics.
  Detailed description sees [section: GPS calibration file](#gps-reference-file)

#### \[Database\] section:

***Note: this section must be filled correctly when launch in root node.***

This section describes how the root program connects the MySQL database to store the analytical data.

The subdomain of this section include:

- **Host**: host address of the MySQL database
- **User**: user of the MySQL database
- **Password**: credential of the database
- **Database**: schema of the database

### \[Arena\] section:

***Note: this section must be filled correctly when launch in root node.***

This section describes the geographical settings of the demo, i.e. what is the width and the height of the demo arena,
and what is the actual size of each grid in employing AStar path finding algorithm.

### GPS reference file

The GPS reference file is a xml file that stores the basic information of the GPS of reference markers, GPS of origin
point and the geographical metrics. A sample file is provided in `<proj_root>/_config/gpsref.xml`.

```xml
<?xml version="1.0"?>
<RefPoints>
    <RefPoint>
        <aruco_id>10</aruco_id>
        <x>1.0</x>
        <y>0.0</y>
    </RefPoint>
    <Origin>
        <longitude>22.0</longitude>
        <latitude>114.0</latitude>
    </Origin>
    <Metric>
        <longitude_diff_in_xm>1.0</longitude_diff_in_xm>
        <longitude_diff_in_ym>0.0</longitude_diff_in_ym>
        <latitude_diff_in_xm>0.0</latitude_diff_in_xm>
        <latitude_diff_in_ym>1.0</latitude_diff_in_ym>
    </Metric>
</RefPoints>
```

### Camera calibration file

According to the pin-hole camera model, the first step of reconstructing 2D camera coordinates into the 3D world
coordinates is camera calibration. The camera calibration process produces 2 matrix, camera matrix and distortion
matrix and stores them into an xml file. 

A sample calibration file is stored in `<proj_root>/webcam_640x480.xml`

```xml
<?xml version="1.0"?>
<opencv_storage>
<calibration_time>"Wed Jun 30 04:05:17 2021"</calibration_time>
<nr_of_frames>15</nr_of_frames>
<image_width>640</image_width>
<image_height>480</image_height>
<board_width>12</board_width>
<board_height>8</board_height>
<square_size>70.</square_size>
<fix_aspect_ratio>1.</fix_aspect_ratio>
<flags>6158</flags>
<fisheye_model>0</fisheye_model>
<camera_matrix type_id="opencv-matrix">
  <rows>3</rows>
  <cols>3</cols>
  <dt>d</dt>
  <data>
    5.3368467133099159e+02 0. 320. 0. 5.3368467133099159e+02 240. 0. 0.
    1.</data></camera_matrix>
<distortion_coefficients type_id="opencv-matrix">
  <rows>5</rows>
  <cols>1</cols>
  <dt>d</dt>
  <data>
    1.0183631577296211e-01 -6.7735104715374406e-01 0. 0.
    8.4581480933649966e-01</data></distortion_coefficients>
<avg_reprojection_error>1.0449172724416682e+00</avg_reprojection_error>
</opencv_storage>
```

---

# Backend

Simple version for now.

## Lamppost Alive Status Table

```shell
mysql> select * from Lamppost;
+---------+---------------------+
| addr    | last_seen           |
+---------+---------------------+
| 0.0.1.0 | 2021-07-14 19:07:25 |
| 0.0.1.1 | 2021-07-14 19:07:25 |
| 0.0.1.2 | 2021-07-14 19:07:25 |
| 0.0.1.3 | 2021-07-14 19:07:25 |
+---------+---------------------+
4 rows in set (0.01 sec)
```

## Detected Roadblocks Table

```shell
mysql> select * from RoadBlocks;
+---+---+--------+---------------------+
| x | y | ref_id | last_seen           |
+---+---+--------+---------------------+
| 0 | 0 |      0 | 2021-07-14 19:07:25 |
+---+---+--------+---------------------+
1 row in set (0.01 sec)
```

## Route Table

```shell
mysql> select * from Route;
+--------------------+---------------------+---------+
| x                  | y                   | pace_id |
+--------------------+---------------------+---------+
|                  5 |                   0 |       1 |
|   5.09997787475586 |                 0.1 |       2 |
|  5.199973297119142 |                 0.2 |       3 |
|  5.299928808212281 | 0.30000000000000004 |       4 |
|  5.399786925315857 |                 0.4 |       5 |
|  5.499417626857758 |                 0.5 |       6 |
|  5.598462373018265 |  0.6000000000000001 |       7 |
|  5.695969235897065 |  0.7000000000000001 |       8 |
|  5.789444467425347 |                 0.8 |       9 |
|  5.872362338006497 |                 0.9 |      10 |
|  5.927634514123202 |                   1 |      11 |
|  6.010582527704537 |                 1.1 |      12 |
|  6.103994816914202 |  1.2000000000000002 |      13 |
| 6.2015511612873535 |  1.3000000000000003 |      14 |
|  6.300594513816759 |  1.4000000000000001 |      15 |
|  6.400206445832738 |                 1.5 |      16 |
| 6.5000007943250235 |                 1.6 |      17 |
|  6.599924094532616 |  1.7000000000000002 |      18 |
|  6.699664982734248 |  1.8000000000000003 |      19 |
|  6.799011076281749 |  1.9000000000000001 |      20 |
|   6.89754554664869 |                   2 |      21 |
|  6.993542269321006 |                 2.1 |      22 |
|  7.083039906195973 |                 2.2 |      23 |
|  7.155554457306062 |  2.3000000000000003 |      24 |
|  7.183764724418825 |  2.4000000000000004 |      25 |
|  7.195594770499634 |                 2.5 |      26 |
|  7.203077554234335 |                 2.6 |      27 |
|  7.213657920672441 |                 2.7 |      28 |
|  7.237918200142108 |  2.8000000000000003 |      29 |
| 7.2999691970773934 |  2.9000000000000004 |      30 |
|  7.362097268976564 |                   3 |      31 |
|  7.386377219475239 |                 3.1 |      32 |
|  7.396906214625877 |                 3.2 |      33 |
|  7.404264395516076 |  3.3000000000000003 |      34 |
|     7.416201229047 |  3.4000000000000004 |      35 |
|  7.444099643600075 |                 3.5 |      36 |
|  7.516154709673283 |                 3.6 |      37 |
|  7.604244287061766 |                 3.7 |      38 |
|  7.696898238802094 |  3.8000000000000003 |      39 |
|  7.786059975265647 |  3.9000000000000004 |      40 |
|   7.86161887280633 |                   4 |      41 |
|  7.898523776011013 |                 4.1 |      42 |
|   7.93408566007135 |   4.199999999999999 |      43 |
|  8.003864531454607 |                 4.3 |      44 |
|  8.077199955729665 |                 4.4 |      45 |
|  8.127952636835886 |                 4.5 |      46 |
|  8.206639356736066 |                 4.6 |      47 |
|  8.291918900612187 |                 4.7 |      48 |
|  8.369116762683635 |   4.800000000000001 |      49 |
|  8.415444862282662 |                 4.9 |      50 |
|  8.477223459776702 |                   5 |      51 |
|  8.516223832237339 |                 5.1 |      52 |
|  8.571445018986065 |                 5.2 |      53 |
|  8.598109803727418 |   5.300000000000001 |      54 |
|  8.622890590650314 |                 5.4 |      55 |
|  8.670519872537916 |                 5.5 |      56 |
|  8.688780907745606 |                 5.6 |      57 |
|  8.695716218394931 |                 5.7 |      58 |
|    8.6983143038047 |   5.800000000000001 |      59 |
|  8.699414264830175 |                 5.9 |      60 |
|  8.699796790073488 |                   6 |      61 |
|  8.700087447684652 |                 6.1 |      62 |
|  8.700216874477004 |                 6.2 |      63 |
|  8.700799937608593 |   6.300000000000001 |      64 |
|  8.702155501208999 |                 6.4 |      65 |
|  8.705610002313737 |                 6.5 |      66 |
|  8.714676891001925 |                 6.6 |      67 |
|  8.738386418003014 |                 6.7 |      68 |
|  8.800656096337084 |   6.800000000000001 |      69 |
|  8.863265302879295 |                 6.9 |      70 |
|  8.889516918114222 |                   7 |      71 |
|  8.904935951841665 |                 7.1 |      72 |
|  8.925620028486904 |                 7.2 |      73 |
|  8.971578644825609 |   7.300000000000001 |      74 |
|  8.989462018052222 |                 7.4 |      75 |
|  8.996468278595897 |                 7.5 |      76 |
|  9.000280269474235 |                 7.6 |      77 |
|  9.004084293858183 |                 7.7 |      78 |
|  9.012099073416548 |   7.800000000000001 |      79 |
|  9.032351987150259 |                 7.9 |      80 |
|  9.084603511880424 |                   8 |      81 |
|  9.121843970468632 |   8.100000000000001 |      82 |
|  9.180594019342294 |   8.200000000000001 |      83 |
|  9.220270868372598 |                 8.3 |      84 |
|  9.279869536749514 |                 8.4 |      85 |
|  9.319680170983641 |                 8.5 |      86 |
|   9.37883061151377 |   8.600000000000001 |      87 |
|  9.417151664267923 |   8.700000000000001 |      88 |
|  9.472316063357978 |                 8.8 |      89 |
|  9.500015813189583 |                 8.9 |      90 |
|  9.527644411505221 |                   9 |      91 |
|  9.582910682023703 |   9.100000000000001 |      92 |
|  9.621103326598153 |   9.200000000000001 |      93 |
|  9.680402435792718 |                 9.3 |      94 |
|  9.720101267245521 |                 9.4 |      95 |
|  9.779899733619324 |                 9.5 |      96 |
|  9.819597913481005 |   9.600000000000001 |      97 |
|  9.878894376349734 |   9.700000000000001 |      98 |
|  9.917085393250371 |                 9.8 |      99 |
|  9.972361808107662 |                 9.9 |     100 |
|                 10 |                  10 |     101 |
+--------------------+---------------------+---------+
101 rows in set (0.00 sec)
```
---

# Contribute

- Upcoming features and pending bugs: [here](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/issues)
- The architectural design of this
  system: [here](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/wiki/System-Architecture)
- Build development
  environment: [here](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/wiki/Build-dev-environment)
- Simulation tutorial: [here](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/wiki/Simulation)
- Start demo A-Z: [here](https://github.com/Vito-Swift/Infra-Autopilot-Assistant/wiki/Run-Demo-A-Z)
- Shall you have any questions, contact [](mailto://)