/*
 *	ConfigServer.ino
 *
 *	copyright (c) Andreas Kraft 2018
 *	Licensed under the BSD 3-Clause License. See the LICENSE file for further details.
 *
 *	Implementation of a Configuration Server.
 */

#include "ConfigServer.h"
#include "HttpServer.h"


// TODO
//	setDelay method : add


String								 ConfigServer::title = "Configuration";
String								 ConfigServer::introText = "";
String								 ConfigServer::resultText = "";
String								*ConfigServer::formFields = NULL;
String								*ConfigServer::defaultValues = NULL;
unsigned int						 ConfigServer::numberOfFormFields = 0;
bool 								 ConfigServer::isActiveServer = false;
HttpServer							*ConfigServer::server = NULL;
ConfigServer::ConfigurationCallback	 ConfigServer::callback; 	


void ConfigServer::check() {
	if ( ! isActiveServer || server == NULL) {
		return;
	}
	server->check();
}


bool ConfigServer::start(const String ssid, 
						 const String fields[], 
						 const String values[], 
						 const unsigned int numberOfFields, 
						 const ConfigurationCallback callb) {
	return start(ssid, "", fields, values, numberOfFields, callb);
}


bool ConfigServer::start(const String ssid, 
						 const String password, 
						 const String fields[], 
						 const String values[], 
						 const unsigned int numberOfFields, 
						 const ConfigurationCallback callb) {
	if (isActiveServer) {
		return true;
	}
	if (ssid == NULL || ssid.length() == 0) {
		return false;
	}
	if (password.length() > 0 && password.length() < 8) {
		Serial.println("ConfigServer: ERROR password to short. Must be 8 chars or longer.");
		return false;
	}
	if (password.length() == 0) {
		Serial.printf("ConfigServer: Initiating open access point (ssid: %s)\n", ssid.c_str());
		WiFi.softAP(ssid.c_str());
	} else {
		Serial.printf("ConfigServer: Initiating secured access point (ssid: %s)\n", ssid.c_str());
		WiFi.softAP(ssid.c_str(), password.c_str());
	}
	Serial.print("ConfigServer: IP address: ");
	Serial.println(WiFi.softAPIP());

	Serial.println("ConfigServer: Starting HTTP Server");
	if (server != NULL) {
		delete server;
	}
	server = new HttpServer();
	server->addHandler("/", HttpServer::GET, _pageRequestHandler);
	server->addHandler("/post", HttpServer::ALL, _postRequestHandler);
	numberOfFormFields = numberOfFields;

	if (fields != NULL && numberOfFormFields > 0) {
		if (formFields != NULL) {
			delete formFields;
		}
		if (defaultValues != NULL) {
			delete defaultValues;
		}

		// copy form fields
		formFields = new String[numberOfFormFields];
		for (unsigned int i = 0; i < numberOfFormFields; i++) {
			formFields[i] = fields[i];
		}

		// initialize default values
		defaultValues = new String[numberOfFormFields];
		setValues(values);
	}
	callback = callb;
	isActiveServer = true;
	Serial.println("ConfigServer: Started");
	return true;
}


void ConfigServer::end(const bool wifioff) {
	if ( ! isActiveServer) {
		return;
	}
	delete server;
	delete [] formFields;
	delete [] defaultValues;
	WiFi.softAPdisconnect(true);
	WiFi.mode(wifioff ? WIFI_OFF : WIFI_STA);
	isActiveServer = false;
	Serial.println("ConfigServer: Access point stopped");
	Serial.println("ConfigServer: Stopped");	
}

void ConfigServer::setTitle(const String str) {
	title = str;
}

void ConfigServer::setIntroText(const String str) {
	introText = str;
}

void ConfigServer::setResultText(const String str) {
	resultText = str;
}


