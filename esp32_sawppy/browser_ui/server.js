// This NodeJS web server exists to help with developing Sawppy web UI
// elements. This way the HTML/JS/etc. can be developed without having
// to re-upload to the ESP32 upon every change. The goal of this server
// is to be indistinguishable from ESP32 as far as the browser can see.

// Using serve-static as static file server.
// https://expressjs.com/en/resources/middleware/serve-static.html
const http = require('http');
const serveStatic = require('serve-static');
const staticServer = serveStatic('./static', {maxAge: 1});
const finalHandler = require('finalhandler');

var webServer = http.createServer(function (request, response) {
    staticServer(request, response, finalHandler(request, response));
}).listen(8080);


// WebSocket server via 'ws' package

const WebSocket = require('ws');

var webSockTimeout;
var webSockPeriodicSend;
var webSockInstance;

const wss = new WebSocket.Server({
    noServer: true,
    clientTracking: true,
    path: '/joy_msg'
});

wss.on('connection', onConnection);
webServer.on('upgrade', onUpgrade);

function onUpgrade(request, socket, head) {
    if (0 === wss.clients.size) {
        wss.handleUpgrade(request, socket, head, function (ws) {
            wss.emit('connection', ws, request);
          });
    } else {
        console.log("Already have a joy_msg sender, rejecting additional connection.");
        socket.destroy();
        return;
    }
}

function onTimeoutClose() {
    console.log("Should close websock now.");
    webSockInstance.terminate();
}

function sendVoltageStub() {
    var voltageStub = JSON.stringify({voltage:3.141569});
    webSockInstance.send(voltageStub);
}

function onConnection(websocket, request) {
    websocket.on('message', onMessage );
    websocket.on('close', onClose);
    webSockInstance = websocket;
    webSockTimeout = setTimeout(onTimeoutClose, 1250);
    webSockPeriodicSend = setInterval(sendVoltageStub, 100);
}

function onMessage(message) {
    try
    {
        var parsed = JSON.parse(message);
        if (parsed.axes === undefined ||
            parsed.axes.length === undefined ||
            parsed.axes.length < 2)
        {
            console.log('Bad joystick message.');
        }
        else
        {
            console.log('Steer ' + parsed.axes[0] + ' speed ' + parsed.axes[1]);
            webSockTimeout.refresh();
        }
    }
    catch (SyntaxError)
    {
        console.log("Failed to parse " + message + " as JSON");
    }
};

function onClose(code, reason) {
    console.log('Websocket closed ' + code + reason);
    clearTimeout(webSockTimeout);
    clearInterval(webSockPeriodicSend);
};
