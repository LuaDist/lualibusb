/* 
 * Authors:	Jorge Visca, Andres Aguirre
 * MINA Group, Facultad de Ingenieria, Universidad de la Republica, Uruguay.
 * http://luaforge.net/projects/lualibusb
 * 
 * License at file end. 
 */

#include <usb.h> /* libusb header */
//#include <unistd.h> /* for geteuid */
#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/*
 * helper. convert a tipical libusb return: 0 ok, negative error.
 * to a tipical lua return: (true) ok, (nil,cod) error.
*/
static int proccess_return_code(lua_State *L, int ret) {
	if (ret==0) {
		lua_pushboolean (L, 1);
		return 1;		
	} else {
		lua_pushnil(L);
		lua_pushnumber (L, ret);
		return 2;
	}	
}

/*
 * helper. pushes a number into the stack, and makes it a global with a name 
*/
static void push_integer_global(lua_State *L, int i, const char * n) {
	lua_pushnumber(L, i);
	lua_setglobal(L, n);
}

static int l_usb_find_busses(lua_State *L) {
	int ret = usb_find_busses();
	lua_pushnumber(L, ret); 
	return 1; /* number of results */
}

static int l_usb_find_devices(lua_State *L) {
	int ret = usb_find_devices();
	lua_pushnumber(L, ret); 
	return 1; /* number of results */
}

static int l_usb_get_busses(lua_State *L) {
	struct usb_bus * bus;
	lua_newtable(L);
	for (bus=usb_busses; bus!=NULL; bus=bus->next) {
		lua_pushstring(L, bus->dirname);//key
		lua_pushlightuserdata (L, bus);//data
		lua_settable(L, -3);  // The table location
		//printf("+++++bus dirname %s \n", bus->dirname);
	}
	return 1; /* number of results */
}

static int l_usb_get_devices(lua_State *L) {
	struct usb_bus * bus = lua_touserdata(L, 1);  
	
	lua_newtable(L);
	struct usb_device * device;
	for (device=bus->devices; device!=NULL; device=device->next) {
		lua_pushstring(L, device->filename);//key
		lua_pushlightuserdata (L, device);//data
		lua_settable(L, -3);  // The table location
		//printf("=====bus %s device %s\n", bus->dirname, device->filename);
	}	
	return 1;
}

static int parse_device_descriptor(lua_State *L) {
	struct usb_device * device = lua_touserdata(L, 1);  
	
	struct usb_device_descriptor descriptor = device->descriptor;
	
	lua_newtable(L);
	lua_pushstring (L, "bLength");			lua_pushnumber(L, descriptor.bLength); lua_settable(L, -3);
	lua_pushstring (L, "bDescriptorType");	lua_pushnumber(L, descriptor.bDescriptorType); lua_settable(L, -3);
	lua_pushstring (L, "bcdUSB");			lua_pushnumber(L, descriptor.bcdUSB); lua_settable(L, -3);
	lua_pushstring (L, "bDeviceClass");		lua_pushnumber(L, descriptor.bDeviceClass); lua_settable(L, -3);
	lua_pushstring (L, "bDeviceSubClass");	lua_pushnumber(L, descriptor.bDeviceSubClass); lua_settable(L, -3);
	lua_pushstring (L, "bDeviceProtocol");	lua_pushnumber(L, descriptor.bDeviceProtocol); lua_settable(L, -3);
	lua_pushstring (L, "bMaxPacketSize0");	lua_pushnumber(L, descriptor.bMaxPacketSize0); lua_settable(L, -3);		
	lua_pushstring (L, "idVendor");			lua_pushnumber(L, descriptor.idVendor); lua_settable(L, -3);
	lua_pushstring (L, "idProduct");		lua_pushnumber(L, descriptor.idProduct); lua_settable(L, -3);
	lua_pushstring (L, "bcdDevice");		lua_pushnumber(L, descriptor.bcdDevice); lua_settable(L, -3);
	lua_pushstring (L, "iManufacturer");	lua_pushnumber(L, descriptor.iManufacturer); lua_settable(L, -3);
	lua_pushstring (L, "iProduct");			lua_pushnumber(L, descriptor.iProduct); lua_settable(L, -3);
	lua_pushstring (L, "iSerialNumber");	lua_pushnumber(L, descriptor.iSerialNumber); lua_settable(L, -3);
	lua_pushstring (L, "bNumConfigurations"); lua_pushnumber(L, descriptor.bNumConfigurations); lua_settable(L, -3);

	return 1;
}

