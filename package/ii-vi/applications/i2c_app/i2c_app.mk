################################################################################
#
# i2c_app
#
################################################################################

SRC_DIR = i2c_app

I2C_APP_VERSION := 1.0.0
I2C_APP_SITE := package/ii-vi/applications/$(SRC_DIR)
I2C_APP_SITE_METHOD := local
I2C_APP_INSTALL_TARGET := YES
I2C_APP_BINARY := i2c_app
I2C_APP_TARGET_BINARY := /usr/bin/$(I2C_APP_BINARY)

define I2C_APP_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D) all
endef

define I2C_APP_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 1777 $(@D)/$(I2C_APP_BINARY) $(TARGET_DIR)$(I2C_APP_TARGET_BINARY)
endef

$(eval $(call generic-package))
