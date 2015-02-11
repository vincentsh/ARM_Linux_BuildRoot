################################################################################
#
# mthread_demo
#
################################################################################

SRC_DIR = mthread_demo

MTHREAD_DEMO_VERSION := 1.0.0
MTHREAD_DEMO_SITE := package/ii-vi/applications/$(SRC_DIR)
MTHREAD_DEMO_SITE_METHOD := local
MTHREAD_DEMO_INSTALL_TARGET := YES
MTHREAD_DEMO_BINARY := mthread_demo
MTHREAD_DEMO_TARGET_BINARY := /usr/bin/$(MTHREAD_DEMO_BINARY)

define MTHREAD_DEMO_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D) all
endef

define MTHREAD_DEMO_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 1777 $(@D)/$(MTHREAD_DEMO_BINARY) $(TARGET_DIR)$(MTHREAD_DEMO_TARGET_BINARY)
endef

$(eval $(call generic-package))