static int parse_endpoint_settings(lua_State *L) {
	struct usb_endpoint_descriptor * descriptor = lua_touserdata(L, 1);  
	
	lua_newtable(L);
	lua_pushstring (L, "bLength");			lua_pushnumber(L, descriptor->bLength); lua_settable(L, -3);
	lua_pushstring (L, "bDescriptorType");	lua_pushnumber(L, descriptor->bDescriptorType); lua_settable(L, -3);
	lua_pushstring (L, "bEndpointAddress");	lua_pushnumber(L, descriptor->bEndpointAddress); lua_settable(L, -3);
	lua_pushstring (L, "bmAttributes");		lua_pushnumber(L, descriptor->bmAttributes); lua_settable(L, -3);
	lua_pushstring (L, "wMaxPacketSize");	lua_pushnumber(L, descriptor->wMaxPacketSize); lua_settable(L, -3);
	lua_pushstring (L, "bInterval");		lua_pushnumber(L, descriptor->bInterval); lua_settable(L, -3);
	lua_pushstring (L, "bRefresh");			lua_pushnumber(L, descriptor->bRefresh); lua_settable(L, -3);		
	lua_pushstring (L, "bSynchAddress");	lua_pushnumber(L, descriptor->bSynchAddress); lua_settable(L, -3);
	lua_pushstring (L, "extra");			lua_pushlstring(L, (char*)descriptor->extra, descriptor->extralen); lua_settable(L, -3);
	
	return 1;
}
static int parse_device_config(lua_State *L) {
	struct usb_device * device = lua_touserdata(L, 1);
	
	struct usb_config_descriptor *config = device->config;
	
	lua_newtable(L);
	lua_pushstring (L, "bLength");	lua_pushnumber(L, config->bLength); lua_settable(L, -3);
	lua_pushstring (L, "bDescriptorType");	lua_pushnumber(L, config->bDescriptorType); lua_settable(L, -3);
	lua_pushstring (L, "wTotalLength");	lua_pushnumber(L, config->wTotalLength); lua_settable(L, -3);
	lua_pushstring (L, "bNumInterfaces");	lua_pushnumber(L, config->bNumInterfaces); lua_settable(L, -3);
	lua_pushstring (L, "bConfigurationValue");	lua_pushnumber(L, config->bConfigurationValue); lua_settable(L, -3);
	lua_pushstring (L, "iConfiguration");	lua_pushnumber(L, config->iConfiguration); lua_settable(L, -3);
	lua_pushstring (L, "bmAttributes");	lua_pushnumber(L, config->bmAttributes); lua_settable(L, -3);
	lua_pushstring (L, "MaxPower");	lua_pushnumber(L, config->MaxPower); lua_settable(L, -3);
	lua_pushstring (L, "interface");	lua_pushlightuserdata(L, config->interface); lua_settable(L, -3);
	lua_pushstring (L, "extra");	lua_pushlstring(L, (char*)config->extra, config->extralen); lua_settable(L, -3);

	return 1;
}

static int parse_interface_alt_settings(lua_State *L) {
	struct usb_interface * interface = lua_touserdata(L, 1);  
	
	struct usb_interface_descriptor altsetting;
	int i;
	lua_newtable(L);
	for (i=0; i < interface->num_altsetting; i++) {
		//printf("+++++ %u \n", i);
		altsetting = interface->altsetting[i];
		lua_pushnumber(L, altsetting.bAlternateSetting); //key
		lua_newtable(L); //value		
		lua_pushstring (L, "bLength");	lua_pushnumber(L, altsetting.bLength); lua_settable(L, -3);
		lua_pushstring (L, "bDescriptorType");	lua_pushnumber(L, altsetting.bDescriptorType); lua_settable(L, -3);
		lua_pushstring (L, "bInterfaceNumber");	lua_pushnumber(L, altsetting.bInterfaceNumber); lua_settable(L, -3);
		lua_pushstring (L, "bAlternateSetting");	lua_pushnumber(L, altsetting.bAlternateSetting); lua_settable(L, -3);
		lua_pushstring (L, "bNumEndpoints");	lua_pushnumber(L, altsetting.bNumEndpoints); lua_settable(L, -3);
		lua_pushstring (L, "bInterfaceClass");	lua_pushnumber(L, altsetting.bInterfaceClass); lua_settable(L, -3);
		lua_pushstring (L, "bInterfaceSubClass");	lua_pushnumber(L, altsetting.bInterfaceSubClass); lua_settable(L, -3);
		lua_pushstring (L, "bInterfaceProtocol");	lua_pushnumber(L, altsetting.bInterfaceProtocol); lua_settable(L, -3);
		lua_pushstring (L, "iInterface");	lua_pushnumber(L, altsetting.iInterface); lua_settable(L, -3);
		lua_pushstring (L, "endpoint");	lua_pushlightuserdata(L, altsetting.endpoint); lua_settable(L, -3);
		lua_pushstring (L, "extra");	lua_pushlstring(L, (char*)altsetting.extra, altsetting.extralen); lua_settable(L, -3);
	}
	lua_settable(L, -3);

	return 1;
}

