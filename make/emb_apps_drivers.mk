BUILD_DIR = ./build
# TARGET := $(BUILD_DIR)/libemb_apps_drivers.a
TARGET := $(BUILD_DIR)/libemb_apps_drivers.a

INCLUDE_PATH := ../Header_Files/ \
								../Header_Files/yapi_callbacks \
								./shared_drivers \
								$(GZ_SHARED_LIBS_DIR)/gz_math \
								$(GZ_SHARED_LIBS_DIR)/gz_array \
								$(GZ_SHARED_LIBS_DIR)/gz_hash \
								$(GZ_SHARED_LIBS_DIR)/gz_log \
								$(GZ_SHARED_LIBS_DIR) \
								$(YAPI_SERVICE_DIR)

INCLUDE=$(foreach d, $(INCLUDE_PATH), -I$d)

# application code
# SOURCES_APP := ../app/app_bms.cpp

# Shared drivers
DRIVERS_SOURCES := 	shared_drivers/*.cpp

# Shared libraries
LIB_SOURCES := $(GZ_SHARED_LIBS_DIR)/gz_math/*.c \
							$(GZ_SHARED_LIBS_DIR)/gz_array/*.c \
							$(GZ_SHARED_LIBS_DIR)/gz_log/*.c \
							$(GZ_SHARED_LIBS_DIR)/gz_hash/*.c \
							$(YAPI_SERVICE_DIR)/*.c

OBJ_FILES_APP := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(wildcard $(SOURCES_APP))) )

OBJ_FILES_HAL += $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(wildcard $(DRIVERS_SOURCES))) )

OBJ_FILES_GZ += $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(wildcard $(LIB_SOURCES))) )

$(info OBJ_FILES_APP: $(OBJ_FILES_APP) )
$(info OBJ_FILES_HAL: $(OBJ_FILES_HAL) )
$(info OBJ_FILES_GZ: $(OBJ_FILES_GZ) )

# Give all the source directories to make
VPATH = $(sort $(dir $(SOURCES_APP)) )
VPATH += $(sort $(dir $(DRIVERS_SOURCES)) )
VPATH += $(sort $(dir $(LIB_SOURCES)) )

$(BUILD_DIR)/%.o : %.cpp
	$(shell mkdir -p $(BUILD_DIR) )
	$(XX) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $< -o $@

$(BUILD_DIR)/%.o : %.c
	$(shell mkdir -p $(BUILD_DIR) )
	$(XX) $(CFLAGS) $(LDFLAGS) $(INCLUDE) -c $< -o $@

$(TARGET) : $(OBJ_FILES_APP) $(OBJ_FILES_HAL) $(OBJ_FILES_GZ)
	${AR} rcs $@ $^

# $(TARGET) : $(OBJ_FILES_APP) $(OBJ_FILES_HAL) $(OBJ_FILES_GZ) 
# 	${AR} rcs $@ $^
