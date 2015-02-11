################################################################################
#
# bbb_led_mod
#
################################################################################

SRC_DIR = bbb_led_mod

BBB_LED_MOD_VERSION := 1.0.0
BBB_LED_MOD_SITE := package/ii-vi/modules/$(SRC_DIR)
BBB_LED_MOD_SITE_METHOD := local
BBB_LED_MOD_INSTALL_TARGET := YES

BBB_LED_MOD_DEPENDENCIES = linux

define BBB_LED_MOD_BUILD_CMDS
	$(MAKE) -C $(KERNEL_DIR) ARCH=$(ARCH) CC=$(TARGET_CC) LD=$(TARGET_LD) M=$(@D) modules
endef

define BBB_LED_MOD_INSTALL_TARGET_CMDS
	$(MAKE) -C $(KERNEL_DIR) M=$(@D) \
		STRIP=$(TARGET_STRIP) \
		INSTALL_MOD_PATH=$(TARGET_DIR) INSTALL_MOD_STRIP=1 \
		INSTALL_MOD_DIR=$(MOD_DIR) \
		modules_install
endef

$(eval $(call generic-package))
