var steerControl;
var speedControl;
var steerSpeedInterval;
const updatePeriodMilliseconds = 500;

var exampleSocket;

function eventSetup() {
  startSteerSpeed();
  startWebSocket();
}

function startSteerSpeed() {
  steerControl = document.getElementById('rangeSteer');
  speedControl = document.getElementById('rangeSpeed');
}

function updateSteerSpeed() {
  var joy_msg_axes = [0,0];

  joy_msg_axes[0] = steerControl.value;
  joy_msg_axes[1] = speedControl.value;

  message= { joy_msg:  joy_msg_axes };

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
  console.log("WebSocket Open!");
  exampleSocket.send("joy_msg_send");
}

function onSocketError(event) {
  console.log("WebSocket Error!");
  console.log(event);
}

function onSocketClose(event) {
  console.log("WebSocket Closed.");
  stopUpdates();
}

function onSocketMessage(event) {
  console.log("WebSocket Message Received.");
  console.log(event.data);
  if (event.data === "joy_msg_receive")
  {
    stopUpdates(); // Stop earlier timers
    steerSpeedInterval = setInterval(updateSteerSpeed, updatePeriodMilliseconds);
  }
}

document.addEventListener('DOMContentLoaded', eventSetup, false);
