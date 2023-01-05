###########################################################
# platform definition
###########################################################
ROOT      =.

LOCAL_CFLAGS += ${TRUSTEDCORE_LOCAL_CFLAGS}
LOCAL_ASFLAGS += ${TRUSTEDCORE_LOCAL_ASFLAGS}

LOCAL_CFLAGS += -O2 -fstrict-aliasing -funswitch-loops -fno-short-enums -W -Wall -mfloat-abi=softfp -mfpu=neon -fno-omit-frame-pointer -fstack-protector-all -Wstack-protector -MD

ifeq ($(strip $(CFG_HI_TEE_WERROR_SUPPORT)), y)
LOCAL_CFLAGS += -Werror -Wall
endif

LOCAL_C_INCLUDES += \
    -I$(TRUSTEDCORE_DIR)/include/TEE \
    -I$(TRUSTEDCORE_DIR)/include/kernel \
    -I$(TRUSTEDCORE_DIR)/TEE_ext \
    -I$(TRUSTEDCORE_DIR)/platform/init \
    -I$(TRUSTEDCORE_DIR)/platform/${PLATFORM_DIR}/${PLATFORM_DIR_NAME}/ext/user \
    -I$(HI_TEE_API_DIR)/ta_al/trustedcore \
    -I$(HI_TEE_API_DIR)/include

ELF_MAIN_ENTRY_PATH = $(TRUSTEDCORE_DIR)/release/internal_tasks/elf_main_entry.o

ALL_OBJ = $(LOCAL_SRC_FILES)
LOCAL_OUT = $(OUT)/internal_tasks/$(notdir $(CURDIR))
ALL_OUT_OBJ = $(foreach VAR,$(ALL_OBJ),$(addprefix $(LOCAL_OUT)/,$(VAR)))

CFLAGS += $(LOCAL_CFLAGS) $(LOCAL_C_INCLUDES)
LDFLAGS += -r -d --strip-debug --whole-archive -T $(TA_LINK_SCRIPT) $(ELF_MAIN_ENTRY_PATH) --no-whole-archive
MANIFEST_FILE := $(ROOT)/manifest.txt

##########################################################
# build
##########################################################
.PHONY : all prepare clean
ifeq ($(strip $(CONFIG_TRUSTEDCORE_RELEASE)), )
all: $(ALL_OBJ)
	@echo $(ALL_OBJ)
	$(LD1) $(LDFLAGS) -o $(LOCAL_MODULE) $(ALL_OUT_OBJ) `$(CC1) -mthumb-interwork -print-libgcc-file-name`
	test ! -f $(MANIFEST_FILE) || $(TRUSTEDCORE_PACK_TA_SCRIPT) $(LOCAL_MODULE) $(MANIFEST_FILE)
	@echo "Make $(LOCAL_MODULE) Success!"
else
all:
	cp $(TRUSTEDCORE_RELEASE_DIR)/$(HI_CHIP_TYPE)/$(CFG_HI_TVP_MEM_LAYOUT)/$(notdir $(LOCAL_MODULE)) $(LOCAL_MODULE)
	@echo "Make $(LOCAL_MODULE) Success!"
endif
$(ALL_OBJ):prepare

prepare:
	$(MKDIR) $(foreach VAR,$(ALL_OUT_OBJ),$(dir $(VAR)))

clean:
	$(RM) $(LOCAL_MODULE) $(ALL_OUT_OBJ) $(ALL_OBJ)

%.o : %.c | prepare
	$(CC1) -c $(CFLAGS) $< -o $(LOCAL_OUT)/$@

%.o : %.S | prepare
	$(CC1) -c $(CFLAGS) $< -o $(LOCAL_OUT)/$@

%.o : %.s | prepare
	$(CC1) -c $(CFLAGS) $< -o $(LOCAL_OUT)/$@

