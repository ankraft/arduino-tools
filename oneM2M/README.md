# oneM2M

This class implements a very small but useful subset of
[oneM2M's](http://www.onem2m.org) resources and the restful *Mca* interface 
to access resources on an CSE.


## Installation

- Copy the files from this directory to your project.
- Also copy the .h and .ino files from the following sub-projects to your project:
	- [HttpServer](../HttpServer/README.md)  
	- [LinkedList](../LinkedList/README.md)  
- This class also uses the [ArduinoJson](https://arduinojson.org) library.


## Supported Resources & Limitations

The following oneM2M resource are supported.

- CSEBase (only retrieval)
- AE (create, retrieval)
- Container (create, retrieval)
- ContentInstance (create, retrieval of latest from Container)
- Subscription (create, retrieval, notifications)

In addition four low-level access method are available to allow for
more sophisticated calls to the target CSE, if necessary, as well as to
support own implementations of further oneM2M resource types.

- **createResource()**
- **getResource()**
- **updateResource()**
- **deleteResource()**

See the detailed class description below for more details.

Only *JSON* is supported for request and answer encodings.

Only **non-secure** CSE connections are supported, yet.


## Usage & Examples

A prerequisite for the following examples is that the Arduino is connected to 
a LAN via WiFi or other means.

### Contact and Retrieve a CSE

The following example shows how to create a *cse* object and how to retrieve 
the CSEBase resource from a oneM2M CSE. When creating the *cse* object it
gets passed the host name, port, base path on the server as well as the
originator information.

Note, that checking the length of the ```getCSE()```'s
return value in the examples demonstrates how to check for a successful call
to the CSE. Most functions return the actual JSON structure returned by the request. This way more sophisticated applications    can check the actual return value and other attributes.

```cpp
# include "oneM2M.h"

OneM2M cse("cse.example.com", 8080, "/", "originator");

void setup() {
    if (cse.getCSE().length() > 0) {
        Serial.println("successfully retrieved CSE");
    }
}
```


### Create and Retrieve Resources

The following examples demonstrate two ways to create an AE resource on a 
CSE and then retrieve it. The first example first creates and then 
retrieves it (the path of the AE is */cse-name/ae1*):

```cpp
if (cse.createAE("/cse-name/myAE", "myAE").length() > 0) {
    String ae = cse.retrieveAE("/cse-name/myAE");
}
```

The second example shows the use of the ```getAE()``` method. This method,
if successful, always returns an AE resource. If it doesn't exist on the CSE 
then it is created first.

```cpp
if (cse.getAE("/cse-name/myAE", "myAE").length() > 0) {
    Serial.println("successfully retrieved AE");
}
```

The same scheme apply to the handling of Container resources, with the methods
 ```createContainer()```, ```retrieveContainer()```, and ```getContainer()```
respectively.


### Add a ContentInstance to a Container

The following example shows how to add a ContentInstance resource to a 
Container on a CSE:

```cpp
if (cse.addContentInstance("/cse-name/myAE/aContainer", "some content").length() > 0) {
    Serial.println("successfully added ContentInstance");
}
```

The content type will be set to *text/plain:0* by default.


### Retrieve a Latest ContentInstance

This example demonstrates how to retrieve the latest ContentInstance resource 
from a Container.

```cpp
OneM2M::Content content = cse.getLatestContentInstance("/cse-name/myAE/aContainer");
if (content.state) {
    Serial.println(content.content);
}
```

The *content* object above holds various information from the latest 
ContentInstance. The `state` attribute indicates whether the resource was 
retrieved successfully, while the *content* attribute contains the actual 
content data. See the description of the *Content* structure below.


### Delete a Resource 

There is only one generic method for deleting any resources.

```cpp
if (cse.deleteResource("/cse-name/myAE/aContainer").length() > 0) {
    Serial.println("resource deleted");
}
```


### Listen to Notifications

This class also offers the possibility to subscripe to notifications 
from a CSE when resources, for example, are created or updated. 

For this, and before subscription resources can be created, the notification
sub-system must be initialized.  
This is done by calling the  *setupNotifications()* static method on the
*OneM2M* class:

```cpp
OneM2M::setupNotifications();
```

This, among other things, starts an HTTP server that receives the notifications
from the CSE.

The next step is to define a callback function that is called whenever a 
notification is received. A callback function can receive notifications for 
more than one subscription, but there can only be one callback function for
each subscription.

```cpp
void notificationCallback(String resourceIdentifier, OneM2M::ResourceType type, String resource) {
    Serial.println(resourceIdentifier);
    Serial.println(content);
    Serial.println(type);
}
```

The last step is to create a subscription and assign the callback function to 
it:

```cpp
cse.getSubscriptionNotify("/cse/ae/container/aSubscription", notificationCallback);
```

This adds a subscription (resource name *aSubscription*) to the resource */cse/ae/container* and assign the above defined *notificationCallback* to it. The *getSubscriptionNotify()* method first retrieves or creates a subscription 
resource and then registers the callback function via the 
*addNotificationCallback()* static method.

## Class Documentation

The *OneM2M* class has the following public methods.

Most of the CSE-access methods return a String object. If a request was
successful, then this string contains the full answer to the request from the
CSE. Usually and if not stated otherwise in the descriptions below, this is
a JSON encoded oneM2M resource. If a request was not successful, then this String is empty (length == 0).


### Constructor

- **OneM2M::OneM2M(String host, int port, String basePath, String originator)**  
The class's constructor.  
*host* specifies the host name to the CSE, while 
*port* specifies the CSE's port. *basePath* is an optional path for the CSE.
*originator* are the originator's credentials to access the CSE.

### oneM2M Resources Methods

#### CSE

- **String getCSE(void)**  
Retrieve the CSEBase resource.

#### AE

- **String retrieveAE(String path)**  
Retrieve an AE resource.  
*path* is the resource path of the AE resource. The last path element must
be the resource name of the AE.
- **String createAE(String path, String appID)**  
Create an AE resource.  
*path* is the resource path of the AE resource. The last path element must
be the resource name of the AE.  
*appID* is the application ID for that AE.
- **String getAE(String path, String appID)**  
Retrieve an AE. The AE is created in case it does not exist yet.  
*path* is the resource path of the AE resource. The last path element 
must be the resource name of the AE.  
*appID* is the application ID of that AE.

#### Container

- **String retrieveContainer(String path)**  
Retrieve a Container resource.  
*path* is the resource path of the Container resource. The last path
element must be the resource name of the Container.
- **String createContainer(String path)**  
Create a Container resource.  
*path* is the resource path of the Container. The last path element must be
the resource name of the Container.
- **String getContainer(String path)**  
Retrieve a Container. The Container is created in case it does not exist yet.  
*path* is the resource path of the Container. The last path element must be
the resource name of the Container.

#### ContentInstance

- **String addContentInstance(String path, String content)**  
Add a ContentInstance resource to a Container, with a default content. 
type of *text/plain:0*.  
*path* is the resource path of the Container, NOT the ContentInstance.  
*content* is the actual content for the ContentInstance. 
- **String addContentInstance(String path, String content, String contentType)**  
Add a ContentInstance resource to a Container with the possibility to
specifying a content type.  
*path* is the resource path of the Container, NOT the ContentInstance.  
*content* is the actual content for the ContentInstance. *contentType* 
is the content's content type.
- **Content getLatestContentInstance(String path)**  
Retrieve the latest content instance from the CSE.  
*path* is the resource path of the Container, NOT the ContentInstance.  
This method returns a *Content* structure that contains the content, 
content type, and creation date of the retrieved content instance. For 
further details see the description (*Class Structures* below).
- **Content contentFromContentInstance(String resource)**  
Extract the *Content* information from a JSON-encoded resource String.  
*resource* is the JSON-encoded resource.  
This method returns a *Content* structure that contains the content, 
content type, and creation date of the retrieved content instance.

#### Subscription

Note, that notifications need to be enabled before creating a Subscription resource (see [Notification Methods](#NotificationMethods) below).

- **String getSubscription(String path)**  
Retrieve a Subscription resource. The Subscription is created in case
it does not exist yet.    
*path* is the resource path of the Subscription resource. The last path
element must be the resource name of the Subscription.
- **String retrieveSubscription(String path)**  
Retrieve a Subscription resource.  
*path* is the resource path of the Subscription. The last path element 
must be the resource name of the Subscription.
- **String addSubscription(String path)**  
Add a Subscription resource to another resource.  
*path* is the resource path of the Subscription resource. The last path
element must be the resource name of the Subscription.
- **String getSubscriptionNotify(String path, NotificationCallback callback)**  
Retrieve a Subscription resource. The Subscription is created in case
it does not exist yet. In addition a notification callback function
is registered for this subscription.  
*path* is the resource path of the Subscription resource. The last path
element must be the resource name of the Subscription.  
*callback* is the callback function for notifications for this 
subscription.

#### Direct Access 

These methods can be used to access a CSE and directly pass JSON-encoded 
resources. The calling function is responsible for correct encoding, 
decoding and interpretation of the JSON result. In case of an error the 
methods return a zero-length String.

- **String createResource(String path, int type, String content)**  
Create a resource on the CSE.  
*path* is the resource path of the resource. The last path element must be
the new resource name of the resource.  
*type* is the resource type of the new resource.  
*content* must contain a valid resource in JSON encoding.
- **String getResource(String path, int type)**  
Retrieve a resource from the CSE.  
*path* is the resource path of the resource.  
*type* is the resource type of the resource.
- **String updateResource(String path, int type, String content)**  
Update an existing resource on the CSE. 
*path* is the resource path of the resource.  
*type* is the resource type of the resource.  
*content* must contain a valid resource update in JSON encoding. 
- **String deleteResource(String path)**  
Delete a resource from the CSE.  
*path* is the resource path of the resource.

### Static Methods

The OneM2M class defines the following static methods for all instances of the 
class.

#### Notification Methods[](#NotificationMethods)
- **static void setupNotifications(void)**  
Setup and initialize the notification sub-system. This method starts an HTTP
server that listens on port 1440 for notification events from connected CSEs.
There is only one notification sub-system for all instances of the OneM2M
class.  
This or another setup method must be called before Subscription resources can
be created and notifications can be received.
- **static void setupNotifications(String host, int port, String path)**  
Setup and initialize the notification sub-system. This method works the same
as the method without parameters, but with this method one can specify own
parameters for the HTTP server setup.  
*host* is the external host address of the HTTP server to bind to.  
*port* is the port to bind to.  
*path* is the path on the HTTP server for notifications.

- **static void shutdownNotifications(void)**  
Shutdown the notification sub-system. This also shuts down the HTTP server.
After the shutdown no notifications will be received and no new Subscription
resources can be created.
- **static bool addNotificationCallback(String subscriptionResourceID, NotificationCallback callback)**  
Add a callback functionfor a specific subscription's resource ID that is called
when a notification for that subscription is received. The method can be
called multiple times to update the callback function for the same resource
ID.  
*subscriptionResourceID* is the resource ID of the subscription.  
*callback* is a pointer to a function that will receive the notification
information. See also the description for *NotificationCallback*.  
The method returns true when the addition / update was successful.
- **static bool removeNotificationCallback(String subscriptionResourceID)**  
Remove the callback function for specific subscription's resource ID.  
*subscriptionResourceID* is the resource ID of the subscription.  
The method returns true when the removal was successful.
- **static void checkNotifications(void)**  
This method handles receiving and processing of incoming notifications.
This method must be called very regurlarly (e.g. every few hundred 
milliseconds) to check for incoming notification requests from the CSE.


#### Miscellaneous Functions
- **static void setJsonMaxSize(int size)**  
Set the size of the internal JSON buffers. Since memory is very limited on
Arduino devices the buffers for parsing JSON structures are limited to *1024*
bytes by default. If, for example, large ContentInstance resources are
retrieved then the maximum buffer size must be increased.  
*size* is the new maximum size for internal JSON buffers.
- **static int jsonMaxSize(void)**  
Return the current set size of internal JSON buffers.
- **static String getResourceIdentifier(String resource)**  
Get the resource ID from a JSON-encoded resource.  
*resource* is a JSON-encoded resource.  
This method returns the resource ID as a string.


### Class Types

The *OneM2M* class defines the following types.


#### Enum ResourceType

This enum type defines the values for oneM2M resource types.
 
A special value (UNKNOWN = -1) is used in case of an unknown or unidentified type.


#### Struct Content

This structure contains results when retrieving ContentInstances. It contains the following fields:

- **String resourceIdentifier**  
The Resource identifier of the resource.
- **String content**  
The actual content.
- **String contentFormat**  
The content format description. This specifies how the content is encoded. 
See oneM2M **TS-0004 - Service Layer Core Protocol Specification**, 
section **m2m:encodingType** for further details.
- **String creationTime**  
The creation time of the resource. The format is 'Basic Format' as specified
in ISO8601.
- **bool state**  
This flag indicates the resource's retrieval state. It is set to *true* when
the retrieval was successful, *false* otherwise.


#### NotificationCallback

This type defines the signature for notification callback functions.

- **void (* NotificationCallback)(String resourceIdentifier, Types type, String content)**

Callback functions will receive the following parameters:
- *resourceIdentifier* is the resource identifier.
- *type* is the resource type. Since Types is an enum (and therefore an
integer) it might be one of the values defined in the enum *Types*, but also 




## Compatibility
This class has been tested with the following oneM2M & CSE implementations:

- [Eclipse om2m](https://www.eclipse.org/om2m/)

Other implementations might work as well. Please let me know of successful deployments.

## License
Licensed under the BSD 3-Clause License. See the [LICENSE](../LICENSE) file for further details.
