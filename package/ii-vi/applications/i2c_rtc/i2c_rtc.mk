################################################################################
#
# i2c_rtc
#
################################################################################

SRC_DIR = i2c_rtc

I2C_RTC_VERSION := 1.0.0
I2C_RTC_SITE := package/ii-vi/applications/$(SRC_DIR)
I2C_RTC_SITE_METHOD := local
I2C_RTC_INSTALL_TARGET := YES
I2C_RTC_BINARY := i2c_rtc
I2C_RTC_TARGET_BINARY := /usr/bin/$(I2C_RTC_BINARY)

define I2C_RTC_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D) all
endef

define I2C_RTC_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 1777 $(@D)/$(I2C_RTC_BINARY) $(TARGET_DIR)$(I2C_RTC_TARGET_BINARY)
endef

$(eval $(call generic-package))