static int l_usb_open(lua_State *L) {
	struct usb_device * device = lua_touserdata(L, 1);  
	usb_dev_handle * dev_handle = usb_open(device);
	lua_pushlightuserdata (L, dev_handle);
	return 1; /* number of results */
}

static int l_usb_close(lua_State *L) {
	struct usb_dev_handle * device = lua_touserdata(L, 1);  
	int ret = usb_close(device);

	return (proccess_return_code(L, ret));
}

static int l_usb_set_configuration(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int configuration = lua_tonumber(L, 2);  
	
	int ret = usb_set_configuration(dev_handle, configuration);
	
	return (proccess_return_code(L, ret));
}

static int l_usb_set_altinterface(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int alternate = lua_tonumber(L, 2);  
	
	int ret = usb_set_altinterface(dev_handle, alternate); 
	
	return (proccess_return_code(L, ret));
}

static int l_usb_claim_interface(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int interface = lua_tonumber(L, 2);  
	
	int ret = usb_claim_interface(dev_handle, interface); 
	
	return (proccess_return_code(L, ret));
}

static int l_usb_clear_halt(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	unsigned int ep = lua_tonumber(L, 2);  
	
	int ret = usb_clear_halt(dev_handle, ep); 
	
	return (proccess_return_code(L, ret));
}

static int l_usb_reset(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	
	int ret = usb_reset(dev_handle); 
	
	return (proccess_return_code(L, ret));
}

static int l_usb_release_interface(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int interface = lua_tonumber(L, 2);  
	
	int ret = usb_release_interface(dev_handle, interface); 
	
	return (proccess_return_code(L, ret));
}

static int l_usb_control_msg(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int requesttype = lua_tonumber(L, 2);  
	int request = lua_tonumber(L, 3);
	int value = lua_tonumber(L, 4);
	int index = lua_tonumber(L, 5);
	const char *bytes = lua_tostring (L, 6);
	int size = lua_strlen (L, 6);
	int timeout = lua_tonumber(L, 7);
	
	//make a copy just in case (why it isn't const?)
	char *bytes_copy=(char*)malloc(size);
	memcpy(bytes_copy, bytes, size);
	
	int ret = usb_control_msg(dev_handle, requesttype, request, 
							value, index, bytes_copy, size, timeout);	
	
	if( ret < 0 ) {
		lua_pushnil (L);
		lua_pushnumber (L, ret);
		return 2; /* number of results */   
	} else{
		lua_pushlstring (L, bytes_copy, ret);
		return 1; /* number of results */   
	}
}

static int l_usb_get_string_simple(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int index = lua_tonumber(L, 2);  
	size_t buflen = lua_tonumber(L, 3);  /* get 3rd argument, int */
	
	if (buflen==0) { buflen=255; }  //default len	
	char *buf=(char*)malloc(buflen);
	
	int ret = usb_get_string_simple(dev_handle, index, buf, buflen);
	
	if( ret < 0 ) {
		lua_pushnil (L);
		lua_pushnumber (L, ret);
		return 2; /* number of results */   
	} else{
		lua_pushlstring (L, buf, ret);
		return 1; /* number of results */   
	}
}

static int l_usb_bulk_write(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1); 
	int ep = lua_tonumber(L, 2); 
	const char *bytes = lua_tostring (L, 3);
	int size = lua_strlen (L, 3);
	int timeout = lua_tonumber (L, 4);
	
	int ret = usb_bulk_write(dev_handle, ep, bytes, size, timeout);
	
	if( ret < 0 ) {
		lua_pushnil (L);
		lua_pushnumber (L, ret);
		return 2; /* number of results */   
	} else{
		lua_pushnumber (L, ret);
		return 1; /* number of results */   
	}	

}

