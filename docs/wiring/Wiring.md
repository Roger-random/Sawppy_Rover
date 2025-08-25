# Wiring

The wiring harness connects each of the motors to the BusLinker board on the 
body panel.  
Each end of each wire contains a connector to your servo motor:
* For the LX-16A, the Molex mini SPOX connector should be used.
    * [Mini-SPOX 5264 3 Position Housing](https://www.newark.com/molex/50-37-5033/connector-rcpt-3pos-1row-2-5mm/dp/57H1785)
    * [Mini-SPOX 5264 Socket](https://www.newark.com/molex/08-70-1040/contact-socket-22awg-crimp/dp/54H5573)
    * Each servo has two connectors, so you have to daisy chain them.
    * Here is an image of the connector:  
      ![Mini-SPOX 3 Position Connector](connector_images/molex-5264-3P-2.png)
* For the LX-224, you can use the JST-2.0-3P connector as shown below.  
    * Each servo has three connectors, enabling chaining in serial and/or parallel.

![Wiring Overview](overview.png)
Wiring diagram for one side of the rover, for the LX-224 configuration.  The other side is identical, except for the cable going to the BusLinker board.  For the LX-16A, you'd need to connect your servos in series or make a break-out board.

Some notes on termininology (with acknowledgement to 
[this NASA JPL Open Source Rover wiring documentation](https://github.com/nasa-jpl/open-source-rover/tree/master/electrical/wiring):
* *Wiring harness* (also called cable harness or wire assembly): the collective term for all the wiring and cables that are run through a system to connect power sources, sensors, data lines, etc. In the case of the rover, this means the cables that feed power to the motors, the control signals to the servos, and the encoder data back to the control boards.
* *Cable*: A group of wires/conductors, usually with an outer sheath.
* *Wire*: A single conductor, solid or stranded core, of any gauge.

Harness diagrams further below were generated with [WireViz](https://github.com/wireviz/WireViz/) and can be re-generated with the *generateHarness.bat/.sh* scripts.
![Harness Details](harness.png)

In summary, for the LX-224 configuraiton, you need
* 1x: 82 cm cable from BusLinker to right front wheel 
* 1x: 50 cm cable from BusLinker to left front wheel 
* 2x: 64 cm cable from front steering servo to rear steering servo 
* 2x: 36 cm cable from rear steering servo to middle drive servo 
* 4x: 21 cm cables from drive servo to steering servo 

