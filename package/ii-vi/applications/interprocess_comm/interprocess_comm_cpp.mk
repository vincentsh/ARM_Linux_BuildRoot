################################################################################
#
# interprocess_comm
#
################################################################################

SRC_DIR = interprocess_comm

INTERPROCESS_COMM_VERSION := 1.0.0
INTERPROCESS_COMM_SITE := package/ii-vi/applications/$(SRC_DIR)
INTERPROCESS_COMM_SITE_METHOD := local
INTERPROCESS_COMM_INSTALL_TARGET := YES
INTERPROCESS_COMM_BINARY := ipc_demo
INTERPROCESS_COMM_TARGET_BINARY_DIR := /usr/bin/

define INTERPROCESS_COMM_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) CXX=$(TARGET_CXX) LD=$(TARGET_LD) -C $(@D) all
endef

define INTERPROCESS_COMM_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 1777 $(foreach bin,$(INTERPROCESS_COMM_BINARY),$(@D)/$(bin)) $(TARGET_DIR)$(INTERPROCESS_COMM_TARGET_BINARY_DIR)
endef

$(eval $(call generic-package))
