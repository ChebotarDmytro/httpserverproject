# Your Documentation

1. Requirements

   - Ubuntu 20.04
   - Qt 6.2.4

2. Build

   - cd ~/your_project_folder/
   - mkdir build && cd build
   - qmake .. && make
   - ./your_app

3. Test cases

   ```bash
   $ curl -i http://localhost:8000/foo
   HTTP/1.1 404 Not Found
   
   $ curl -i -H "Content-Type: application/json" -X POST -d '{ "secret": 42 }' http://localhost:8000/foo
   HTTP/1.1 200 OK
   Content-Length: 0
   
   $ curl -i http://localhost:8000/foo
   HTTP/1.1 200 OK
   Content-Type: application/json
   Content-Length: 16
   
   { "secret": 42 }
   
   $ curl -i -H "Content-Type: text/plain" -X POST -d 'squeamish ossifrage' http://localhost:8000/yo/yo/yo
   HTTP/1.1 200 OK
   Content-Length: 0
   
   $ curl -i http://localhost:8000/yo/yo/yo
   HTTP/1.1 200 OK
   Content-Type: text/plain
   Content-Length: 19
   
   squeamish ossifrage
   
   # Deleting a URI removes the mapping
   $ curl -i -X DELETE http://localhost:8000/yo/yo/yo
   HTTP/1.1 200 OK
   Content-Length: 0
   
   $ curl -i http://localhost:8000/yo/yo/yo
   HTTP/1.1 404 Not Found
   ```

