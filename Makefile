# author: Quang Nguyen (quang.nguyen@goalzero.com)
# brief: Master Makefile for 6g unit test project

export XX := g++
export AR := ar
export LDFLAGS := -std=c++11 -Wall
export DEBUG_FLAGS := -g3 -O0
export CFLAGS := -D PLATFORM_linux -D BUILD_Y6G_4000
export GZ_SHARED_LIBS_DIR := ./shared_libraries
export YAPI_SERVICE_DIR := ./shared_libraries/yapi_service

TEST_SUITE :=	shared_lib_test

UTILITIES_APPS := ota_host_app \
									provision_app \
									socket_app

EMB_APPS_DRIVERS := emb_apps_drivers

ifeq (1,$(DEBUG))
    # Found DEBUG flag in DEFINES
		override CFLAGS += -D DEBUG $(DEBUG_FLAGS)
else
endif

.PHONY: ${EMB_APPS_DRIVERS} ${TEST_SUITE} 

all: $(TEST_SUITE)

$(TEST_SUITE): $(EMB_APPS_DRIVERS)
	@$(MAKE) -f make/$(basename $(notdir $@)).mk

${UTILITIES_APPS}: $(EMB_APPS_DRIVERS)
	@$(MAKE) -f make/$(basename $(notdir $@)).mk

$(EMB_APPS_DRIVERS):
	@$(MAKE) -f make/$(basename $(notdir $@)).mk

clean:
	rm -f ota_host
	rm -f provision
	rm -f lib_test
	rm -f socket
	rm -r ./build
