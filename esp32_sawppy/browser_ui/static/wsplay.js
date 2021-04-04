var exampleSocket;

function goWebSocket() {
  console.log("goWebSocket called");

  exampleSocket = new WebSocket("ws://192.168.1.244:8080/");

  exampleSocket.onopen = onSocketOpen;
  exampleSocket.onerror = onSocketError;
  exampleSocket.onclose = onSocketClose;
  exampleSocket.onmessage = onSocketMessage;
}

function onSocketOpen(event) {
  console.log("WebSocket Open!");
  exampleSocket.send("Hello WebSocket World!");
}

function onSocketError(event) {
  console.log("WebSocket Error!");
  console.log(event);
}

function onSocketClose(event) {
  console.log("WebSocket Closed.");
}

function onSocketMessage(event) {
  console.log("WebSocket Message Received.");
  console.log(event.data);
  exampleSocket.close();
}

document.addEventListener('DOMContentLoaded', goWebSocket, false);
