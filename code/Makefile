#############################################################################
#
# Project Makefile
#
# (c) Wouter van Ooijen (www.voti.nl) 2016
#
# This file is in the public domain.
# 
#############################################################################

# source files in this project (main.cpp is automatically assumed)
SOURCES := $(wildcard src/*.cpp)

# header files in this project
HEADERS := $(wildcard headers/*.hpp)

# other places to look for files for this project
SEARCH  := ./headers ./src

#add moving-platform
MOVING	:= ../../moving_platform/code
SEARCH  += $(MOVING)/headers $(MOVING)/src
SOURCES += $(wildcard $(MOVING)/src/*.cpp)
HEADERS += $(wildcard $(MOVING)/headers/*.hpp)

# set REATIVE to the next higher directory 
# and defer to the Makefile.due there
RELATIVE := $(RELATIVE)../
include $(RELATIVE)Makefile.due

