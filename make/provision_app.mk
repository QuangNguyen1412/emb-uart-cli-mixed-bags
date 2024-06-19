TARGET := provision
BUILD_DIR := build
LIBS := $(BUILD_DIR)/libemb_apps_drivers.a

INCLUDE_PATH := provision_app \
								./shared_drivers \
								$(GZ_SHARED_LIBS_DIR)/gz_log/ \
								$(GZ_SHARED_LIBS_DIR)/gz_array/ \
								$(GZ_SHARED_LIBS_DIR)/gz_math \
								$(GZ_SHARED_LIBS_DIR)/gz_hash \
								$(YAPI_SERVICE_DIR)/ \
								../Header_Files/

INCLUDE=$(foreach d, $(INCLUDE_PATH), -I$d)

SOURCES := 	provision_app/*.cpp
						
LDFLAGS += -lpthread

$(TARGET) : $(SOURCES) $(LIBS) 
	${XX} $(CFLAGS) $(LDFLAGS) $(INCLUDE) $^ -o $@
