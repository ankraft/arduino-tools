/*
 *	oneM2M.h
 *
 *	copyright (c) Andreas Kraft 2018
 *	Licensed under the BSD 3-Clause License. See the LICENSE file for further details.
 *
 *	Implementation of a small subset oneM2M's resources and Mca interface.
 */

# ifndef __ONEM2M_H__
# define __ONEM2M_H__

# include "HttpServer.h"
# include "LinkedList.h"

class OneM2M {
public:

	// Enumeration for oneM2M resource types
	enum ResourceType {
		UNKNOWN = -1,
		ACCESSCONTROLPOLICY = 1,
		AE = 2, 
		CONTAINER = 3, 
		CONTENTINSTANCE = 4, 
		CSE = 5,
		DELIVERY = 6,
		EVENTCONFIG = 7,
		EXECINSTANCE = 8,
		GROUP = 9,
		LOCATIONPOLICY = 10,
		M2MSERVICESUBSCRIPTIONPROFILE = 11,
		MGMTCMD = 12,
		MGMTOBJ = 13,
		node = 14,
		pollingChannel = 15,
		REMOTECSE = 16,
		REQUEST = 17,
		SCHEDULE = 18,
		SERVICESUBSCRIBEDAPPRULE = 19,
		SERVICESUBSCRIBEDNODE = 20,
		STATSCOLLECT = 21,
		STATSCONFIG = 22,
		SUBSCRIPTION = 23,
		SEMANTICDESCRIPTOR = 24,
		NOTIFICATIONTARGETMGMTPOLICYREF = 25,
		NOTIFICATIONTARGETPOLICY = 26,
		POLICYDELETIONRULES = 27,
		FLEXCONTAINER = 28,
		TIMESERIES = 29,
		TIMESERIESINSTANCE = 30,
		ROLE = 31,
		TOKEN = 32,
		TRAFFICPATTERN = 33,
		DYNAMICAUTHORIZATIONCONSULTATION = 34,
		AUTHORIZATIONDECISION = 35,
		AUTHORIZATIONPOLICY = 36,
		AUTHORIZATIONINFORMATION = 37,
		ONTOLOGYREPOSITORY = 38,
		ONTOLOGY = 39,
		SEMANTICMASHUPJOBPROFILE = 40,
		SEMANTICMASHUPINSTANCE = 41,
		SEMANTICMASHUPRESULT = 42,
		AECONTACTLIST = 43,
		AECONTACTLISTPERCSE = 44
	};


	// Structure to hold the results when retrieving a content instance.
	struct Content {
		String 	resourceIdentifier;		// Resource identifier of the resource
		String 	content;				// Actual content
		String 	contentFormat;			// Content format description
		String 	creationTime;			// Creation time of the resource
		bool	state;					// Indicate the resource's retrieval state 
	};

	// typedef for notification callback functions
	typedef void (*NotificationCallback)(String resourceIdentifier, OneM2M::ResourceType type, String resource);


private:

	struct PathElements {
		String path;
		String rn;
	};

	struct NotificationCBStruct {
		String 					subscriptionResourceID;
		NotificationCallback 	callback;
		// calback
	};

	String 										 _host;
	int											 _port;
	String 										 _basePath;
	String 										 _originator;
	
	static int 		 							 _jsonSize;			// Size for JSON buffers
	static HttpServer							*_notificationServer;
	static String		 						 _notificationURL;
	static LinkedList<NotificationCBStruct *>	 _notificationCallbacks;


	OneM2M();	// prevent usage of simple ctor

	String 										 _getPath(String resourceName);

	static NotificationCBStruct 				*_getCallback(String resourceIdentifier);
	static ResourceType 						 _getCallbackType(String resourceIdentifier);
	static HttpServer::RequestResult			 _notificationRequestHandler(String path, 
																			 HttpServer::Method method, 
																			 long length,
																			 String type, 
																			 char *content);
	static String 								 _getRequestContent(WiFiClient client, int expectedReturnCode);
	static PathElements 						 _splitPath(String path);
	static String 								 _escapeJSON(String value);


public:

