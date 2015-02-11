################################################################################
#
# hello_world_mod
#
################################################################################

SRC_DIR = hello_world_mod

HELLO_WORLD_MOD_VERSION := 1.0.0
HELLO_WORLD_MOD_SITE := package/ii-vi/modules/$(SRC_DIR)
HELLO_WORLD_MOD_SITE_METHOD := local
HELLO_WORLD_MOD_INSTALL_TARGET := YES

HELLO_WORLD_MOD_DEPENDENCIES = linux

define HELLO_WORLD_MOD_BUILD_CMDS
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(ARCH) CC=$(TARGET_CC) LD=$(TARGET_LD) M=$(@D) modules
endef

define HELLO_WORLD_MOD_INSTALL_TARGET_CMDS
	$(MAKE) -C $(KERNEL_DIR) M=$(@D) \
		INSTALL_MOD_PATH=$(TARGET_DIR) INSTALL_MOD_STRIP=1 \
		INSTALL_MOD_DIR=$(MOD_DIR) \
		modules_install
endef

$(eval $(call generic-package))
