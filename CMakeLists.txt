# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Drahoš
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
# Please note that the package source code is licensed under its own license.

PROJECT(lualibusb C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
INCLUDE(dist.cmake)

FIND_PACKAGE (LibUSB REQUIRED)

INCLUDE_DIRECTORIES(${LibUSB_INCLUDE_DIR})

# Build
ADD_LUA_MODULE(libusb lualibusb.c)
TARGET_LINK_LIBRARIES(libusb ${LibUSB_LIBRARIES})

# Install all files and documentation
INSTALL (TARGETS libusb DESTINATION ${INSTALL_CMOD})
INSTALL (FILES COPYRIGHT README DESTINATION ${INSTALL_DATA})
INSTALL (FILES test.lua DESTINATION ${INSTALL_TEST})

