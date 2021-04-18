/*

Sawppy Rover touchscreen driving joystick JavaScript

Copyright (c) Roger Cheng

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

/*

  This script file has three major areas to implement an onscreen joystick for
  interactively driving Sawppy Rover. It was designed primarily with touchscreen
  in mind but it is also usable with a mouse on a desktop computer browser.

  1: Draw visual representation onscreen with <canvas>
    https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API

  2: Respond to user input with Pointer Events
    https://developer.mozilla.org/en-US/docs/Web/API/Pointer_events

  3: Communicate user intent to server using WebSockets
    https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API

  Ideally these components would have been organized as separate classes with
  matching properties and methods. However, since it was designed to run on
  old browsers on old phones(*), everything is global and flat due to
  unpredictable support of modern JavaScript features like classes.

  (*) Target platform is Internet Explorer built into Windows Phone 8.1

*/

//////////////////////////////////////////////////////////////////////////
//
//  HTML <canvas> drawing context and related values.

/** HTML DOM <canvas> element hosting the joystick */
var drawCanvas;

/** Drawing context of drawCanvas */
var ctx;

/** Drawing height as a proportion of width */
const portraitRatio = 1.1;

/** Width of drawing context */
var drawWidth;

/** Height of drawing context */
var drawHeight;

//////////////////////////////////////////////////////////////////////////
//
//  Joystick control pad

/** Fraction of drawing surface allocated to border around joystick */
const padBorderFraction = 0.05;

/** Size of labels used on the joystick, as a size fraction of drawing surface */
const padLabelFraction = 0.04;

/** Drawing offset for joystick control pad after accounting for border */
var padOffset;

/** Size of joystick control pad after accounting for border */
var padSize;

/** Color to use for drawing joystick control pad */
var padBackground = '#0000FF';

//////////////////////////////////////////////////////////////////////////
//
//  Joystick control knob

/** Radius of joystick control knob as fraction of drawing surface */
const knobRadiusFraction = 0.1;

/** Calculated radius of joystick control knob */
var knobRadius;

/** Minimum joystick control knob coordinate value */
var knobMin;

/** Maximum joystick control knob coordinate value  */
var knobMax;

/** Joystick control knob X coordinate, clamped within min & max */
var clampedX;

/** Joystick control knob Y coordinate, clamped within min & max */
var clampedY;

/** Joystick control knob X value mapped to range from -1.00 to 1.00 */
var controlX = 0;

/** Joystick control knob Y value mapped to range from -1.00 to 1.00 */
var controlY = 0;

/** Joystick control knob color when user finger is lifted */
var knobUp = '#FF0000';

/** Joystick control knob color when user finger is pressed */
var knobDown = '#00FF00';

/** When running at max speed, steering range is constrained to a smaller range
    so (1) steering angle control can be more precise, and (2) reduce chance
    of rover trying to do a tight turn at speed. This value is the amount of
    reduction. (0.75 means to leave 25% steering range at max speed.) */
const maxSpeedSteerFraction = 0.8;

/** Server connection status text
  @todo Move status text to plain HTML element, it doesn't have to be rendered on canvas. Will also need to change portraitRatio to 1.0 when this happens.
*/
var statusString = "startup";

/** Reference to pointer representing user's finger on the control knob.
  Is 'undefined' when user finger is lifted.
 */
var pointerCaptured = undefined;

/** Called upon DOMContentLoaded to start joystick control */
function eventSetup() {
  startWebSocket();
  startControls();
}

/** Initializes drawing and input events for joystick control pad */
function startControls() {
  drawCanvas = document.getElementById('controlPad');
  ctx = drawCanvas.getContext("2d");
  drawCanvas.onpointerdown = down_handler;
  drawCanvas.onpointerup = up_handler;
  drawCanvas.onpointermove = move_handler;
  drawCanvas.onlostpointercapture = lost_pointer;
  reSize();
}

