/*
 *	oneM2M.ino
 *
 *	copyright (c) Andreas Kraft 2018
 *	Licensed under the BSD 3-Clause License. See the LICENSE file for further details.
 *
 *	Implementation of a small subset oneM2M's resources and Mca interface.
 */

// TODO: support labels

#include <ArduinoJson.h>


int 		 								 OneM2M::_jsonSize = 1024;
HttpServer									*OneM2M::_notificationServer = NULL;
String										 OneM2M::_notificationURL;
LinkedList<OneM2M::NotificationCBStruct *>	 OneM2M::_notificationCallbacks;


OneM2M::OneM2M(String host, int port, String basePath, String originator) {
	_host = host;
	_port = port;
	_basePath = basePath;
	_originator = originator;
}


//
//	CSE
//

String OneM2M::getCSE(void) {
	return getResource("", ResourceType::CSE);
} 


//
//	AE
//

String OneM2M::getAE(String path, String appID) {
	String result = retrieveAE(path);
	if (result.length() == 0) {
		return createAE(path, appID);
	}
	return result;
}


String OneM2M::retrieveAE(String path) {
	return getResource(path, ResourceType::AE);
} 


String OneM2M::createAE(String path, String appID) {
	PathElements elements = _splitPath(path);
	return createResource(elements.path, 
				ResourceType::AE, 
				"{\"m2m:ae\":{\"rn\":\"" + elements.rn + "\",\"api\":\"" + appID + "\", \"rr\":true}}");
}



//
//	Container
//

String OneM2M::getContainer(String path) {
	String result = retrieveContainer(path);
	if (result.length() == 0) {
		return createContainer(path);
	}
	return result;
}


String OneM2M::retrieveContainer(String path) {
	return getResource(path, ResourceType::CONTAINER);
} 


String OneM2M::createContainer(String path) {
	PathElements elements = _splitPath(path);
	return createResource(elements.path, 
				ResourceType::CONTAINER, 
				"{\"m2m:cnt\":{\"rn\":\"" + elements.rn + "\"}}");
}


//
//	ContentInstance
//

String OneM2M::addContentInstance(String path, String content) {
	return addContentInstance(path, content, "text/plain:0");
}


String OneM2M::addContentInstance(String path, String content, String contentType) {
	return createResource(path, // no split here. CIN does not provide an rn
				ResourceType::CONTENTINSTANCE, 
				"{\"m2m:cin\":{\"cnf\":\"" + contentType + "\",\"con\":\"" + _escapeJSON(content) + "\"}}");
}


OneM2M::Content OneM2M::getLatestContentInstance(String path) {
	return contentFromContentInstance(getResource(path + "/la", ResourceType::CONTENTINSTANCE));
}


OneM2M::Content OneM2M::contentFromContentInstance(String resource) {
	OneM2M::Content 	result;
	DynamicJsonBuffer 	jsonBuffer(_jsonSize);

	result.state = false;
	if (resource.length() == 0) {
		return result;
	}
	JsonObject& cin = jsonBuffer.parseObject(resource);
	if (cin.success()) {
		result.resourceIdentifier = cin["m2m:cin"]["ri"].as<String>();
		result.content = cin["m2m:cin"]["con"].as<String>();
		result.contentFormat = cin["m2m:cin"]["cnf"].as<String>();
		result.creationTime = cin["m2m:cin"]["ct"].as<String>();
		result.state = true;
	}
	return result;
}


//
//	Subscription
//

String OneM2M::getSubscription(String path) {
	String result = retrieveSubscription(path);
	if (result.length() == 0) {
		return addSubscription(path);
	}
	return result;
}


String OneM2M::retrieveSubscription(String path) {
	return getResource(path, ResourceType::SUBSCRIPTION);
}


String OneM2M::addSubscription(String path) {
	if (_notificationServer == NULL) {
		return "";
	}
	PathElements elements = _splitPath(path);
	return createResource(elements.path, 
				ResourceType::SUBSCRIPTION, 
				"{\"m2m:sub\":{\"rn\":\"" + elements.rn + "\",\"nu\":\"" + _notificationURL + "\",\"nct\":2}}");
}


String OneM2M::getSubscriptionNotify(String path, NotificationCallback callback) {
	String sc = cse.getSubscription(path);

	if (sc.length() > 0 and callback != NULL) {
		addNotificationCallback(getResourceIdentifier(sc), callback);
	}
	return sc;
}



//
//	General OneM2M functions
//


