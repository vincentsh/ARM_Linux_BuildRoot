################################################################################
#
# test_script
#
################################################################################

SRC_DIR = test_script

TEST_SCRIPT_VERSION := 1.0.0
TEST_SCRIPT_SITE := package/ii-vi/applications/$(SRC_DIR)
TEST_SCRIPT_SITE_METHOD := local
TEST_SCRIPT_INSTALL_TARGET := YES
TEST_SCRIPT_BINARY := test_script
TEST_SCRIPT_TARGET_BINARY := /usr/bin/$(TEST_SCRIPT_BINARY)

define TEST_SCRIPT_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(@D) all
endef

define TEST_SCRIPT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 1777 $(@D)/$(TEST_SCRIPT_BINARY) $(TARGET_DIR)$(TEST_SCRIPT_TARGET_BINARY)
endef

$(eval $(call generic-package))
