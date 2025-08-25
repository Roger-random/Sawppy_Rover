# Wiring

The wiring harness connects each of the motors to the BusLinker board on the 
body panel.  
Each end of each wire contains a JST-2.0-3P connector as shown below.  
Each servo has three connectors, enabling chaining in serial and/or parallel.

![Wiring Overview](overview.png)
Wiring diagram for one side of the rover.  The other side is identical, except for the cable going to the BusLinker board.

Some notes on termininology (with acknowledgement to 
[this NASA JPL Open Source Rover wiring documentation](https://github.com/nasa-jpl/open-source-rover/tree/master/electrical/wiring):
* *Wiring harness* (also called cable harness or wire assembly): the collective term for all the wiring and cables that are run through a system to connect power sources, sensors, data lines, etc. In the case of the rover, this means the cables that feed power to the motors, the control signals to the servos, and the encoder data back to the control boards.
* *Cable*: A group of wires/conductors, usually with an outer sheath.
* *Wire*: A single conductor, solid or stranded core, of any gauge.

Harness diagrams further below were generated with [WireViz](https://github.com/wireviz/WireViz/) and can be re-generated with the *generateHarness.bat/.sh* scripts.
![Harness Details](harness.png)

In summary, you need
* 1x: 82 cm cable from BusLinker to right front wheel 
* 1x: 50 cm cable from BusLinker to left front wheel 
* 2x: 64 cm cable from front steering servo to rear steering servo 
* 2x: 36 cm cable from rear steering servo to middle drive servo 
* 4x: 21 cm cables from drive servo to steering servo 

