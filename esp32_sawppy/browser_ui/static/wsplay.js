var steerControl;
var speedControl;
var steerSpeedInterval;
const updatePeriodMilliseconds = 500;

var exampleSocket;

var statusElement;
var statusString = "startup";

const portraitRatio = 1.1;
const knobRadius = 0.1;
var drawCanvas;
var ctx;
var drawWidth;
var drawHeight;

var boxOffset;
var boxSize;

var pointerCaptured = undefined;

function eventSetup() {
  //startSteerSpeed();
  //startWebSocket();
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
  if (pointerCaptured === undefined && pointerEvent.isPrimary) {
    updateStatus("Pointer Capture");
    pointerCaptured = pointerEvent.pointerId;
    drawCanvas.setPointerCapture(pointerCaptured);
  }
}

function up_handler(pointerEvent) {
  if (pointerEvent.pointerId === pointerCaptured) {
    drawCanvas.releasePointerCapture(pointerCaptured);
    pointerCaptured = undefined;
    updateStatus("Pointer Release");
  }
}

function move_handler(pointerEvent) {
  if (pointerCaptured == pointerEvent.pointerId) {
    updateStatus("Pointer " + pointerEvent.clientX + ", " + pointerEvent.clientY);
  }
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

  boxOffset = drawWidth*0.05;
  boxSize = drawWidth*0.9;

  window.requestAnimationFrame(reDraw);
}

function reDraw() {
  ctx.clearRect(0,0,drawWidth, drawHeight);
  drawBox();
  drawStatusText();
}

function drawBox() {
  ctx.fillStyle = "#0000FF";
  ctx.fillRect(boxOffset, boxOffset, boxSize, boxSize);
}

function updateStatus(newStatusString) {
  statusString = newStatusString;
  window.requestAnimationFrame(reDraw);
}

function drawStatusText() {
  var textSize = drawWidth*0.05;
  ctx.fillStyle = "#000000";
  ctx.font = textSize + "px sans-serif";
  ctx.fillText("Status: " + statusString, textSize, drawWidth+textSize);
}

function startSteerSpeed() {
  steerControl = document.getElementById('rangeSteer');
  speedControl = document.getElementById('rangeSpeed');
  statusElement = document.getElementById('status');
}

function updateSteerSpeed() {
  var joy_msg_axes = [0,0];

  joy_msg_axes[0] = steerControl.value;
  joy_msg_axes[1] = speedControl.value;

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
  updateStatus("WebSocket Open!");
  exampleSocket.send("joy_msg_send");
}

function onSocketError(event) {
  updateStatus("Server Communication Error.");
  console.log(event);
}

function onSocketClose(event) {
  updateStatus("Disconnected.");
  stopUpdates();
}

function onSocketMessage(event) {
  updateStatus("Received Server Command.");
  console.log(event.data);
  if (event.data === "joy_msg_receive")
  {
    stopUpdates(); // Stop earlier timers
    steerSpeedInterval = setInterval(updateSteerSpeed, updatePeriodMilliseconds);
    updateStatus("Connected.");
  }
}

document.addEventListener('DOMContentLoaded', eventSetup, false);
document.defaultView.addEventListener('resize', reSize);