/** Look for user press on control knob, signifying begin of joystick control */
function down_handler(pointerEvent) {
  if (pointerCaptured === undefined &&
      pointerEvent.isPrimary &&
      pointerEvent.offsetX > clampedX-knobRadius &&
      pointerEvent.offsetX < clampedX+knobRadius &&
      pointerEvent.offsetY > clampedY-knobRadius &&
      pointerEvent.offsetY < clampedY+knobRadius) {
    pointerCaptured = pointerEvent.pointerId;
    drawCanvas.setPointerCapture(pointerCaptured);
    update_knob(pointerEvent);
  }
}

/** See if user has released control knob */
function up_handler(pointerEvent) {
  if (pointerEvent.pointerId === pointerCaptured) {
    drawCanvas.releasePointerCapture(pointerCaptured);
    update_knob(pointerEvent);
    lost_pointer();
  }
}

/** Update state when user is no longer controlling the joystick */
function lost_pointer() {
  pointerCaptured = undefined;

  // When user releases, steering angle is untouched but speed drops to zero
  controlY = 0;
  clampedY = knobMin + (knobMax-knobMin)/2;

  // This is usually called at regular intervals, but for stop we send an immediate update
  updateSteerSpeed();
}

/** Knob only moves for captured control pointer, ignore other moves */
function move_handler(pointerEvent) {
  if (pointerCaptured == pointerEvent.pointerId) {
    update_knob(pointerEvent);
  }
}

/** Update steering angle and speed in response to new joystick location */
function update_knob(pointerEvent) {
  clampedY = Math.max(knobMin, Math.min(knobMax, pointerEvent.offsetY));
  controlY = 2*(clampedY - knobMin)/(knobMax - knobMin) - 1;
  controlY = Math.round(controlY*100)/100;

  var steerCoeff = (1-maxSpeedSteerFraction*Math.abs(controlY));

  clampedX = Math.max(knobMin, Math.min(knobMax, pointerEvent.offsetX));
  controlX = 2*(clampedX - knobMin)/(knobMax - knobMin) - 1;
  controlX = Math.round(controlX*steerCoeff*100)/100;

  window.requestAnimationFrame(reDraw);
}

/** Recalculate all canvas related values upon resize
 * @todo Take advantage of landscape layout. Until then it is just the same as portrait layout, squished to the left.
 */
function reSize() {
  // Calculate new drawing canvas size
  if (window.innerHeight > window.innerWidth * portraitRatio)
  {
    // Height is taller than required ratio. Take up full width and partial height.
    drawWidth = window.innerWidth;
    drawHeight = drawWidth * portraitRatio;
  }
  else
  {
    // Height is shorter than required ratio. Take up full height and partial width
    drawWidth = window.innerHeight / portraitRatio;
    drawHeight = window.innerHeight;
  }
  drawCanvas.width = drawWidth;
  drawCanvas.height = drawHeight;

  // New pad background size as fraction of canvas size
  padOffset = drawWidth*padBorderFraction;
  padSize = drawWidth*(1.0-padBorderFraction*2);

  // New knob size and min/max bounds as fraction of pad size
  knobRadius = padSize*knobRadiusFraction;
  knobMin = padOffset + knobRadius;
  knobMax = padOffset + padSize - knobRadius;

  // Calculate new knob location corresponding to control X/Y
  clampedX = knobMin + (controlX+1)*(knobMax-knobMin)/2;
  clampedY = knobMin + (controlY+1)*(knobMax-knobMin)/2;

  window.requestAnimationFrame(reDraw);
}

/** Redraw all elements of joystick control pad */
function reDraw() {
  ctx.clearRect(0,0,drawWidth, drawHeight);
  drawPad();
  drawKnob();
  drawStatusText();
}

/** Draw background of joystick control pad
 * @todo render just once upon reSize() and cache bitmap for fast blit, instead of drawing text repeatedly.
 */
function drawPad() {
  ctx.fillStyle = padBackground;
  ctx.fillRect(padOffset, padOffset, padSize, padSize);

  var labelSize = drawWidth*padLabelFraction;
  var padMid = padOffset+(padSize/2);
  ctx.fillStyle = '#FFFFFF';
  ctx.font =  labelSize + "px sans-serif";
  ctx.textAlign = "center";
  ctx.fillText('FORWARD', padMid, padOffset+labelSize);
  ctx.fillText('REVERSE', padMid, padOffset+padSize-labelSize);
  ctx.textAlign = "left";
  ctx.fillText('LEFT', padOffset, padMid);
  ctx.textAlign = "right";
  ctx.fillText('RIGHT', padOffset+padSize, padMid);
}

