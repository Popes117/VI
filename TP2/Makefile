CXX      := g++ 
CXXFLAGS := -std=c++11 -O3 -Wall -fopenmp
LDFLAGS  := 
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
SHELL	 := /bin/bash

TARGET   := VI-RT-V4-PathTracing
TINYXML := tinyxml2-master

INCLUDE  := -I$(TARGET)/Camera/ -I$(TARGET)/Image -I$(TARGET)/Light -I$(TARGET)/Primitive -I$(TARGET)/Primitive/BRDF -I$(TARGET)/Primitive/Geometry -I$(TARGET)/Rays -I$(TARGET)/Renderer -I$(TARGET)/Scene -I$(TARGET)/Shader -I$(TARGET)/utils -I$(TARGET)/Image/ToneMapper -I$(TARGET)/Image/PostFilter -I$(TINYXML)

SRC      :=                      \
   $(wildcard $(TARGET)/*.cpp) \
   $(wildcard $(TARGET)/Camera/*.cpp)         \
   $(wildcard $(TARGET)/Image/*.cpp)         \
   $(wildcard $(TARGET)/Primitive/BRDF/*.cpp)         \
   $(wildcard $(TARGET)/Primitive/Geometry/*.cpp)         \
   $(wildcard $(TARGET)/Renderer/*.cpp)         \
   $(wildcard $(TARGET)/Scene/*.cpp)         \
   $(wildcard $(TARGET)/Shader/*.cpp)         \
   $(wildcard $(TARGET)/Matrix/*.cpp)         \
   $(filter-out $(TINYXML)/xmltest.cpp, $(wildcard $(TINYXML)/*.cpp)) \

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all:	build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	cp $(TARGET)/Image/*.ppm $(APP_DIR)
	cp $(TARGET)/Image/*.hdr $(APP_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean 

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*


