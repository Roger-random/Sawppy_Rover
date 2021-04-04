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

const wss = new WebSocket.Server({ server: webServer });

wss.on('connection', onConnection);

function onConnection(websocket, request) {
    websocket.on('message', onMessage );
    websocket.send('Thank you for connecting!');
    console.log('Request Headers ');

    // Print header contents to console.
    Object.keys(request.headers).forEach(function(key) {
        console.log(key, request.headers[key]);
    })
}

function onMessage(message) {
    console.log('received: %s', message);
};
