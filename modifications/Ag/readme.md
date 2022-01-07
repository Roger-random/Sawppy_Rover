
# BOM (mostly) open hardware

- ~£400 Open source [Jetson baseboard](https://capablerobot.com/products/nx-baseboard/) start with a Nano compute module, upgrade if needed. 
- ~£300 [Swappy/Tenacity rover body](https://github.com/jetdillo/tenacity_rover#readme)
- ~£40 [Improved suspension from Small Robot Army](https://twitter.com/SmallRobotArmy/status/1476667953546346530)
- ~£20 [Open Core running uROS](https://github.com/rosmo-robot/Open-Core-M5stack/blob/main/README.md)
- ~£90 [3 x dual channel 5A SimpleFOC motor controllers](https://github.com/rosmo-robot/Rosmo_ESC)
- ~£240 [6x 4108 BLDC motor TBC](https://s.click.aliexpress.com/e/_AE2SCu)
- ~70 [4 x 35kg servo](https://www.hiwonder.hk/products/hiwonder-hts-35h-high-voltage-bus-servo-35kg-torque-with-data-feedback)
- ~£100+ Jehu [18650 battery pack](https://jag35.com/collections/pcb-based-products/products/high-power-18650-battery-module-diy-pcb-kit-75x)

# optional extras
- ~£170 [ZED-F9P Sparkfun RTK](https://www.ardusimple.com/rtk-open-source-hardware/) Or maybe [$$Ark](https://arkelectron.com/product/ark-rtk-gps/)
- ~£100 [Tinkerforge IMU](https://www.tinkerforge.com/en/shop/bricks/imu-v2-brick.html) [ROS2 ](https://discourse.ros.org/t/ros-tinkerforge-imu-v2-bricks-driver/15539)
- ~£200 Hang a [Nindmani Delta](https://github.com/AutoRoboCulture/nindamani-the-weed-removal-robot)
- ~£40 Meshtastic for [RTK comms](https://meshtastic.discourse.group/) latency a problem?

# OS / Basic nav
- [Dashing on Jetson Nano](https://github.com/ANI717/Headless-Jetson-Nano-Setup)
- [Tensorflow model to follow crop rows - Outputs Cmd_vel](https://github.com/ANI717/ANI717_Robotics#design-diagram)
- [Swappy ROS2 Rover for Ackerman drive - Subscribes Cmd_vel](https://github.com/mgonzs13/ros2_rover)

# Sign up
If you're interested in developing Swappy for BLDC/ ROS2 please [comment here](https://github.com/samuk/Sawppy_Rover/discussions)



# Notes
- [ROS2 RTK](https://github.com/aussierobots/ublox_dgnss) - [Teb-Local-Planner](https://github.com/rst-tu-dortmund/teb_local_planner/tree/foxy-devel)
- [Radxa CM3 module](https://www.cnx-software.com/2021/11/07/radxa-cm3-raspberry-pi-cm4-alternative/) in [Carrier board](https://hackaday.io/project/165108-carrier-board-for-the-raspberry-pi-compute-module)
- [Robot_localization](https://github.com/cra-ros-pkg/robot_localization/tree/ros2)

# Ag Navigation notes (later)

![Visual & ML](https://pbs.twimg.com/media/FIRSEUpXoA8Sf_V?format=jpg&name=900x900)

The core of this navigation strategy is the VisualServoing 

- [visual servoing](https://github.com/PRBonn/visual-crop-row-navigation#readme) 
- [https://github.com/NMBURobotics/vox_nav](vox_nav)
- [Acorn Motherboard?](https://github.com/Twisted-Fields/acorn-robot-electronics/blob/main/README.md) Unclear what value added if going ROS2 route.
- [ROS1 earth_rover_localization](https://github.com/earthrover/earth_rover_localization/tree/master/earth_rover_localization)
