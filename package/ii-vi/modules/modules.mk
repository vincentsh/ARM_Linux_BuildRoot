################################################################################
#
# ii-vi modules
#
################################################################################

export KERNEL_DIR=$(BASE_DIR)/build/linux-*
export MOD_DIR = user_modules

include package/ii-vi/modules/*/*.mk
