# Copyright (C) 2007-2012 LuaDist.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

project ( lualibusb C )
cmake_minimum_required ( VERSION 2.8 )
include ( cmake/dist.cmake )

find_package ( LibUSB REQUIRED )
include_directories ( ${LibUSB_INCLUDE_DIR} )

# Build
install_lua_module ( libusb lualibusb.c LINK ${LibUSB_LIBRARIES} )
install_data ( COPYRIGHT README )
install_test ( test.lua )