/** Draw joystick control knob */
function drawKnob() {
  ctx.save();
  ctx.beginPath();
  ctx.arc(clampedX, clampedY, padSize*knobRadiusFraction, 0, 2*Math.PI);
  if (pointerCaptured === undefined) {
    ctx.fillStyle = knobUp;
  }
  else {
    ctx.fillStyle = knobDown;
  }
  ctx.fill();
  ctx.restore();
}

/** Update status text string & queue redraw */
function updateStatus(newStatusString) {
  statusString = newStatusString;
  window.requestAnimationFrame(reDraw);
}

/** Draw connection status string
 * @todo move to HTML text element, this doesn't need to be on canvas.
 */
 function drawStatusText() {
  var textSize = drawWidth*0.05;
  ctx.fillStyle = "#000000";
  ctx.font = textSize + "px sans-serif";
  ctx.textAlign = "center";
  ctx.fillText(statusString, drawWidth/2, drawWidth+textSize);
}

//////////////////////////////////////////////////////////////////////////
//
//  Server communication via websock

/** Steering and speed updates are sent regularly once per this time period */
const updatePeriodMilliseconds = 100;

/** Timer object from calling setInterval() */
var serverUpdateTimer;

/** Websocket instance used for server communication */
var serverComm;

/** Called by serverUpdateTimer to send a speed+steer update to server */
function updateSteerSpeed() {
  if (WebSocket.OPEN === serverComm.readyState) {
    var joy_msg_axes = [0,0];

    joy_msg_axes[0] = -controlX; // Negative X (left) is positive angular velocity
    joy_msg_axes[1] = -controlY; // Negative Y (up) is positive linear velocity

    message= { axes:  joy_msg_axes };

    serverComm.send(JSON.stringify(message));
  }
}

/** Halt server updates */
function stopUpdates() {
  clearInterval(serverUpdateTimer);
}

/** Create server communication channel */
function startWebSocket() {
  serverComm = new WebSocket("ws://"+location.host+"/joy_msg");

  serverComm.onopen = onSocketOpen;
  serverComm.onerror = onSocketError;
  serverComm.onclose = onSocketClose;
  serverComm.onmessage = onSocketMessage;
}

/** When websocket is open, start sending regular updates to server */
function onSocketOpen(event) {
  updateStatus("Open");

  stopUpdates(); // Stop earlier timers
  serverUpdateTimer = setInterval(updateSteerSpeed, updatePeriodMilliseconds);
  updateStatus("Connected");
}

/** In case of error, dump information to browser console */
function onSocketError(event) {
  updateStatus("Error");
  console.log(event);
}

/** When socket is closed, clean up and visually indicate control is no longer possible */
function onSocketClose(event) {
  updateStatus("Disconnected");
  stopUpdates();
  // Release pointer capture
  try {
    drawCanvas.releasePointerCapture(pointerCaptured);
  } catch(err) {
    // DOMException expected if capture has already been lost.
  }
  pointerCaptured = undefined;
  controlX = 0;
  controlY = 0;
  padBackground = '#444444';
  knobUp = '#777777';
  knobDown = '#777777';
  reSize();
}

/** If server sends data, look for items we know to process, ignore the rest. */
function onSocketMessage(event) {
  try {
    message = JSON.parse(event.data);
    if (undefined !== message && undefined !== message.voltage) {
      var oneDecimal = Math.round(message.voltage * 10)/10;
      updateStatus("Battery: " + oneDecimal + "V");
    }
  } catch(err) {
    console.log("Invalid JSON received from server.");
  }
}

//////////////////////////////////////////////////////////////////////////
//
//  Page load setup

document.addEventListener('DOMContentLoaded', eventSetup, false);
document.defaultView.addEventListener('resize', reSize);
