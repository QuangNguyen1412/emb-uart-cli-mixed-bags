TARGET := cpp_test
BUILD_DIR := build
LIBS := $(BUILD_DIR)/libemb_apps_drivers.a

INCLUDE_PATH := cpp_test_app \
								$(GZ_SHARED_LIBS_DIR)/gz_log \
								$(GZ_SHARED_LIBS_DIR)/gz_hash \
								$(GZ_SHARED_LIBS_DIR)/gz_math \
								$(GZ_SHARED_LIBS_DIR)/gz_array 

INCLUDE=$(foreach d, $(INCLUDE_PATH), -I$d)

SOURCES := 	cpp_test_app/*.cpp
						
LDFLAGS += -lpthread

$(TARGET) : $(SOURCES) $(LIBS)
	${XX} $(CFLAGS) $(LDFLAGS) $(INCLUDE) $^ -o $@
