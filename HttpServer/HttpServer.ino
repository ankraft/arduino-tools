/*
 *	HttpServer.ino
 *
 *	copyright (c) Andreas Kraft 2018
 *	Licensed under the BSD 3-Clause License. See the LICENSE file for further details.
 *
 *	Implementation of a simple HTTP Server.
 */

#include "HttpServer.h"

// TODO: support wildcards in paths


HttpServer::HttpServer() {
	this->defaultRequestHandler = NULL;
	initServer(80);
}

HttpServer::HttpServer(int port) {
	this->defaultRequestHandler = NULL;
	initServer(port);
}

HttpServer::HttpServer(int port, RequestHandler defaultRequestHandler) {
	this->defaultRequestHandler = defaultRequestHandler;
	initServer(port);
}


HttpServer::~HttpServer() {
	if (server) {
		server->stop();
		server = NULL;
	}
	while (handlers.size() > 0) {
		handlers.remove(0);
	}
}


void HttpServer::check() {
	if (! server) {
		return;
	}
	WiFiClient client = server->available();
	if (client) {	// has connection
		// Serial.println("New Client connected.");

		String 			currentLine = "";
		String 			responseHeader = "";
		bool			headerFinished = false;
		unsigned long 	contentLength = 0;
		unsigned long 	currentLength = 0;
		String 			contentType = "";
		Method 			method = NONE;
		String 			path;
		char 			*body = NULL;

		while(client.connected()) {
			if (client.available()) { 
        		char c = client.read();
        		// Serial.print(c);
				if (headerFinished) {
	       			body[currentLength++] = c;
        			if (currentLength == contentLength) {
    	   				break; // break while loop. Content retrieval finished,
        			}
        		} else {
        			if (c == '\n') {
        				if (currentLine.length() == 0) {	// header finished
	        				headerFinished = true;
	        				body = (char *)calloc((contentLength + 1), sizeof(char)); // allocate body array
	        				//Serial.println("header finished");
	        				if (contentLength == 0) {
	        					break;	// we can break here, no content.
	        				}
    	    			} else {
    	    				// look for request method. Should be the very first line
    	    				if (method == NONE) {
    	    					//Serial.println(currentLine);
    	    					int idx = currentLine.indexOf(' ');
    	    					if (idx > -1) {
    	    						method = getMethod(currentLine.substring(0,idx));
    	    					}

    	    					// Get the path from the same line
    	    					int idxPath = currentLine.indexOf(' ', idx+1);
    	    					if (idxPath > -1) {
    	    						path = currentLine.substring(idx+1, idxPath);
    	    					}
    	    				}

    	    				// look for the content length
    	    				if (currentLine.startsWith("Content-Length:")) {
    	    					contentLength = currentLine.substring(15).toInt();

    	    				// look for content type
    	    				} else if (currentLine.startsWith("Content-Type:")) {
    	    					contentType = currentLine.substring(13);
    	    					contentType.trim();
    	    				}
    	    				currentLine = "";
    	    			}
        			} else if (c != '\r') {  
          				currentLine += c;
        			}
        		}
        	}
		}

		// call the handler and return the result
		Handler *handler = findHandler(path, method); // do we have a request handler for that path and method?

		RequestHandler rh = handler != NULL ? handler->handler : defaultRequestHandler; // otherwise assign the provided one
		if (rh) {
			RequestResult result = (*rh)(path, method, contentLength, contentType, body);
			String answer = "HTTP/1.1 "  + String(result.returnCode) + " " + getResultMessage(result.returnCode);
			if (result.type.length() > 0) {
				answer += "\nContent-Type: " + result.type;
			}
			if (result.attributes.length() > 0) {
				answer += "\n" + result.attributes;
			}
			if (result.content.length() > 0) {
				answer += "\nContent-Length: ";
				answer += result.content.length();
				answer += "\r\n\r\n";
				answer += result.content;
			}
			Serial.println(answer);
			client.print(answer);
		} else {
			client.println("HTTP/1.1 501 Not Implemented");
		}

		// Close the client connection
		if (body) {
			free(body);
		}
		client.stop();
    	//Serial.println("Client Disconnected.");
	}
}


void HttpServer::addHandler(String path, Method method, RequestHandler handler) {
	// find an existing handler for that path. If yes, then replace the handler
	Handler *h = findHandler(path, method);
	if (h) {	
		h->handler = handler;
	} else {	// if not found then create a new one
		Handler *nh = new Handler();
		nh->path = path;
		nh->method = method;
		nh->handler = handler;
		handlers.add(nh);
	}
	//Serial.printf("Added HTTP handler for path: %s | method: %d\n", path.c_str(), method);
}


void HttpServer::removeHandler(String path, Method method) {
	// We can't use findHandler() here bc we need the index
	for (int i = 0; i < handlers.size(); i++) {
		Handler *h = handlers.get(i);
		if (h && h->path.compareTo(path) == 0 &&
			(h->method == method || h->method == Method::ALL)) {

			handlers.remove(i);
			delete h;
			return;
		}
	}
}


bool HttpServer::hasHandler(String path, Method method) {
	return 	findHandler(path, method) != NULL;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Internal methods
//


HttpServer::Handler* HttpServer::findHandler(String path, Method method) {
	for (int i = 0; i < handlers.size(); i++) {
		Handler *h = handlers.get(i);
		if (h && h->path.compareTo(path) == 0 && 
			(h->method == method || h->method == Method::ALL)) {

			return h;
		}
	}
	return NULL;
}



HttpServer::Method HttpServer::getMethod(String v) {
	if (v.compareTo("GET") == 0) {
		return GET;
	} else if (v.compareTo("POST") == 0) {
		return POST;
	} else if (v.compareTo("PUT") == 0) {
		return PUT;
	} else if (v.compareTo("DELETE") == 0) {
		return DELETE;
	} else if (v.compareTo("HEAD") == 0) {
		return HEAD;
	} else if (v.compareTo("OPTIONS") == 0) {
		return OPTIONS;
	} else if (v.compareTo("CONNECT") == 0) {
		return CONNECT;
	}
	return NONE;
}


// Get the message for a http result code
String HttpServer::getResultMessage(int code) {
	switch (code) {
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 102: return "Processing";
		
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 207: return "Multi-Status";
		case 208: return "Already Reported";
		case 226: return "IM Used";

		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found (Moved Temporarily)";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
		case 307: return "Temporary Redirect";
		case 308: return "Permanent Redirect";

		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Request Entity Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 416: return "Requested range not satisfiable";	
		case 417: return "Expectation Failed";
		case 420: return "Policy Not Fulfilled";
		case 421: return "Misdirected Request";
		case 422: return "Unprocessable Entity";
		case 423: return "Locked";
		case 424: return "Failed Dependency";
		case 426: return "Upgrade Required";
		case 428: return "Precondition Required";
		case 429: return "Too Many Requests";
		case 431: return "Request Header Fields Too Large";
		case 451: return "Unavailable For Legal Reasons";

		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version not supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 508: return "Loop Detected";
		case 509: return "Bandwidth Limit Exceeded";
		case 510: return "Not Extended";
		case 511: return "Network Authentication Required";
	}
	return "unknown";
}


// Init the WifiServer
void HttpServer::initServer(int port) {
	server = new WiFiServer(port);
    server->begin();
	//Serial.printf("Started server on port %d\n", port);
}