void ConfigServer::setValues(const String values[]) {
	unsigned int nif = 0; // number of input fields
	if (values != NULL) {
		for (unsigned int i = 0; i < numberOfFormFields; i++) {
			if (formFields[i].startsWith("S;")) {
				defaultValues[nif] = (values[nif] != NULL) ? values[nif] : "";
				nif++;
			}
		}
	}
	// fill up the rest with empty strings
	for (unsigned int i = nif; i < numberOfFormFields; i++) {
		defaultValues[i] = "";
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	HttpServer Callbacks

HttpServer::RequestResult ConfigServer::_pageRequestHandler(String path, HttpServer::Method method, long length, String type, char *content) {
	HttpServer::RequestResult result;
	Serial.println("ConfigServer: Received page request");

	result.returnCode = 200;
	result.type = "text/html";
	result.content = "<html><head>";
	result.content += "<title>" + title + "</title>";
	result.content += "<style>";
	result.content += "body {padding: 30px;} ";
	result.content += "#config {font-family: \"Trebuchet MS\", Arial, Helvetica, sans-serif; border-collapse: collapse;} ";
	result.content += "#config tr:nth-child(even) {background-color: #f2f2f2;} ";
	result.content += "#config td {border: 0px; padding: 8px;} ";
	result.content += "#config td.header {margin-top: 20px; padding-top: 12px; padding-bottom: 12px; text-align: left; background-color: #555; color: white;} ";
	result.content += "#config td.name {text-weight: bold;} ";
	result.content += "#config td.field {width: 250px;} ";
	result.content += "#config td.submit {padding-top: 25px; padding-bottom: 25px; height:150%; background-color: white;} ";
	result.content += "#config input {width: 100%; height:100%; font-size: 90%;} ";
	result.content += "#config input[type=submit] {font-size:100%; color: #DA2C43; display: block; height:100%; width: 80%; margin: 0 auto;} ";
	result.content += "</style>";
	result.content += "</head><body>";
	result.content +="<H1>" + title + "</H1>";
	result.content += introText;
	if (formFields != NULL) {
		result.content += "<form action=\"/post\" method=\"get\"><table  border=\"0\" id=\"config\" style=\"margin-top:25px;\">";
		unsigned int nif = 0; // number of input fields
		for (unsigned int i = 0; i < numberOfFormFields; i++) {
			result.content += "<tr>";
			String field = formFields[i];
			if (field.startsWith("H;")) {
				result.content += "<td colspan=\"2\" class=\"header\">" + field.substring(2) + "</td>";
			} else if (field.startsWith("S;")) {
				result.content += "<td class=\"name\">" + field.substring(2) + "</td>";
				result.content += "<td class=\"field\"><input type=\"text\" name=\"" + String(nif) + "\" value=\"" + defaultValues[nif] + "\"></td>";
				nif++;
			}
			result.content += "</tr>";
		}
		result.content += "<tr><td class=\"submit\" colspan=\"2\" style=\"padding-top: 25px;\"><input type=\"submit\" value=\"Submit\"></td></tr>";
		result.content += "</table></form></body></html>";
	}
	return result;
}

HttpServer::RequestResult ConfigServer::_postRequestHandler(String path, HttpServer::Method method, long length, String type, char *content) {
	HttpServer::RequestResult result;
	Serial.println("ConfigServer: Received post request");
	Serial.printf("Path: %s\n", path.c_str());

	// Parse request arguments
	int nrArgs = HttpServer::parseRequestArguments(path);

	// Sort request arguments into an array and call the callback function to push the arguments
	bool cbResult = true;
	if (callback != NULL) {
		String configuration[nrArgs];
		for (unsigned int i = 0; i < nrArgs; i++) {
			configuration[i] = HttpServer::getRequestArgument(String(i));
		}
		setValues(configuration);	// set new default values
		cbResult = (*callback)(configuration);
	}

	// answer something meaningful
	result.returnCode = 200;	// OK
	result.type = "text/html";
	result.content  = "<html><head><title>" + title + "</title>";
	if (cbResult) {
		result.content += "<meta http-equiv=\"refresh\" content=\"3;url=/\" /></head>";
	}
	result.content += "<body>" + resultText + "</body></html>";
	return result;
}
