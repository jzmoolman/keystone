IMAGE_DIR = $(shell realpath ~/src/keystone-fork/tmp)
$(info $(IMAGE_DIR))
IMAGE_NAME = cva6.img
FWPAYLOAD = fw_payload.bin
FWPAYLOAD_SECTORSTART := 2048
FWPAYLOAD_SECTORSIZE = $(shell ls -l --block-size=512 $(IMAGE_DIR)/$(FWPAYLOAD_NAME) | cut -d " " -f5 )
FWPAYLOAD_SECTOREND = $(shell echo $(FWPAYLOAD_SECTORSTART)+$(FWPAYLOAD_SECTORSIZE) | bc)
SDDEVICE_PART1 = $(shell lsblk $(SDDEVICE) -no PATH | head -2 | tail -1)
SDDEVICE_PART2 = $(shell lsblk $(SDDEVICE) -no PATH | head -3 | tail -1)
# Always flash uImage at 512M, easier for u-boot boot command
UIMAGE_SECTORSTART := 512M
flash-sdcard: format-sd
	dd if=$(RISCV)/fw_payload.bin of=$(SDDEVICE_PART1) status=progress oflag=sync bs=1M
	dd if=$(RISCV)/uImage         of=$(SDDEVICE_PART2) status=progress oflag=sync bs=1M

format-sd: $(SDDEVICE)
	@test -n "$(SDDEVICE)" || (echo 'SDDEVICE must be set, Ex: make flash-sdcard SDDEVICE=/dev/sdc' && exit 1)
	rm -f $IMAGE_DIR/$IMAGE_NAME
	dd if=/dev/zero of=$(IMAGE_DIR)/$(IMAGE_NAME) bs=512 count=204800
	sudo sgdisk --clear -g --new=1:$(FWPAYLOAD_SECTORSTART):$(FWPAYLOAD_SECTOREND) --new=2:$(UIMAGE_SECTORSTART):0 --typecode=1:3000 --typecode=2:8300 $(SDDEVICE)

# specific recipes
