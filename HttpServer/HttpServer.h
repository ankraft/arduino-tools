/*
 *	HttpServer.h
 *
 *	copyright (c) Andreas Kraft 2018
 *	Licensed under the BSD 3-Clause License. See the LICENSE file for further details.
 *
 *	Implementation of a simple HTTP Server.
 */

# ifndef __HTTPSERVER__
# define __HTTPSERVER__

#ifdef ESP32
#include <WiFi.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
# include "LinkedList.h"


class HttpServer {
public:

	// Enum type for HTTP commands
	enum Method {
		NONE, GET, POST, PUT, HEAD, DELETE, OPTIONS, CONNECT, ALL
	};

	// Struct that holds a request handler's result.
	struct RequestResult {
		int returnCode;
		String attributes;
		String type;
		String content;
	};

	// typedef for request handlers
	typedef RequestResult (*RequestHandler)(String path, Method method, long length, String type, char *content);


private:

	// internal struct for keeping handlers
	struct Handler {
		String 				path;
		Method 				method;
		RequestHandler		handler;
	};

	WiFiServer	 			*server;
	RequestHandler			 defaultRequestHandler;
	LinkedList<Handler *>	 handlers;

	void 			initServer(int port);						// init the WifiServer
	Method 			getMethod(String v);						// get the HTTP method from a string
	Handler*		findHandler(String path, Method method); 	// find the handler for a request
	String 			getResultMessage(int code);					// get the message for a http result code


public:

	//	Constructor to initialize the HTTP Server.
	//	The server binds to and listens on port 80 (http default).
	HttpServer();

	//	Constructor to initialize the HTTP Server.
	//	*port* is the port for the server to bind and listen.
	HttpServer(int port);

	//	Constructor to initialize the HTTP Server.
	//	*port* is the port for the server to bind and listen.
	//	*defaultRequestHandler* is a function that is called in
	//	case no other matching handler was defined for that request.
	HttpServer(int port, RequestHandler defaultRequestHandler);

	//	Destructor
	~HttpServer();

	//	Check for an incoming HTTP request. This method must be called
	//	very regularly in order to receive and process requests.
	//	It returns either immediately (when there is no pending request to 
	//	process), or after a request has been processed (by calling the 
	//	appropriate handler function) and sending the answer back to the client.
	void check();

	//	Add a new request handler function.
	//	*path* is the matching request path.
	//	*method* is the matching request method (see enum *Method* below).
	//	If the special method *ALL* is given here, then the handler is called
	//	for all method types.
	//	*handler* is the actual handler function to call for the incoming
	//	request (see type *RequestHandler*).
	//	Note, that this method is called with the same *path* and *method* but
	//	another *handler* function, then a previous handler function is 
	//	replaced.
	void addHandler(String path, Method method, RequestHandler handler);

	//	Remove a previously defined handler function.
	//	*path* is the matching request path.
	//	*method* is the matching request method (see enum *Method* below).
	void removeHandler(String path, Method method);

	//	Check whether a handler function has been defined for a path and method.
	//	*path* is a matching request path.
	//	*method* is a matching request method (see enum *Method* below).
	bool hasHandler(String path, Method method);

};

# endif
