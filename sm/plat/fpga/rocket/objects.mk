
#
# SPDX-License-Identifier: BSD-2-Clause
#

# vivado
PLATFORM_RISCV_XLEN=64

# vivado && opensbi/platform/ariane
FW_TEXT_START=0x80000000

# vivado
FW_DYNAMIC=n
FW_JUMP=n


# Firmware with payload configuration.
FW_PAYLOAD=y

ifeq ($(PLATFORM_RISCV_XLEN), 32)
  # This needs to be 4MB aligned for 32-bit support
  FW_PAYLOAD_OFFSET=0x400000
else
  # This needs to be 2MB aligned for 64-bit support
  FW_PAYLOAD_OFFSET=0x200000
endif


#FW_PAYLOAD_FDT_ADDR=0x82200000
FW_PAYLOAD_ALIGN=0x1000

ifeq ($(KEYSTONE_SM),)
$(error KEYSTONE_SM not defined for SM)
endif

ifeq ($(KEYSTONE_SDK_DIR),)
$(error KEYSTONE_SDK_DIR not defined)
endif
$(info echo src_dir=$(echo src_dir))
platform-cflags-y = -I$(KEYSTONE_SM)/src -I$(src_dir)/platform/$(PLATFORM)/include \
                        -I$(KEYSTONE_SDK_DIR)/include/shared


# opensbi platform
platform-objs-y += platform.o

$(info zzzzzz "PLATFORM$(PLATFORM)")

platform-genflags-y += "-DTARGET_PLATFORM_HEADER=\"platform/$(PLATFORM)/platform.h\""

include $(KEYSTONE_SM)/src/objects.mk

$(info zzzzzz "keystone-sm-sources=$(keystone-sm-sources)")
platform-objs-y += $(addprefix ../../../src/,$(subst .c,.o,$(keystone-sm-sources)))

carray-platform_override_modules-y += platform




