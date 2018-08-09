# ConfigServer

A framework to set-up and operate a temporary configuration web page for an application.

## Installation

- Copy the files from this directory to your project.
- Also copy the .h and .ino files from the following sub-projects to your project:
	- [HttpServer](../HttpServer/README.md)  
	- [LinkedList](../LinkedList/README.md)  

## Usage

When calling one of the ``start()`` methods the ConfigServer puts the Arduino into *Access Point + Station* mode and thereby creates a new WiFi network with the provided *ssid*. In addition an HTTP server is started that hosts a configuration web page.

The IP address for the configuration web server is **192.168.4.1**. The configuration web page can be accessed at [http://192.168.4.1](http://192.168.4.1).

### Configuration Fields and Values

Before starting the server one must define two array to setup the configuration web page:

- One array of strings contains the basic layout of the configuration page. Each entry defines one line on the page and has a specific format:
	-  **Header**: ``"H;A Header"``
	-  **String input**: ``"S;An inputfield"``
- The second array of strings contains default values for the input fields in the page. These entries are related to the *String input* fields of the first array, ignoring the *header* fields (i.e. the first entry in this array is the default value for the first input field).

Example:

```cpp
String fields[] = { "H;First Headline", "S;First Input Field", "S;Second Input Field",
                    "H;Second Headline", "S;Third Input Field"};
String values[] = { "first value", "second value",
                    "third value"};
```

### Starting the server

After defining the input field and value arrays the configuration server can be started.

```cpp
ConfigServer::start("Arduino", "WiFiPassword", fields, values, 5, configurationCallback);
```
This enables the *Access Point + Station* mode of the WiFi module and creates a new secured WiFi network 	*Arduino* with the password *WiFiPassword*. 
It also creates an HTTP server that serves the configuration page.

After starting the configuration server one must call the ``check()`` method very regularly, for example in the ``loop()`` function of a sketch,

```cpp
void loop() {
	...
	ConfigServer::check();
	...
}
```

### Handling Callbacks

The callback function provided with the ``start()`` method is called when the configuration form is submitted. This function can be used to handle and apply the new configuration data.

```cpp
bool configurationCallback(const String configuration[]) {
	...
	return true;
}
```

The *configuration* array, similar to the *values* array, contains the submitted form data in the same order as the input fields.

A callback function must return a boolean value, which indicates whether the configuration web page will be displayed again automatically after a few seconds (*true*) or the view stays on the result page (*false*). 

#### Stopping the Server

The configuration server can be stopped with the ``end()`` method.

```cpp
ConfigServer::end();
```

This method stops the HTTP server as well as puts the WiFi module back in normal *Station* mode. When the optional *wifioff* argument is set to *true* then WiFi is fully switch off.


## Class Documentation

The configuration server defines the following public methods and types.

### Constructors

Since the *ConfigServer* class only offers static methods, it does not have a callable constructor.

### Start and Stop Methods

- **static bool start(const String ssid, const String fields[], const String values[], const unsigned int numberOfFields, const ConfigurationCallback callback)**  
Start the ConfigServer. After this method is called, a new *open* WiFi network is available. Also, a new HTTP server on the device is also available that serves the configuration web page.  
*ssid* is the ssid for the new *open* WiFi network.  
*fields* is an array of fields that specify the input fields for the configuration form. See the description for further information about input field formats.  
*values* is an array of default values for the input fields. The number of entries in this array must be equal or less the number of entries in *fields*.  
*numberOfFields* specifies the number of entries in *fields*.  
*callback* is a callback function that receives new values from the configuration form.
- **static bool start(const String ssid, const String password, const String fields[], const String values[], const unsigned int numberOfFields, const ConfigurationCallback callback)**  
Start the ConfigServer. After this method is called, a new *secured* WiFi network is available. Also, a new HTTP server on the device is also available that serves the configuration web page.  
*ssid* is the ssid for the new *secured* WiFi network.  
*password* is the WiFi password for the new secured WiFi network. The password must be at least 8 characters long.  
*fields* is an array of fields that specify the input fields for the configuration form. See the description for further information about input field formats.  
*values* is an array of default values for the input fields. The number of entries in this array must be equal or less the number of entries in *fields*.  
*numberOfFields* specifies the number of entries in *fields*.  
*callback* is a callback function that receives submitted new values from the configuration form.
- **static void end(const bool wifioff = false)**  
Shutdown the ConfigServer, the HTTP server, and the WiFi network.  
*wifioff* specifies whether the devices only switches off the AccessPoint (*false*) or the whole WiFi module (*true*).

### Server Methods

- **static void check()**  
Check for an incoming HTTP request. This method must be called very regularly (e.g. every few hundred milliseconds) in order to receive and process requests.

### Miscellaneous

- **static void setTitle(const String title)**  
Set a title for the configuration page. This is presented as the headline of the page.  
*title* is the new title.
- **static void setIntroText(const String introText)**  
Set an introduction text for the configuration page. This text is presented between the title and the configuration form.  
*introText* the introduction text. This text can contain either plain text or HTML.
- **static void setResultText(const String resultText)**  
Set the text that is presented after successfully handling the form parameter by the callback function.  
*resultText* the result text. This text can contain either plain text or HTML.
- **static void setValues(const String values[])**  
Set new default values for the configuration form.  
*values* is an array of default values for the input fields. The format, number and order of the values is the same as with the *value* argument of the *start()* methods.

### Types and Definitions

- **typedef bool (\*ConfigurationCallback)(const String configuration[])**  
This typedef defines the configuration callback's function signature. The callback receives an array of Strings with new values for the configuration input fields.  
The format, number and order of the values in the *configuration* array is the same as with the *value* argument of the *start()* methods.  
A callback function must return a boolean value, which indicates whether the configuration web page will be displayed again after a few seconds (*true*) or not (*false*).

## Limitations

Tested on ESP8266 and ESP32 Arduino boards.

## License

Licensed under the BSD 3-Clause License. See the [LICENSE](../LICENSE) file for further details.