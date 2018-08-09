/*
 *	ConfigServer.h
 *
 *	copyright (c) Andreas Kraft 2018
 *	Licensed under the BSD 3-Clause License. See the LICENSE file for further details.
 *
 *	Implementation of a Configuration Server.
 */

# ifndef __CONFIGSERVER__
# define __CONFIGSERVER__

#ifdef ESP32
#include <WiFi.h>
#endif
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#include "HttpServer.h"


class ConfigServer {
public:

	// typedef for configuration callback
	typedef bool (*ConfigurationCallback)(const String configuration[]);


private:
	static String					 title;
	static String					 introText;
	static String 					 resultText;
	static String					*formFields;
	static String 					*defaultValues;
	static unsigned int				 numberOfFormFields;
	static bool 					 isActiveServer;
	static HttpServer				*server;
	static ConfigurationCallback	 callback; 	

	static HttpServer::RequestResult _pageRequestHandler(String path, HttpServer::Method method, long length, String type, char *content);
	static HttpServer::RequestResult _postRequestHandler(String path, HttpServer::Method method, long length, String type, char *content);

public:

	// Start the ConfigServer. After this method is called, a new open WiFi network is available. Also, a new 
	// HTTP server on the device is also available that serves the configuration web page.
	// *ssid* is the ssid for the new open WiFi network.
	// *fields* is an array of fields that specify the input fields for the configuration form. See the description for further information about input field formats.
	// *values* is an array of default values for the input fields. The number of entries in this array must be equal or less the number of entries in *fields*.
	// *numberOfFields* specifies the number of entries in *fields*.
	// *callback* is a callback function that receives new values from the configuration form.
	static bool start(const String ssid, 
					  const String fields[], 
					  const String values[], 
					  const unsigned int numberOfFields, 
					  const ConfigurationCallback callback);

	// Start the ConfigServer. After this method is called, a new secured WiFi network is available. Also, a new 
	// HTTP server on the device is also available that serves the configuration web page.
	// *ssid* is the ssid for the new secured WiFi network.
	// *password* is the WiFi password for the new secured WiFi network. The password must be at least 8 characters long.
	// *fields* is an array of fields that specify the input fields for the configuration form. See the description for further information about input field formats.
	// *values* is an array of default values for the input fields. The number of entries in this array must be equal or less the number of entries in *fields*.
	// *numberOfFields* specifies the number of entries in *fields*.
	// *callback* is a callback function that receives new values from the configuration form.
	static bool start(const String ssid, 
					  const String password, 
					  const String fields[], 
					  const String values[], 
					  const unsigned int numberOfFields, 
					  const ConfigurationCallback callback);

	// Shutdown the ConfigServer, the HTTP server, and the WiFi network.
	// *wifioff* specifies whether the devices only switches off the AccessPoint (*false*) or the whole WiFi module (*true*).
	static void end(const bool wifioff = false);

	// Check for an incoming HTTP request. This method must be called very regularly (e.g. every few hundred
	//	milliseconds) in order to receive and process requests.
	static void check();

	// Set a title for the configuration page. This is presented as the headline of the page.
	// *title* is the new title.
	static void setTitle(const String title);

	// Set an introduction text for the configuration page. This text is presented between the title and
	// the configuration form.
	//*introText* the introduction text. This text can contain either plain text or HTML.
	static void setIntroText(const String introText);

	// Set the text that is presented after successfully handling the form parameter by the callback
	// function.
	// *resultText* the result text. This text can contain either plain text or HTML.
	static void setResultText(const String resultText);

	// Set new default values for the configuration form. 
	// *values* is an array of default values for the input fields. The format, number and order of the values is the same as with the *value* argument of the *start()* methods.
	static void setValues(const String values[]);
};

# endif
