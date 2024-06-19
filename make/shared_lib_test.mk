TARGET := lib_test
BUILD_DIR := build
LIBS := $(BUILD_DIR)/libemb_apps_drivers.a

INCLUDE_PATH := ../shared_lib_test/ \
								$(GZ_SHARED_LIBS_DIR)/gz_log/ \
								$(GZ_SHARED_LIBS_DIR)/gz_array/ \
								$(GZ_SHARED_LIBS_DIR)/gz_math \
								$(GZ_SHARED_LIBS_DIR)/gz_observer \
								../Header_Files/

INCLUDE=$(foreach d, $(INCLUDE_PATH), -I$d)

SOURCES := 	shared_lib_test/*.cpp \
						$(GZ_SHARED_LIBS_DIR)/gz_observer/*.c
						
LDFLAGS += -lpthread

$(TARGET) : $(SOURCES)
	${XX} $(CFLAGS) $(LDFLAGS) $(INCLUDE) $^ -o $@