	//	Most of the CSE-access methods return a String object. If the call was
	//	successful, then this string contains the full answer to the request
	//	from the CSE. Usually and if not stated otherwise in the descriptions
	//	below, this is a JSON encoded oneM2M resource. If the call was not
	//	successful, then this String is empty (length == 0).

	//	The class's constructor.  
	//	*host* specifies the host name to the CSE, while 
	//	*port* specifies the CSE's port. *basePath* is an optional path for the CSE.
	//	*originator* are the originator's credentials to access the CSE.
	OneM2M(String host, int port, String basePath, String originator);


	//	Retrieve the CSEBase resource.
	String 			getCSE(void);


	//
	//	AE
	//

	//	Retrieve an AE resource. The AE is created in case it does not exist yet.
	//	*path* is the resource path of the aE resource. The last path
	//	element must be the resource name of the Subscription.
	//	*appID* is the application ID of that AE.
	String 			getAE(String path, String appID);

	//	Retrieve an AE resource.
	//	*path* is the resource path of the AE resource. The last path element
	//	must be	the resource name of the AE.
	//	*appID* is the application ID of that AE.
	String 			retrieveAE(String path);

	//	Create an AE. 
	//	*path* is the resource path of the AE resource. The last path element
	//	must be the resource name of the AE.
	//	*appID* is the application ID of that AE.
	String 			createAE(String path, String appID);


	//
	//	Container
	//

	//	Retrieve a Container resource. The Container is created in case it does
	//	not exist yet.  
	//	*path* is the resource path of the Container resource. The last path
	//	element must be the resource name of the Container.
	String 			getContainer(String path);

	//	Retrieve a Container resource.
	//	*path* is the resource path of the Container. The last path element 
	//	must be the resource name of the Container.
	String 			retrieveContainer(String path);

	//	Create a Container.
	//	*path* is the resource path of the Container. The last path element
	//	must be the resource name of the Container.
	String 			createContainer(String path);


	//
	//	ContentInstance
	//

	//	Add a ContentInstance resource to a Container, with a default content
	//	type of *text/plain:0*.  
	//	*path* is the resource path of the Container, NOT the ContentInstance.
	//	*content* is the actual content of the ContentInstance. 
	String 			addContentInstance(String path, String content);

	//	Add a ContentInstance resource to a Container with the possibility to
	//	specifying a content type.
	//	*path* is the resource path of the Container, NOT the ContentInstance.
	//	*content* is the actual content of the ContentInstance. *contentType* 
	//	is the content's content type.
	String 			addContentInstance(String path, String content, String contentType);

	//	Retrieve the latest content instance from the CSE.  
	//	*path* is the resource path of the Container, NOT the ContentInstance.  
	//	This method returns a *Content* structure that contains the content, 
	//	content type, and creation date of the retrieved content instance.
	Content 		getLatestContentInstance(String path);

	//	Extract the *Content* information from a JSON-encoded resource String.
	//	*resource* is the JSON-encoded resource.
	//	This method returns a *Content* structure that contains the content, 
	//	content type, and creation date of the retrieved content instance.
	Content 		contentFromContentInstance(String resource);


	//
	//	Subscription
	//

	//	Retrieve a Subscription resource. The Subscription is created in case
	//	it does not exist yet. 
	//	*path* is the resource path of the Subscription resource. The last path
	//	element must be the resource name of the Subscription.
	String 			getSubscription(String path);

	//	Retrieve a Subscription resource.
	//	*path* is the resource path of the Subscription. The last path element 
	//	must be the resource name of the Subscription.
	String 			retrieveSubscription(String path);

	//	Add a Subscription resource to another resource.  
	//	*path* is the resource path of the Subscription resource. The last path
	//	element must be the resource name of the Subscription.
	String 			addSubscription(String path);

