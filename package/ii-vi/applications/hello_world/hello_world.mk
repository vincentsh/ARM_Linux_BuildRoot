################################################################################
#
# hello_world
#
################################################################################

SRC_DIR = hello_world

HELLO_WORLD_VERSION := 1.0.0
HELLO_WORLD_SITE := package/ii-vi/applications/$(SRC_DIR)
HELLO_WORLD_SITE_METHOD := local
HELLO_WORLD_INSTALL_TARGET := YES
HELLO_WORLD_BINARY := hello_world
HELLO_WORLD_TARGET_BINARY := /usr/bin/$(HELLO_WORLD_BINARY)

define HELLO_WORLD_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D) all
endef

define HELLO_WORLD_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 1777 $(@D)/$(HELLO_WORLD_BINARY) $(TARGET_DIR)$(HELLO_WORLD_TARGET_BINARY)
endef

$(eval $(call generic-package))
