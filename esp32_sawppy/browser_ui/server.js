const http = require('http')

const port = 8080

const server = http.createServer((req, res) => {
  res.statusCode = 200
  res.setHeader('Content-Type', 'text/html')
  res.end('<h1>Hello, ESP32 Placeholder World!</h1>')
})

server.listen(port, () => {
  console.log(`Server running at port ${port}`)
})
