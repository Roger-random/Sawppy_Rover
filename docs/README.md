# Instructions

This directory has information for building your own Sawppy the Rover

There are no dependencies between components, they can be fabricated in any order or distributed across team members.

Assembly steps do require components and possibly other assemblies to be completed.

## 3D Printed Components
[General overview](Print%20Overview.md) of 3D-printed rover parts.

Details:
* [Servo accessories](Print%20Servo%20Parts.md)
* [Steering knuckle](Print%20Steering%20Knuckle.md)
* [Fixed knuckle](Print%20Fixed%20Knuckle.md)
* [Wheel](Print%20Wheel.md)
* [Wheel hub](Print%20Wheel%20Hub.md)
* [Corner Steering Joints](Print%20Corner%20Steering%20Joints.md)
* [Suspension Bogie Joints](Print%20Suspension%20Bogie%20Joints.md)
* [Suspension Rocker Joints](Print%20Suspension%20Rocker%20Joints.md)
* [Main Body Box Corners](Print%20Body%20Box%20Corners.md)
* [Differential](Print%20Differential.md)

## Customized Metal Components
* [15mm square aluminum extrusion beams](Misumi%20HFS%203.md)
* [8mm diameter shafts](Shaft8mm.md)

## Mechanical Assembly
Each sub-assembly is listed with its pre-requisites.

* [Servo Actuator Module](AssembleActuatorModule.md)
  * Requires Component: [Servo accessories](Print%20Servo%20Parts.md)
* [Steerable Wheel](AssembleSteerableWheel.md)
  * Requires Component: [Steering knuckle](Print%20Steering%20Knuckle.md)
  * Requires Component: [Wheel hub](Print%20Wheel%20Hub.md)
  * Requires Assembly: [Servo Actuator Module](AssembleActuatorModule.md)
* [Fixed Wheel](AssembleFixedWheel.md)
  * Requires Component: [Fixed knuckle](Print%20Fixed%20Knuckle.md)
  * Requires Component: [Wheel hub](Print%20Wheel%20Hub.md)
  * Requires Assembly: [Servo Actuator Module](AssembleActuatorModule.md)
* [Suspension Bogie](AssembleBogie.md)
  * Requires Component: [Suspension Bogie Joints](Print%20Suspension%20Bogie%20Joints.md)
  * Requires Component: [Corner Steering Joints](Print%20Corner%20Steering%20Joints.md)
  * Requires Assembly: [Servo Actuator Module](AssembleActuatorModule.md)
  * Requires Assembly: [Fixed Wheel](AssembleFixedWheel.md)
  * Requires Assembly: [Steerable Wheel](AssembleSteerableWheel.md)
* [Suspension Rocker](AssembleRocker.md)
  * Requires Component: [Suspension Rocker Joints](Print%20Suspension%20Rocker%20Joints.md)
  * Requires Component: [Suspension Bogie Joints](Print%20Suspension%20Bogie%20Joints.md)
  * Requires Component: [Corner Steering Joints](Print%20Corner%20Steering%20Joints.md)
  * Requires Assembly: [Servo Actuator Module](AssembleActuatorModule.md)
  * Requires Assembly: [Steerable Wheel](AssembleSteerableWheel.md)
* [Main Body Box / Equipment Bay](AssembleBodyBox.md)
  * Requires Component: [Main Body Box Corners](Print%20Body%20Box%20Corners.md)
  * Requires Component: [Suspension Rocker Joints](Print%20Suspension%20Rocker%20Joints.md)
  * Requires Component: [15mm square aluminum extrusion beams](Misumi%20HFS%203.md)
* [Differential](AssembleDifferential.md)
  * Requires Component: [Differential](Print%20Differential.md)
  * Requires Assembly: [Main Body Box / Equipment Bay](AssembleBodyBox.md)
* [FinalAssembly](AssembleFinal.md)
  * Requires Assembly: [Differential](AssembleDifferential.md)
  * Requires Assembly: [Suspension Rocker](AssembleRocker.md)
  * Requires Component: [Wheel](Print%20Wheel.md)
  
![Mechanical chassis](images/MechanicalChassis.jpg)
