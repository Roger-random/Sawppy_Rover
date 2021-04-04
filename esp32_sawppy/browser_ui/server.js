// This NodeJS web server exists to help with developing Sawppy web UI
// elements. This way the HTML/JS/etc. can be developed without having
// to re-upload to the ESP32 upon every change. The goal of this server
// is to be indistinguishable from ESP32 as far as the browser can see.

// References for static file serving:
// https://nodejs.org/en/knowledge/HTTP/servers/how-to-serve-static-files/
// https://github.com/cloudhead/node-static

var static = require('node-static');

var fileServer = new static.Server('./static', {cache: 1});

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

wss.on('connection', function connection(ws) {
  ws.on('message', function incoming(message) {
    console.log('received: %s', message);
  });

  ws.send('something');
});