String OneM2M::createResource(String path, int type, String content) {
	WiFiClient client;

	if ( ! client.connect(_host.c_str(), _port)) {
		Serial.println("connection failed");
		client.stop();
		return "";
	}

	String request =	"POST " + path + " HTTP/1.1\r\n" +
						"Host: " + _host + "\r\n" +
						"X-M2M-Origin: " + _originator + "\r\n" +
						"Content-Type: application/json;ty=" + type + "\r\n" +
						"Content-Length: " +  content.length() + "\r\n" +
						"Accept: application/json\r\n" +
						"Connection: close\r\n\r\n" + 
						content;
	//Serial.println(request + "\n");
	client.print(request);
	return _getRequestContent(client, 201);
}


String OneM2M::getResource(String path, int type) {
	WiFiClient client;

	if ( ! client.connect(_host.c_str(), _port)) {
		Serial.println("connection failed");
		client.stop();
		return "";
	}
	String request =	"GET " + _getPath(path) + " HTTP/1.1\r\n" +
						"Host: " + _host + "\r\n" +
						"X-M2M-Origin: " + _originator + "\r\n" +
						"Content-Type: application/json;ty=" + type + "\r\n" +
						"Accept: application/json\r\n" +
						"Connection: close\r\n\r\n";
	client.print(request);
 	return _getRequestContent(client, 200);
}


String OneM2M::updateResource(String path, int type, String content) {
	WiFiClient client;

	if ( ! client.connect(_host.c_str(), _port)) {
		Serial.println("connection failed");
		client.stop();
		return "";
	}

	String request =	"PUT " + path + " HTTP/1.1\r\n" +
						"Host: " + _host + "\r\n" +
						"X-M2M-Origin: " + _originator + "\r\n" +
						"Content-Type: application/json;ty=" + type + "\r\n" +
						"Content-Length: " +  content.length() + "\r\n" +
						"Accept: application/json\r\n" +
						"Connection: close\r\n\r\n" + 
						content;
	// Serial.println(request + "\n");
	client.print(request);
	return _getRequestContent(client, 200);
}


String OneM2M::deleteResource(String path) {
	WiFiClient client;

	if ( ! client.connect(_host.c_str(), _port)) {
		Serial.println("connection failed");
		client.stop();
		return "";
	}

	String request =	"DELETE " + path + " HTTP/1.1\r\n" +
						"Host: " + _host + "\r\n" +
						"X-M2M-Origin: " + _originator + "\r\n" +
						"Content-Type: application/json\r\n" +
						"Connection: close\r\n\r\n";
	// Serial.println(request + "\n");
	client.print(request);
	return _getRequestContent(client, 200);
}


//
//	Internal helper functions
//


