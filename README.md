# HTTP Server
This is a small HTTP server built in C.

It handles GET requests and serves basic file types. It's single-threaded and blocking and generally isn't to be practically used but just for learning.
Opens a TCP socket and listens on the specified port for connections, reads the request per connection and responds accordingly.

Supports basic static file types: HTML, CSS, JS, JSON, PNG, JPG, SVG, WEBP.
And responds with either:
- 200 OK
- 404 File Not Found
- 403 Forbidden (Only when path traversal is attempted)

My utilisation of Berkeley's sockets API was greatly assisted by Beej's Network Programming guide. 
I don't think I need to make it obvious this isn't production-grade and is simply me fiddling around. Feel free to mess with it though :)

## Usage
```bash
./server port root_directory
```
### Usage example:
```bash
$ ./server 5000 ~/testdir
[+] Listening on port: 5000
-------------------------------

[+] Connection accepted from ************ on port 5000
[+] HTTP Request recieved from ************: 
GET / HTTP/1.1
Host: ************:5000
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Linux; Android 10; K) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Mobile Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8
Sec-GPC: 1
Accept-Language: en-GB,en;q=0.8
Accept-Encoding: gzip, deflate

[+] HTTP Response sent to ************: 200 OK
-------------------------------

```
