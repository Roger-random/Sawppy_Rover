const updatePeriodMilliseconds = 100;
var steerSpeedInterval;

var exampleSocket;

var statusElement;
var statusString = "startup";

const portraitRatio = 1.1;
var drawCanvas;
var ctx;
var drawWidth;
var drawHeight;

const boxBorderFraction = 0.05;
const boxLabelFraction = 0.04;
var boxOffset;
var boxSize;

const knobRadiusFraction = 0.1;
var knobRadius;
var knobMin;
var knobMax;
var clampedX;
var clampedY;
var msgX = 0;
var msgY = 0;

var pointerCaptured = undefined;

function eventSetup() {
  startWebSocket();
  startControls();
}

function startControls() {
  drawCanvas = document.getElementById('controlPad');
  ctx = drawCanvas.getContext("2d");
  drawCanvas.onpointerdown = down_handler;
  drawCanvas.onpointerup = up_handler;
  drawCanvas.onpointermove = move_handler;
  reSize();
}

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

function up_handler(pointerEvent) {
  if (pointerEvent.pointerId === pointerCaptured) {
    drawCanvas.releasePointerCapture(pointerCaptured);
    pointerCaptured = undefined;
    update_knob(pointerEvent);

    msgY = 0;
    clampedY = knobMin + (knobMax-knobMin)/2;
  }
}

function move_handler(pointerEvent) {
  if (pointerCaptured == pointerEvent.pointerId) {
    update_knob(pointerEvent);
  }
}

function update_knob(pointerEvent) {
  clampedX = Math.max(knobMin, Math.min(knobMax, pointerEvent.offsetX));
  msgX = 2*(clampedX - knobMin)/(knobMax - knobMin) - 1;
  msgX = Math.round(msgX*100)/100;

  clampedY = Math.max(knobMin, Math.min(knobMax, pointerEvent.offsetY));
  msgY = 2*(clampedY - knobMin)/(knobMax - knobMin) - 1;
  msgY = Math.round(msgY*100)/100;

  window.requestAnimationFrame(reDraw);
}

function reSize() {
  // TODO: Take advantage of landscape layout. Until then it is
  // just the same as portrait layout, squished to the left.
  if (window.innerHeight > window.innerWidth * portraitRatio)
  {
    drawWidth = window.innerWidth;
    drawHeight = drawWidth * portraitRatio;
  }
  else
  {
    drawWidth = window.innerHeight / portraitRatio;
    drawHeight = window.innerHeight;
  }

  drawCanvas.width = drawWidth;
  drawCanvas.height = drawHeight;

  boxOffset = drawWidth*boxBorderFraction;
  boxSize = drawWidth*(1.0-boxBorderFraction*2);

  knobRadius = boxSize*knobRadiusFraction;
  knobMin = boxOffset + knobRadius;
  knobMax = boxOffset + boxSize - knobRadius;

  clampedX = knobMin + (msgX+1)*(knobMax-knobMin)/2;
  clampedY = knobMin + (msgY+1)*(knobMax-knobMin)/2;

  window.requestAnimationFrame(reDraw);
}

function reDraw() {
  ctx.clearRect(0,0,drawWidth, drawHeight);
  drawBox();
  drawKnob();
  drawStatusText();
}

function drawBox() {
  ctx.fillStyle = '#0000FF';
  ctx.fillRect(boxOffset, boxOffset, boxSize, boxSize);

  var labelSize = drawWidth*boxLabelFraction;
  var boxMid = boxOffset+(boxSize/2);
  ctx.fillStyle = '#FFFFFF';
  ctx.font =  labelSize + "px sans-serif";
  ctx.textAlign = "center";
  ctx.fillText('FORWARD', boxMid, boxOffset+labelSize);
  ctx.fillText('REVERSE', boxMid, boxOffset+boxSize-labelSize);
  ctx.textAlign = "left";
  ctx.fillText('LEFT', boxOffset, boxMid);
  ctx.textAlign = "right";
  ctx.fillText('RIGHT', boxOffset+boxSize, boxMid);

}

function drawKnob() {
  ctx.save();
  ctx.beginPath();
  ctx.arc(clampedX, clampedY, boxSize*knobRadiusFraction, 0, 2*Math.PI);
  if (pointerCaptured === undefined) {
    ctx.fillStyle = '#FF0000';
  }
  else {
    ctx.fillStyle = '#00FF00';
  }
  ctx.fill();
  ctx.restore();
}

function updateStatus(newStatusString) {
  statusString = newStatusString;
  window.requestAnimationFrame(reDraw);
}

function drawStatusText() {
  var textSize = drawWidth*0.05;
  ctx.fillStyle = "#000000";
  ctx.font = textSize + "px sans-serif";
  ctx.textAlign = "center";
  ctx.fillText(statusString, drawWidth/2, drawWidth+textSize);
}

function updateSteerSpeed() {
  var joy_msg_axes = [0,0];

  joy_msg_axes[0] =  msgX;
  joy_msg_axes[1] = -msgY;

  message= { axes:  joy_msg_axes };

  exampleSocket.send(JSON.stringify(message));
}

function stopUpdates() {
  clearInterval(steerSpeedInterval);
}

function startWebSocket() {
  exampleSocket = new WebSocket("ws://192.168.1.244:8080/");

  exampleSocket.onopen = onSocketOpen;
  exampleSocket.onerror = onSocketError;
  exampleSocket.onclose = onSocketClose;
  exampleSocket.onmessage = onSocketMessage;
}

function onSocketOpen(event) {
  updateStatus("Open");
  exampleSocket.send("joy_msg_send");
}

function onSocketError(event) {
  updateStatus("Error");
  console.log(event);
}

function onSocketClose(event) {
  updateStatus("Disconnected");
  stopUpdates();
}

function onSocketMessage(event) {
  updateStatus("Received");
  console.log(event.data);
  if (event.data === "joy_msg_receive")
  {
    stopUpdates(); // Stop earlier timers
    steerSpeedInterval = setInterval(updateSteerSpeed, updatePeriodMilliseconds);
    updateStatus("Connected");
  }
}

document.addEventListener('DOMContentLoaded', eventSetup, false);
document.defaultView.addEventListener('resize', reSize);
