# 🌲 Smart Fires Rover Project

### Overview
The Smart Fires Rover is an autonomous uncrewed ground vehicle (UGV) designed for environmental
monitoring and forest health assessment. Developed as part of an undergraduate research initiative, 
this project utilizes a custom ROS 2 navigation and perception stack to traverse forested terrain, 
collect spatial data, and monitor ecological metrics.


### ⚙️ Hardware Architecture
The rover operates on a decentralized sensor array processed by a central compute module:
* **Core Compute:** Raspberry Pi 5 
* **Perception:** Lidar for 2D/3D spatial mapping and obstacle avoidance.
* **Localization:** GPS for global positioning in remote environments.

### 💻 Software Stack
* **OS:** Ubuntu 
* **Framework:** ROS 2 Jazzy
* **Key Packages:** `Nav2` (Navigation), `slam_toolbox` (Mapping), `robot_localization` (Sensor Fusion)

### 🗺️ System Architecture
The software architecture relies on a modular ROS 2 node graph, handling sensor data ingestion, SLAM (Simultaneous Localization and Mapping), 
and autonomous path planning. 
