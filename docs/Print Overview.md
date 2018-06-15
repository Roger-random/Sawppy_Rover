# 3D Printing Overview

Some general information about 3D printing Sawppy the Rover's parts

All of the 3D-printed parts were designed to be tolerant of dimensional errors. A less fancy way
to rephrase is: you don't need an expensive high precision 3D printer to print this rover.
In order to verify this claim, I purchased one of the least expensive 3D printers available
and printed an entire iteration of the rover on it.

## Verified to be Printable On Monoprice Maker Select

I bought a [Monoprice Maker Select](https://www.monoprice.com/product?p_id=13860), which is a rebranded
[Wanhao Duplicator i3](http://www.wanhao3dprinter.com/Unboxin/ShowArticle.asp?ArticleID=70), which is
itself a knockoff of the [Prusa i3](https://www.prusaprinters.org/prusa-i3/).
And not even a new one - I bought a returned "[Open Box](https://www.monoprice.com/product?p_id=21864)"
unit which is occasionally on sale for $199 or less. The downside, of course, is the printer will show
signs of (ab)use by its previous owner. The condition will be the luck of the draw.

This printer has a print bed 200mm wide by 200mm deep and roughly 180mm tall which can
comfortably print any single rover part. (It's actually large enough to print several parts at once.)
Most rover parts can in theory fit within the 120mm x 120mm area of the 
[Monoprice Maker Select Mini](https://www.monoprice.com/product?p_id=15365)
(or [open box](https://www.monoprice.com/product?p_id=30386)) but a few large parts (like the differential
brace) will not fit and some parts (like the wheel) will be a very tight fit.

All that said, if you want to print this rover on a Monoprice Maker Select/Wanhao Duplicator i3,
know that its low price does come with tradeoffs.

### Known Problems Printing on Monoprice Maker Select/Wanhao Duplicator i3

* The printer chassis is not very rigid. In practice, this translates to minor surface distortions.
Surfaces that are supposed to be flat become a little wavy, things of that nature. Fortunately these
cosmetic flaws are irrelevant for rover operation.
* It is not a heavy duty piece of equipment and may break down under continuous use. Either plan on
letting the machine rest between print jobs, or plan on making printer upgrades. It is a popular
enough printer that an internet search will help you do whatever you need to do to with this printer.
I chose the upgrade path when the default control board started emitting the smell of unhappy
electronics after a few weeks of around-the-clock printing. The stock control board was replaced with
an [Azteeg X5 mini](http://www.panucatt.com/azteeg_X5_mini_reprap_3d_printer_controller_p/ax5mini.htm).
* The first draft of the rover was printed in PLA, the second in PETG. While the stock print head for
the Monoprice Maker Select is technically capable of PETG, it could only reliable extrude at a very
slow rate. In order to speed up printing PETG the stock print hardware must be upgraded to an all-metal
hot end, in my case an [E3D Titan Aero](https://e3d-online.com/titan-aero-builder).

## STL Slicer Settings

The rover prototypes were printed with the following parameters:

* Nozzle Diameter: 0.4mm (default nozzle of Monoprice Maker Select)
* Layer Height: 0.3mm
* Number of perimeters: 3. (Multiplied by 0.4mm nozzle diameter equals 1.2 mm vertical walls.)
* Number of top/bottom layers: 4. (Multiplied by 0.3mm layer height equals 1.2mm horizontal walls.)
* Infill type: Triangular
* Infill percentage: 30%

The first draft was printed in PLA at high speed for fast iteration, at the sacrifice of some
precision. The second was in PETG and printed slower because of the nature of PETG and it also
gave us more precise dimensions.

All parts were designed to be printed without supports.

## Print post-processing

In addition to the typical cleanup work required of 3D printed parts, some printed rover parts require
post-processing of the following types:

* **Bridge Removal**: All bolt holes facing the print bed will have a thin layer of extra plastic to
help the 3D printer bridge across what would otherwise be empty space for the bolt hole. After printing
is complete, this thin layer can be removed with a drill bit diameter around 4mm or 5/32". Smaller is
OK, precision is not as important in bridge removal.
* **Reaming**: Several pieces need to precisely fit around an 8mm steel shaft. A reamer is a precise cutting tool
to ensure the printed 8mm hole will fit an 8mm shaft. An 8mm drill bit is not as good. Precision matters
here, and a 5/16" drill bit (which is technically closer to 7.93mm) may not be good enough.
* **Tapping**: A few parts are not mechanically demanding, so we can tap M3 thread directly
into the plastic.
* **Heat-set thread insert**: The rest of the parts that use M3 fasteners will need a brass heat-set
insert installed in the plastic for a durable M3 thread.
* **608 Bearings**: Sawppy the Rover uses [commodity type 608 bearings](https://newscrewdriver.com/2018/03/07/for-cheap-commodity-bearings-search-for-608/)
held in printed plastic parts. If the 22mm hole is a little too small, the bearings will need to be
installed with the help of a tool. Ideally an [arbor press](https://en.wikipedia.org/wiki/Arbor_press), but a C-clamp will work.
