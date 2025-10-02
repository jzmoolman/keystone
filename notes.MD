Keystone patch qemu to allow for "real rom" simulation, I have taken it out for now

if you really need to load your bootrom.bin before fw_jump.elf, you may need to build QEMU with a patch or embed it in the BIOS flow (e.g. by combining it with OpenSBI).

create pathces or post-extracting hoook
  diff -git
  think the post-extracting hook are better for this 

overlays/keystone/boot/keystone-sm/keystone-sm.mk - reference to hardcode dir
add linux kernal driver for et-rocketchip manually after first build