	//	Retrieve a Subscription resource. The Subscription is created in case
	//	it does not exist yet. In addition a notification callback function
	//	is registered for this subscription.
	//	*path* is the resource path of the Subscription resource. The last path
	//	element must be the resource name of the Subscription.
	//	*callback* is the callback function for notifications for this  
	//	subscription.
	String 			getSubscriptionNotify(String path, NotificationCallback callback);


	//	Create a resource on the CSE.
	//	*path* is the resource path of the resource. The last path element 
	//	must be the new resource name of the resource.
	//	*type* is the resource type of the new resource.
	//	*content* must contain a valid resource in JSON encoding.
	String 			createResource(String path, int type, String content);

	//	Retrieve a resource from the CSE.
	//	*path* is the resource path of the resource.
	//	*type* is the resource type of the resource.
	String 			getResource(String path, int type);

	//	Update an existing resource on the CSE.
	//	*path* is the resource path of the resource.
	//	*type* is the resource type of the resource.
	//	*content* must contain a valid resource update in JSON encoding. 
	String 			updateResource(String path, int type, String content);

	//	Delete a resource from the CSE.
	//	*path* is the resource path of the resource.
	String			deleteResource(String path);


	//////////////////////////////////////////////////////////////////////////
	//
	// 	Static Functions
	//

	//
	// 	Notification Functions
	//

	//	Setup and initialize the notification sub-system. This method starts
	//	an HTTP server that listens on port 1440 for notification events from
	//	connected CSEs. There is only notification one sub-system for all 
	//	instances of the OneM2M class.
	//	This or another setup method must be called before Subscription 
	//	resources can be created and notifications can be received.
	static void 	setupNotifications(void);

	//	Setup and initialize the notification sub-system. This method works the
	//	same as the method without parameters, but with this method one can
	//	specify own parameters for the HTTP server setup.
	//	*host* is the external host address of the HTTP server to bind to.
	//	*port* is the port to bind to.
	//	*path* is the path on the HTTP server for notifications.
	static void 	setupNotifications(String host, int port, String path);

	//	Shutdown the notification sub-system. This also shuts down the HTTP
	//	server. After the shutdown no notifications will be received and no
	//	new Subscription resources can be created.
	static void 	shutdownNotifications(void);

	//	Add a callback function for a specific subscription's resource ID that 
	//	is called when a notification for that subscription is received. The 
	//	method canbe called multiple times to update the callback function for 
	//	the same resource ID.
	//	*subscriptionResourceID* is the resource ID of the subscription.
	//	*callback* is a pointer to a function that will receive the 
	//	notification information. See also the description for 
	//	*NotificationCallback*.
	//	The method returns true when the addition / update was successful.
	static bool 	addNotificationCallback(String subscriptionResourceID, NotificationCallback callback);

	//	Remove the callback function for specific subscription's resource ID.
	//	*subscriptionResourceID* is the resource ID of the subscription.
	//	The method returns true when the removal was successful.
	static bool 	removeNotificationCallback(String subscriptionResourceID);

	//	This method handles receiving and processing of incoming notifications.
	//	This method must be called very regurlarly (e.g. every few hundred
	//	milliseconds) to check for incoming notification requests from the CSE.  
	static void		checkNotifications(void);


	//
	// 	Miscellaneous Functions
	//

	//	Set the size of the internal JSON buffers. Since memory is very limited
	//	on Arduino devices the buffers for parsing JSON structures are limited
	//	to *1024* bytes by default. If, for example, large ContentInstance 
	//	resources are retrieved then the maximum buffer size must be increased.
	//	*size* is the new maximum size for internal JSON buffers.
	static void 	setJsonMaxSize(int size);

	//	Return the current set size of internal JSON buffers.
	static int 		jsonMaxSize(void);

	//	Get the resource ID from a JSON-encoded resource.
	//	*resource* is a JSON-encoded resource.
	//	This method returns the resource ID as a string.
	static String 	getResourceIdentifier(String resource);

};

# endif