# Sawppy the Rover

Sawppy is a motorized model of Mars rovers Curiosity and Perseverance. It faithfully reproduces
the [Rocker-Bogie suspension kinematics](https://en.wikipedia.org/wiki/Rocker-bogie) of real
rovers and is intended to be a hardware platform for future software projects in autonomous
operation. Go forth and boldly explore the back yard, Sawppy!

This repo exists to hold this [Ag readme](https://github.com/samuk/Sawppy_Rover/blob/main/modifications/Ag/readme.md) & to host [Discussions](https://github.com/samuk/Sawppy_Rover/discussions)

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

**Status Update, Perseverance Landing Edition**

Generating web traffic was never an objective of my Sawppy project so I hadn't kept an eye on the
statistics, but I looked at it just now and noticed a distinct rise over the past two weeks. I
guess the successful landing of Perseverance rover on Mars motivated a lot of people to think about
3D-printing their own rover here on Earth. Welcome, fellow rover fans!

Here's a quick summary past and future for fans new and old alike:

What you see on this project page is what I now refer to as "Sawppy V1". It was my first project
that I documented with the explicit goal of helping others build their own. In contrast to most
of my other projects which were just to show an example and/or notes to remind myself later. I was
happy to see that many people have accepted my invitation and built their own rovers.
(See "Sawppy Builders" log entries on [Sawppy's Hackaday.io page](https://hackaday.io/project/158208).)
I appreciate constructive feedback submitted by Sawppy rover builders from around the world,
and have been pondering how I can evolve future Sawppy versions to address feedback.

One persistent line of feedback was that Sawppy was still too complex and expensive for some rover
fans, even though Sawppy was designed to be more affordable and simpler than the JPL Open Source
Rover that inspired it. It is true I built Sawppy for myself and others with roughly my level of
skill and budget. This translated to an audience of mostly adult hobbyists and some students of
college and high school age who can drop $500 USD on a project.

To address the audience left out of the fun by Sawppy V1, my current step in Sawppy evolution is
"Micro Sawppy". I am designing and developing a smaller, simpler, and more affordable rover. One
that I want to be accessible to elementary school age students (with adult supervision) with a
target parts cost of $100 USD. I also want to write the instructions to be more exact and prescriptive.
Sawppy V1 instructions had several areas that were left open to builder preference, something I've
learned is confusing to beginners who lacked the experience to have a preference.

Micro Sawppy also serves as my testbed for several ideas for 3D-printed rover construction that,
if successful, I want to adapt and scale up to a "Sawppy V2". This future rover would be roughly
the same size as Sawppy V1. But I want to make it easier to build, more adaptable to variations,
and a lower base cost target of $400 USD.

For latest updates and/or more details, I post my development progress to the
[Sawppy category on my personal blog](https://newscrewdriver.com/category/projects/sawppy-the-rover/). 

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

* **Simplest**: Wired control [running on an Arduino](https://github.com/Roger-random/Sawppy_Rover/tree/master/arduino_sawppy) instead of Raspberry Pi. Created as a [backup control option](https://newscrewdriver.com/2019/05/20/sawppy-roving-with-wired-handheld-controller/) in noisy RF environments where WiFi is unreliable. It also happens to feature the bare-bones version of Sawppy geometry calculations. Wired control meant skipping all the overhead of wireless communication. Cutting out all HTML code also meant this is a good basis for other control mechanisms: send desired speed and direction into Arduino and let it handle Sawppy chassis geometry calculation.
* **Original**: HTML-based wireless teleoperation software stack [modified from SGVHAK rover](/docs/SGVHAK%20Rover%20Software.md). This was written to be easy for others to understand and modify.
* **R/C control**: [lightly modified from my SGVHAK rover software](https://github.com/mw46d/SGVHAK_Rover) by Marco Walther (mw46d) for a Raspberry Pi-based way to interface with traditional remote control receivers.

Plus three options for turning Sawppy into a [ROS](http://ros.org) robotics platform.

* **ROS Kinetic**: [heavily modified from my SGVHAK rover software](https://github.com/mw46d/Sawppy_ROS) by Marco Walther (mw46d) which translates ROS `/cmd_vel` commands into Sawppy movement. This is a good stepping stone beyond original Sawppy software.
* **ROS Melodic**: [a ground-up rewrite of a ROS-centric stack](https://github.com/srmainwaring/curio) by Rhys Mainwaring (srmainwaring) is extensive and powerful. Going beyond responding to `/cmd_vel` commands, it also calculates `/odom` by interpolating LX-16A position encoder ~270 degree feedback into full 360 degrees. Plus visualizing rover state in RViz, and files to put a digital Sawppy in Gazebo robot simulation environment.
*  **ROS2 Foxy**: [A ROS2 version](https://github.com/mgonzs13/ros2_rover) by Miguel Ángel González Santamarta (mgonzs13) This is a ROS 2 (Foxy) version of the Probable Adventure Project. A C++ version and a Python version of the lx16a controller are included. Besides, a PS3 joy controller and a hokuyo laser can be used.

# Modifications From Rover Builders

Many people who built their own Sawppy rover are also people who love to tinker
and experiment. As a result these rovers reflect the individuality of their
builder. Some of these builders have shared their work with the community,
see the [`modifications` folder](/modifications) for more information.

# Other Project Resources

* [Project page on Hackaday.io](https://hackaday.io/project/158208-sawppy-the-rover)
* [Build blog](https://newscrewdriver.com/category/projects/sawppy-the-rover/)
* [Live Onshape CAD file "Sawppy the Rover"](https://cad.onshape.com/documents/43678ef564a43281c83e1aef/w/392bbf8745395bc24367a35c/e/9bd6bbb7aba50a97523d14f2)

# Financial Support

Sawppy is shared free of charge, but builders in the United States may choose to buy some parts via
[Amazon affiliate links](docs/AmazonAssociate.md)
which will send a small sales commission to Sawppy's creator.