static int l_usb_bulk_read(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);
	int ep = lua_tonumber(L, 2);
	int size = lua_tonumber (L, 3);
	int timeout = lua_tonumber (L, 4);
	
	char *bytes=(char*)malloc(size);
	int ret = usb_bulk_read(dev_handle, ep, bytes, size, timeout);
	
	if( ret < 0 ) {
		lua_pushnil (L);
		lua_pushnumber (L, ret);
		return 2;
	} else{
		lua_pushlstring (L, bytes, ret);
		return 1; /* number of results */   
	}
}
static int l_usb_interrupt_write(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int ep = lua_tonumber(L, 2);  
	const char *bytes = lua_tostring (L, 3);
	int size = lua_strlen (L, 3);
	int timeout = lua_tonumber (L, 4);
	
	int ret = usb_interrupt_write(dev_handle, ep, bytes, size, timeout);
	
	if( ret < 0 ) {
		lua_pushnil (L);
		lua_pushnumber (L, ret);
		return 2; /* number of results */   
	} else{
		lua_pushnumber (L, ret);
		return 1; /* number of results */   
	}	
}

static int l_usb_interrupt_read(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int ep = lua_tonumber(L, 2);  
	int size = lua_tonumber (L, 3);
	int timeout = lua_tonumber (L, 4);
	
	char *bytes=(char*)malloc(size);
	int ret = usb_interrupt_read(dev_handle, ep, bytes, size, timeout);
	
	if( ret < 0 ) {
		lua_pushnil (L);
		lua_pushnumber (L, ret);
		return 2; /* number of results */   
	} else{
		lua_pushlstring (L, bytes, ret);
		return 1; /* number of results */   
	}
}

static int l_usb_get_descriptor(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int type = lua_tonumber(L, 2);  
	int index = lua_tonumber (L, 3);
	int size = lua_tonumber (L, 4);
	
	if (size==0) {size=255;} //default value
	char *buf=(char*)malloc(size);
	
	int ret = usb_get_descriptor(dev_handle, 
			(unsigned char)type, (unsigned char)index, buf, size);
	
	if( ret < 0 ) {
		lua_pushnil (L);
		lua_pushnumber (L, ret);
		return 2; /* number of results */   
	} else{
		lua_pushlstring (L, buf, ret);
		return 1; /* number of results */   
	}
}

static int l_usb_get_descriptor_by_endpoint(lua_State *L) {
	struct usb_dev_handle * dev_handle = lua_touserdata(L, 1);  
	int ep = lua_tonumber (L, 2);
	int type = lua_tonumber(L, 3);  
	int index = lua_tonumber (L, 4);
	int size = lua_tonumber (L, 5);
	
	if (size==0) {size=255;} //default value
	char *buf=(char*)malloc(size);
	
	int ret = usb_get_descriptor_by_endpoint(dev_handle, ep,
			(unsigned char)type, (unsigned char)index, buf, size);
	
	if( ret < 0 ) {
		lua_pushnil (L);
		lua_pushnumber (L, ret);
		return 2; /* number of results */   
	} else{
		lua_pushlstring (L, buf, ret);
		return 1; /* number of results */   
	}
}

static const struct luaL_reg libusb [] = {
		//core
		{ "find_busses", l_usb_find_busses }, 
		{ "find_devices",l_usb_find_devices }, 
		{ "get_busses", l_usb_get_busses },

		{ "get_devices", l_usb_get_devices },

		//Device operations
		{ "open", l_usb_open },
		{ "close", l_usb_close },
		{ "set_configuration", l_usb_set_configuration },
		{ "set_altinterface", l_usb_set_altinterface },
		{ "clear_halt", l_usb_clear_halt },
		{ "reset", l_usb_reset },
		{ "claim_interface", l_usb_claim_interface },
		{ "release_interface", l_usb_release_interface },
		
		//Control Transfers
		{ "control_msg", l_usb_control_msg },		
		{ "get_string_simple", l_usb_get_string_simple },
		//TODO: usb_get_string
		{ "get_descriptor", l_usb_get_descriptor },
		{ "get_descriptor_by_endpoint", l_usb_get_descriptor_by_endpoint },
		
		//Bulk Transfers
		{ "bulk_write", l_usb_bulk_write },
		{ "bulk_read", l_usb_bulk_read },

		//Interrupt Transfers
		{ "interrupt_write", l_usb_interrupt_write },
		{ "interrupt_read", l_usb_interrupt_read },
		
				
		{ "device_descriptor", parse_device_descriptor },
		{ "device_config", parse_device_config },
		{ "interface_alt_settings", parse_interface_alt_settings },
		{ "endpoint_descriptor", parse_endpoint_settings },
		
		{ NULL, NULL } /* sentinel */
};

