// This NodeJS web server exists to help with developing Sawppy web UI
// elements. This way the HTML/JS/etc. can be developed without having
// to re-upload to the ESP32 upon every change. The goal of this server
// is to be indistinguishable from ESP32 as far as the browser can see.

// References for static file serving:
// https://nodejs.org/en/knowledge/HTTP/servers/how-to-serve-static-files/
// https://github.com/cloudhead/node-static

var nodestatic = require('node-static');

// cache set to 1 second during development so our changes are not covered
// up by client side caching
var fileServer = new nodestatic.Server('./static', {cache: 1});

var webServer = require('http').createServer(function (request, response) {
    request.addListener('end', function () {
        fileServer.serve(request, response, function (e, res) {
            if (e && (e.status === 404)) {
                // Any other request is treated as valid request for index.html.
                fileServer.serveFile('/index.html', 200, {}, request, response);
            }
        });
    }).resume();
}).listen(8080);


// WebSocket server via 'ws' package

const WebSocket = require('ws');

const wss = new WebSocket.Server({ server: webServer, path: '/joy_msg' });

wss.on('connection', onConnection);

function onConnection(websocket, request) {
    websocket.on('message', onMessage );
    websocket.on('close', onClose);
    websocket.send('joy_msg_receive');
}

function onMessage(message) {
    //console.log('received: %s', message);
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
        }
    }
    catch (SyntaxError)
    {
        console.log("Failed to parse " + message + " as JSON");
    }
};

function onClose(code, reason) {
    console.log('Websocket closed ' + code + reason);
};
