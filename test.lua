#!/usr/bin/lua
--[[
Authors: Jorge Visca, Andres Aguirre
MINA - INCO - Facultad de Ingeniería - UDELAR 
http://luaforge.net/projects/lualibusb

License at file end. 
]]


assert(package.loadlib("Debug/libluausb.so","luaopen_libusb"))()

--libusb.find_busses()
--libusb.find_devices()

print ("\n------------------------------------")
print ("-- A few exported constants:")
print ("USB_CLASS_HUB", USB_CLASS_HUB)
print ("USB_DT_STRING", USB_DT_STRING)

print ("\n------------------------------------")
print ("-- Dumping libusb:", libusb)
for k, _ in pairs(libusb) do
	print (k)
end

local buses=libusb.get_busses()
local last_device
	
print ("\n------------------------------------")
print ("-- Dumping buses&devices:", buses)
for dirname, bus in pairs(buses) do
	--print (k)
	local devices=libusb.get_devices(bus)
	for filename, device in pairs(devices) do

		local descriptor=libusb.device_descriptor(device)		
		print ("Bus: " .. dirname, "Device: " .. filename, 
				"ID: "..string.format("%04x:%04x",descriptor.idVendor,descriptor.idProduct)) --, "desc:", descriptor.iProduct, s or err)
		last_device=device		
		
	end
	
end

print ("\n------------------------------------")
print ("-- Device descriptor:", last_device)
local descriptor=libusb.device_descriptor(last_device)
for k, v in pairs(descriptor) do
	print (k, v)
end

print ("\n------------------------------------")
print ("-- Device configuration:", last_device)
local config=libusb.device_config(last_device)
for k, v in pairs(config) do
	print (k, v)
end

print ("\n------------------------------------")
print ("-- Interface settings:", last_device)
local altsettings=libusb.interface_alt_settings(config.interface)
for k, v in pairs(altsettings) do
	print ("+ Alternative", k, v)
	for ka, va in pairs(v) do
		print (ka, va)
	end	
end

--[[
*
* Lua bindings for libusb.
* Copyright (c) 2009 - MINA - INCO - Facultad de Ingeniería - UDELAR 
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
]]
