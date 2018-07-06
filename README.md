# Sawppy the Rover

Sawppy is a motorized model of Mars rovers Curiosity and Mars 2020. It faithfully reproduces
the [Rocker-Bogie suspension kinematics](https://en.wikipedia.org/wiki/Rocker-bogie) of real
rovers and is intended to be a hardware platform for future software projects in autonomous
operation. Go forth and boldly explore the back yard, Sawppy!

All information for building Sawppy is free and public open source for anyone to build their
own. Build instructions can be found
[under the docs subdirectory](docs/).

Sawppy Rover was inspired by SGVHAK Rover, a six-wheeled robot inspired by the NASA JPL rovers
sent to explore Mars. The major differences between Sawppy and SGVHAK Rover were motivated by
a desire to reduce cost and complexity. To accomplish this goal, Sawppy differs in two major
design objectives:

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

**Sawppy is at version 1.0!** 

This milestone includes a motorized rolling chassis that is mechanically functional.
The remaining areas (electrical, software, etc.) are still very immature, just barely enough
to validate mechanical chassis function. Click this image for a YouTube video illustrating
the chassis in action, climbing a backpack that is almost double the height of wheel diameter.

[![Sawppy conquers a backpack](https://img.youtube.com/vi/acANiRFg-qA/0.jpg)](https://www.youtube.com/watch?v=acANiRFg-qA)

# Other Project Resources

* [Project page on Hackaday.io](https://hackaday.io/project/158208-sawppy-the-rover)
* [Build blog](https://newscrewdriver.com/category/projects/sawppy-the-rover/)
* [Live Onshape CAD file "Sawppy the Rover"](https://cad.onshape.com/documents/43678ef564a43281c83e1aef/w/392bbf8745395bc24367a35c/e/9bd6bbb7aba50a97523d14f2)