// Normalize path
String OneM2M::_getPath(String resourceName) {
	String t = _basePath + resourceName;
	t.replace("//", "/");
	return t;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Static class function
//

//
//	Miscellaneous functions
//

void OneM2M::setJsonMaxSize(int value) {
	_jsonSize = value;
}


int OneM2M::jsonMaxSize(void) {
	return _jsonSize;
}


String OneM2M::getResourceIdentifier(String resource) {
	DynamicJsonBuffer 	jsonBuffer(OneM2M::jsonMaxSize());

	JsonObject& json = jsonBuffer.parseObject(resource);
	if (json.success()) {
		JsonObject::iterator it=json.begin(); // only first one
		return json[it->key]["ri"].as<String>()	;
	}
	return "";
}


// Escape JSON special chars
String OneM2M::_escapeJSON(String value) {
	String result = value;

	result.replace("\\", "\\\\");
	result.replace("\n", "\\n");
	result.replace("\r", "\\r");
	result.replace("\t", "\\t");
	result.replace("\"", "\\\"");
	// a bit of optimization: don't handle chars that are likely not used at all.
	// \b  Backspace (ascii code 08)
	// \f  Form feed (ascii code 0C)
	return result;
}


// Split the path in the resource path and the name
OneM2M::PathElements OneM2M::_splitPath(String path) {
	PathElements elements;
	int l = path.lastIndexOf("/");
	elements.path = path.substring(0,l);
	elements.rn = path.substring(l+1);
	return elements;
}


// Get the content of a request answer
String OneM2M::_getRequestContent(WiFiClient client, int expectedReturnCode) {
	String result = "";
	String status = "";

	unsigned long timeout = millis() + 10000; // TODO Config
	while (client.available() == 0) {
		if (OneM2M::_notificationServer != NULL) {	// if notifications enabled call the server to receive srq requests.
			checkNotifications();
		}
		if (timeout < millis()) { 
			Serial.println(">>> Client Timeout !");
			client.stop();
			return "";
		}
 	}

	if (client.available()){
		status = client.readStringUntil('\n');
		//Serial.println(status);
	}
	if (status.indexOf(String(expectedReturnCode)) == -1) {	// error
		client.stop();
		return "";
	}
	// read header
	while(client.available()){
		String x = client.readStringUntil('\n');
		x.trim();
		if (x.length() == 0) {	// end of header reached
			break;
		}
	}
	// read content 
	while(client.available()){
		String x = client.readStringUntil('\n');
		x.trim();
		result += x; 
	}
	client.stop();
	// Serial.println("Result:");
	// Serial.println(result);

	// The following is a hack to fill the buffer in case the call was successul
	// but the request returns an empty answer. This happens, for example, when
	// a resource is deleted.
	if (result.length() == 0) {
		result = "ok";
	}
	return result;
}


//////////////////////////////////////////////////////////////////////////////

//
//	Notification Functions 
//


void OneM2M::setupNotifications() {
	OneM2M::setupNotifications(WiFi.localIP().toString(), 1440, "/onem2m/Notifications");
}


void OneM2M::setupNotifications(String host, int port, String path) {
	if (_notificationServer) {
		return;
	}
	_notificationURL = "http://" + host + ":" + String(port) + path;
	_notificationServer = new HttpServer(port);
	_notificationServer->addHandler(path, HttpServer::POST, _notificationRequestHandler);
}


void OneM2M::shutdownNotifications(void) {
	if (_notificationServer == NULL) {
		return;
	}
	delete _notificationServer;
	_notificationCallbacks.clear();
	_notificationServer = NULL;
}


bool OneM2M::addNotificationCallback(String subscriptionResourceID, NotificationCallback callback) {
	if (subscriptionResourceID != NULL && subscriptionResourceID.length() == 0) {
		return false;
	}
	NotificationCBStruct *cb =  _getCallback(subscriptionResourceID);
	if (cb) {
		cb->callback = callback;
		return true;
	}
	cb = new NotificationCBStruct();
	cb->subscriptionResourceID = subscriptionResourceID;
	cb->callback = callback;
	_notificationCallbacks.add(cb);
	return true;
}


bool OneM2M::removeNotificationCallback(String subscriptionResourceID) {
	if (subscriptionResourceID != NULL && subscriptionResourceID.length() == 0) {
		return false;
	}
	for (int i = 0; i < _notificationCallbacks.size(); i++) {
		NotificationCBStruct *cb = _notificationCallbacks.get(i);
		if (cb->subscriptionResourceID == subscriptionResourceID) {
			_notificationCallbacks.remove(i);
			return true;
		} 
	}
	return false;
}


//	Check for notification server activities
void OneM2M::checkNotifications(void) {
	if (_notificationServer != NULL) {
		_notificationServer->check();
	}
}


// Handle the internal interpretation and routing of notifications
HttpServer::RequestResult OneM2M::_notificationRequestHandler(String path, 
															  HttpServer::Method method, 
															  long length,
															  String type, 
															  char *content) {
	DynamicJsonBuffer 			jsonBuffer(OneM2M::jsonMaxSize());
	HttpServer::RequestResult	result;
	result.returnCode = 200;
	result.attributes = "X-M2M-RSC: 2000";

	JsonObject& json = jsonBuffer.parseObject(content);
	if (json.success()) {
		if (json.containsKey("m2m:sgn")) {
			// verification request
			if (json["m2m:sgn"]["m2m:vrq"]) {
				return result;
			}
			// notification request 
			String sur = json["m2m:sgn"]["m2m:sur"];
			String rep = json["m2m:sgn"]["m2m:nev"]["m2m:rep"];
			if (sur && rep) {
				NotificationCBStruct *cb = _getCallback(sur);
				if (cb && cb->callback) {
					(*cb->callback)(sur, _getCallbackType(rep), rep); 
				}
				return result;
			}
		}
	}
	result.returnCode = 400;
	result.attributes = "X-M2M-RSC: 4000";
	return result;
}


// search and return a callback struct
OneM2M::NotificationCBStruct *OneM2M::_getCallback(String resourceIdentifier) {
	for (int i = 0; i < _notificationCallbacks.size(); i++) {
		NotificationCBStruct *cb = _notificationCallbacks.get(i);
		if (cb->subscriptionResourceID == resourceIdentifier) {
			return cb;
		} 
	}
	return NULL;
}


// search and return a callback struct
OneM2M::ResourceType OneM2M::_getCallbackType(String resource) {
	DynamicJsonBuffer 	jsonBuffer(OneM2M::jsonMaxSize());

	JsonObject& json = jsonBuffer.parseObject(resource);
	if (json.success()) {
		JsonObject::iterator it=json.begin(); // only first one
		return (OneM2M::ResourceType)json[it->key]["ty"].as<int>()	;
	}
	return OneM2M::ResourceType::UNKNOWN;
}


