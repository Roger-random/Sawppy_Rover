# Sawppy HTML GUI Desktop Development Tool

Sawppy Rover ESP32 brain has the option to serve a HTML-based control
interface to rover operators. To develop the HTML (and associated) files it is
not always necessary to involve the ESP32, sometimes we just need a web server
running on the development workstation.

The files in this directory will set up a Docker container with a Node.js web
server running `server.js` which will act as ESP32 placeholder stub.

Control interface files live in the `static` subdirectory. They are served by
both `server.js` and the ESP32. The ESP32 PlatformIO project has been
configured to pull these files into a SPIFFS file image for uploading to flash
memory of a Sawppy Rover ESP32 brain.
