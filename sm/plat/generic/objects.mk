# We include the default objects.mk
ifeq ($(PLATFORM),)
        $(error PLATFORM not defined for SM)
endif

$(info ==> sm/plat/generic/objects.mk)
$(info ==> $(src_dir)/platform/$(PLATFORM)/objects.mk)

include $(src_dir)/platform/$(PLATFORM)/objects.mk
# And then also define custom keystone SM functionality
platform-genflags-y += "-DTARGET_PLATFORM_HEADER=\"platform/$(PLATFORM)/platform.h\""
platform-cflags-y = -I$(KEYSTONE_SM)/src -I$(src_dir)/platform/$(PLATFORM)/include \
                        -I$(KEYSTONE_SDK_DIR)/include/shared


$(info ==> $(KEYSTONE_SM)/src/objects.mk)
include $(KEYSTONE_SM)/src/objects.mk
platform-objs-y += $(addprefix ../../src/,$(subst .c,.o,$(keystone-sm-sources)))

ifeq ($(KEYSTONE_PLATFORM),generic)

carray-platform_override_modules-y += generic
platform-objs-y += generic.o

else ifeq ($(KEYSTONE_PLATFORM),hifive_unmatched)

include $(KEYSTONE_SM)/plat/hifive/unmatched/objects.mk

endif

$(info <== sm/plat/generic/objects.mk)
