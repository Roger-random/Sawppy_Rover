# Sawppy the Rover

Sawppy is a motorized model of Mars rovers Curiosity and Perseverance. It faithfully reproduces
the [Rocker-Bogie suspension kinematics](https://en.wikipedia.org/wiki/Rocker-bogie) of real
rovers and is intended to be a hardware platform for future software projects in autonomous
operation. Go forth and boldly explore the back yard, Sawppy!

All information for building Sawppy is free and public open source for anyone to build their
own. Build instructions can be found
[under the docs subdirectory](docs/).

Sawppy the Rover was inspired by [JPL's Open Source Rover project](https://opensourcerover.jpl.nasa.gov). 
Most of the differences between Sawppy and its JPL inspiration were motivated by a desire to reduce cost
and complexity. JPL's rover is designed for education, to be assembled by a school team and give a robust
foundation for structured curriculum. Sawppy is more suited for individual hobbyists like myself who are
happy to tinker and willing to make some trade-offs to lower cost.

The budget was __$500__, see [price breakdown here](docs/Price%20breakdown.md).
Getting to that price point required the following changes:

* Motorization: Instead of using gearmotors with encoders managed by RoboClaw motor controllers,
moving the wheels will be done with serial bus servo motors. For more information on these devices,
please see [this article on Hackaday.com](https://hackaday.com/2018/07/05/wrangling-rc-servos-becoming-a-hassle-try-serial-bus-servos/).
* Construction Method: Instead of using the [Actobotics construction system](https://www.servocity.com/actobotics),
Sawppy will be built from [Misumi HFS3 aluminum extrusions](https://us.misumi-ec.com/vona2/detail/110300465870/)
connected by 3D-printed plastic parts. For more information on this construction technique, please
see [this article on Hackaday.com](https://hackaday.com/2018/05/08/how-to-build-anything-out-of-aluminum-extrusion-and-3d-printed-brackets/).

These two major design goals can be summarized as: **S**ervo **A**ctuated **W**heels,
**P**rinted **I**nterconnect **F**or **E**xtrusion. The acronym SAWPIFE led to the nickedname "Sawppy".

# Development Status

**Sawppy version 1.0** 

This milestone includes a motorized rolling chassis that is mechanically functional.
The remaining areas (electrical, software, etc.) are still very immature, just barely enough
to validate mechanical chassis function. Click this image for a YouTube video illustrating
the chassis in action, climbing a backpack that is almost double the height of wheel diameter.

[![Sawppy conquers a backpack](https://img.youtube.com/vi/SEBBMDWgtC4/0.jpg)](https://www.youtube.com/watch?v=SEBBMDWgtC4)

**Sawppy version 1.1**

Sawppy has received only minor mechanical changes for this milestone, including the rocker joint
angle limiter visible in action in the backpack demo video above. Most of the attention has been
on software, with contributions by the Sawppy community.

A Sawppy builder can now choose from many Sawppy software options. Roughly in order of power and complexity, they are:

* **Simplest**: Wired control [running on an Arduino](https://github.com/Roger-random/Sawppy_Rover/tree/master/arduino_sawppy) instead of Raspberry Pi. Bare-bones version of Sawppy geometry calculations. Wired control meant skipping all the overhead of wireless communication. Cutting out all HTML code also meant this is a good basis for other control schemes: send desired speed and direction into Arduino and let it handle Sawppy chassis geometry calculation.
* **Original**: HTML-based wireless teleoperation software stack [modified from SGVHAK rover](/docs/SGVHAK%20Rover%20Software.md). This was written to be easy for others to understand and modify.
* **R/C control**: [lightly modified from my SGVHAK rover software](https://github.com/mw46d/SGVHAK_Rover) by Marco Walther (mw46d) for a Raspberry Pi-based way to interface with traditional remote control receivers.

Plus two options for turning Sawppy into a [ROS](http://ros.org) robotics platform.

* **ROS Kinetic**: [heavily modified from my SGVHAK rover software](https://github.com/mw46d/Sawppy_ROS) by Marco Walther (mw46d) which translates ROS `/cmd_vel` commands into Sawppy movement. This is a good stepping stone beyond original Sawppy software.
* **ROS Melodic**: [a ground-up rewrite of a ROS-centric stack](https://github.com/srmainwaring/curio) by Rhys Mainwaring (srmainwaring) is extensive and powerful. Going beyond responding to `/cmd_vel` commands, it also calculates `/odom` by interpolating LX-16A position encoder ~270 degree feedback into full 360 degrees. Plus visualizing rover state in RViz, and files to put a digital Sawppy in Gazebo robot simulation environment.

# Modifications From Rover Builders

Many people who built their own Sawppy rover are also people who love to tinker
and experiment. As a result these rovers reflect the individuality of their
builder. Some of these builders have shared their work with the community,
see the [`modifications` folder](/modifications) for more information.

# Other Project Resources

* [Project page on Hackaday.io](https://hackaday.io/project/158208-sawppy-the-rover)
* [Build blog](https://newscrewdriver.com/category/projects/sawppy-the-rover/)
* [Live Onshape CAD file "Sawppy the Rover"](https://cad.onshape.com/documents/43678ef564a43281c83e1aef/w/392bbf8745395bc24367a35c/e/9bd6bbb7aba50a97523d14f2)
