################################################################################
#
# led_app
#
################################################################################

SRC_DIR = led_app

LED_APP_VERSION := 1.0.0
LED_APP_SITE := package/ii-vi/applications/$(SRC_DIR)
LED_APP_SITE_METHOD := local
LED_APP_INSTALL_TARGET := YES
LED_APP_BINARY := led_app
LED_APP_TARGET_BINARY := /usr/bin/$(LED_APP_BINARY)

define LED_APP_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D) all
endef

define LED_APP_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 1777 $(@D)/$(LED_APP_BINARY) $(TARGET_DIR)$(LED_APP_TARGET_BINARY)
endef

$(eval $(call generic-package))