int luaopen_libusb(lua_State *L) {
	usb_init();
	usb_find_busses();
	usb_find_devices();

	luaL_openlib(L, "libusb", libusb, 0);

	//connect_usb()
	// add a table in the registry to hold all C pointer / userdata equivalents
	// the table has "weak values" mode
	/*lua_pushliteral(L, "usb_userdata_map"); // "dbus_userdata_map"
	 lua_newtable(L); // "usb_userdata_map" {}
	 lua_newtable(L); // "usb_userdata_map" {} map_meta
	 lua_pushliteral(L, "__mode"); // "usb_userdata_map" {} map_meta "__mode"
	 lua_pushliteral(L, "v"); // "usb_userdata_map" {} map_meta "__mode" "v"
	 lua_settable(L, -3); // "usb_userdata_map" {} map_meta
	 lua_setmetatable(L, -2); // "usb_userdata_map" {}
	 lua_settable(L, LUA_REGISTRYINDEX); //

	 luaL_register(L, "libusb", libusb); // {usb}*/

	push_integer_global(L, USB_CLASS_PER_INTERFACE, "USB_CLASS_PER_INTERFACE");
	push_integer_global(L, USB_CLASS_AUDIO,"USB_CLASS_AUDIO");
	push_integer_global(L, USB_CLASS_COMM,"USB_CLASS_COMM");
	push_integer_global(L, USB_CLASS_HID,"USB_CLASS_HID");
	push_integer_global(L, USB_CLASS_PRINTER,"USB_CLASS_PRINTER");
	push_integer_global(L, USB_CLASS_PTP,"USB_CLASS_PTP");
	push_integer_global(L, USB_CLASS_MASS_STORAGE,"USB_CLASS_MASS_STORAGE");
	push_integer_global(L, USB_CLASS_HUB,"USB_CLASS_HUB");
	push_integer_global(L, USB_CLASS_DATA,"USB_CLASS_DATA");
	push_integer_global(L, USB_CLASS_VENDOR_SPEC,"USB_CLASS_VENDOR_SPEC");

	push_integer_global(L, USB_DT_DEVICE,"USB_DT_DEVICE");
	push_integer_global(L, USB_DT_CONFIG,"USB_DT_CONFIG");
	push_integer_global(L, USB_DT_STRING,"USB_DT_STRING");
	push_integer_global(L, USB_DT_INTERFACE,"USB_DT_INTERFACE");
	push_integer_global(L, USB_DT_ENDPOINT,"USB_DT_ENDPOINT");

	push_integer_global(L, USB_DT_HID,"USB_DT_HID");
	push_integer_global(L, USB_DT_REPORT,"USB_DT_REPORT");
	push_integer_global(L, USB_DT_PHYSICAL,"USB_DT_PHYSICAL");
	push_integer_global(L, USB_DT_HUB,"USB_DT_HUB");

	push_integer_global(L, USB_DT_DEVICE_SIZE,"USB_DT_DEVICE_SIZE");
	push_integer_global(L, USB_DT_CONFIG_SIZE,"USB_DT_CONFIG_SIZE");
	push_integer_global(L, USB_DT_INTERFACE_SIZE,"USB_DT_INTERFACE_SIZE");
	push_integer_global(L, USB_DT_ENDPOINT_SIZE,"USB_DT_ENDPOINT_SIZE");
	push_integer_global(L, USB_DT_ENDPOINT_AUDIO_SIZE,"USB_DT_ENDPOINT_AUDIO_SIZE");
	push_integer_global(L, USB_DT_HUB_NONVAR_SIZE,"USB_DT_HUB_NONVAR_SIZE");

	return 1;
}

/************************************************************************
*
* Lua bindings for libusb.
* Copyright 2008 MINA Group, Facultad de Ingenieria, Universidad de la
* Republica, Uruguay. 
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
************************************************************************/