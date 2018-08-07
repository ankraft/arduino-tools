# HttpServer

A simple HTTP Server Framework.

## Installation

- Copy the files from this directory to your project.
- Also copy the .h and .ino files from the [LinkedList](../LinkedList/README.md) sub-project to your project. 

## Usage

The following examples show how to use the HTTP Server framework.

**Note**, that an HTTP Server can only be instantiated **after** a successful network connection has been established, e.g. the connection to a WiFi network.

### Instantiate an HTTP Server

The following code shows how to instantiate a new HTTP Server on port 80.

```cpp
# include "HttpServer.h"

HttpServer *server;

void setup() {
  ...
  // establish a network connection, e.g. WiFi, BEFORE instantiating a HTTP Server
   aNetworkInitFunction();	
   ...
   server = new HttpServer();	// new server on port 80 
   ...
}
```

### Define and Register a Request Handler

This code snippet shows how to define a request handler

```cpp
HttpServer::RequestResult aRequestHandler(String path, HttpServer::Method method, long length, String type, char *content) {
  HttpServer::RequestResult result;

  // ... do something meaningful here...

  result.returnCode = 200;	// OK
  result.attributes = "X-attribute1: value\nX-attribute2: value"; // additional attributes
  result.type = "text/plain";
  result.content ="Hello, world!";
  return result;
}
```

A request handler must always return a *RequestResult* struct with a *returnCode* set to a valid HTTP status code. 200, for example, means OK. See [https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html](https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html) for more details.

In addition to the mandatory *returnCode* a request handler can add content, the content type, and additional attributes to add to the answer. Multiple attributes can be provided, but they must be separated by a new-line (\n) character.

A request handler for a GET request and path */foo/bar* that uses this function is registered as follows, e.g. in the setup() function after instantiating the HTTP Server:

```cpp
...
server->addHandler("/foo/bar", HttpServer::GET, aRequestHandler);
...
```

### Fallback Handler

In the case when there is no matching request handler can be found there are two possibilities.

The first one is that the HTTP Server has a default request handler set during instantiation. This handler is called for all requests that could not be matched to a registered handler function. This default request handler must have the same signature as any other request handler, and it also returns a valid *RequestResult* structure.

If there is no default request handler defined than the HTTP Server returns an answer with a status code of 501 *Not Implemented*.


### Request Argument Handling

A request handler may receive arguments, for example in a GET request itself or in the body of a POST request. A couple of static class methods can be utilized in order to parse and handle these parameters.

The following example shows how to handle the request ``GET /aHander?argument1=value&argument2=another%20value``.

```cpp
HttpServer::RequestResult aHandler(String path, HttpServer::Method method, long length, String type, char *content) {
   ...

  // Parse request arguments
  int numberOfArguments = HttpServer::parseRequestArguments(path);
  if (numberOfArguments == 2) {
    Serial.println(HttpServer::getRequestArgument("argument1"));
    Serial.println(HttpServer::getRequestArgument("argument2"));  	
  }

  ...
 }
```

## Class Definitions

The *HttpServer* class defines the following constructors, types and structures.


### Constructors

- **HttpServer()**  
Constructor to initialize the HTTP Server.  
The server binds to and listens on port 80 (http default).
- **HttpServer(int port)**  
Constructor to initialize the HTTP Server.  
*port* is the port for the server to bind and listen.
- **HttpServer(int port, RequestHandler defaultRequestHandler)**  
Constructor to initialize the HTTP Server.  
*port* is the port for the server to bind and listen.  
*defaultRequestHandler* is a function that is called in case no other matching handler was defined for that request.


### Server Methods

- **void check()**  
Check for an incoming HTTP request. This method must be called very regularly in order to receive and process requests.  
It returns either immediately (when there is no pending request to process), or after a request has been processed (by calling the appropriate handler function) and sending the answer back to the client.

### Request Handling Methods

- **void addHandler(String path, Method method, RequestHandler handler)**  
Add a new request handler function.  
*path* is the matching request path.  
*method* is the matching request method (see enum *Method* below). If the special method *ALL* is given here, then the handler is called for all method types.  
*handler* is the actual handler function to call for the incoming request (see type *RequestHandler*).  
Note, that this method is called with the same *path* and *method* but another *handler* function, then a previous handler function is replaced. A handler function can be registered multiple times for different paths and/or method types, though.
- **void removeHandler(String path, Method method)**  
Remove a previously defined handler function.  
*path* is the matching request path.  
*method* is the matching request method (see enum *Method* below).
- **bool hasHandler(String path, Method method)**  
Check whether a handler function has been defined for a path and method.  
*path* is a matching request path.  
*method* is a matching request method (see enum *Method* below).

### Request Argument Handling

- **static int parseRequestArguments(String path)**  
Parse a request path for arguments. Found arguments are stored them for later retrieval and processing. Only one set of arguments can be stored at a time for all instances of the HTTPServer class. The names and arguments are URL decoded in the process.  
*path* the request path to parse. 
- **static String getRequestArgument(String key)**  
Check for and retrieve a formerly stored request argument. If the argument couldn't be found then an empty string is returned.  
*key* the name of the argument.
- **static String urlDecode(String value)**  
Decode a URL encoded string. The decoded string is returned.  
*value* the String to decode.


### Types and Definitions

- **typedef RequestResult (*RequestHandler)(String path, Method method, long length, String type, char *content)**  
This typedef defines the request handler's function signature.  
A request handler must always return a *RequestResult* structure.  
A request handler is called with the following parameters:
	- *path*: The request's path.
	- *method*: The request's method.
	- *length*: The number of characters in the *content* parameter.
	- *type*: The request's content-type property.
	- *content*: The actual request content.
- **struct RequestResult**  
This structure defines a container for a *RequestHandler*'s answer. It has the following fields:
	- *int returnCode*: A valid numeric HTTP status code.
	- *String attributes*: Additional optional attributes for the answer.
	- *String type*: The MIME content-type of the answer.
	- *String content*: The actual textual content of the result.
- **enum Method**  
This enum defines enum types for HTTP commands used for request handler functions: *NONE*, *GET*, *POST*, *PUT*, *HEAD*, *DELETE*, *OPTIONS*, *CONNECT*, *ALL*.  
The special command *ALL* can be used as a wild card to match any HTTP command.

## Limitations

- Path matching for request handlers does not yet support wild cards. A path must be a precise match.
- Only one request at a time can be processed. This also includes request argument parsing.
- Only textual content can be returned in a request's answer, no binary data.


## Security

Please be aware, that this simple framework does not implement any security measures.


## License
Licensed under the BSD 3-Clause License. See the [LICENSE](../LICENSE) file for further details.
