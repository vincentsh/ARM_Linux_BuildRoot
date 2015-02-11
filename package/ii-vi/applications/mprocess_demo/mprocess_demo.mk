################################################################################
#
# mprocess_demo
#
################################################################################

SRC_DIR = mprocess_demo

MPROCESS_DEMO_VERSION := 1.0.0
MPROCESS_DEMO_SITE := package/ii-vi/applications/$(SRC_DIR)
MPROCESS_DEMO_SITE_METHOD := local
MPROCESS_DEMO_INSTALL_TARGET := YES
MPROCESS_DEMO_BINARY := mprocess_demo demo_process1 demo_process2
MPROCESS_DEMO_TARGET_BINARY_DIR := /usr/bin

define MPROCESS_DEMO_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D) all
endef

define MPROCESS_DEMO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 1777 $(foreach bin,$(MPROCESS_DEMO_BINARY),$(@D)/$(bin)) $(TARGET_DIR)$(MPROCESS_DEMO_TARGET_BINARY_DIR)
endef

$(eval $(call generic-package))
