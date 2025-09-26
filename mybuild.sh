


export LIBSODIUM_T_DIR="/home/jzmoolman/src/keystone/libsodium_builds/libsodium/src/libsodium"

rm -rf build-rocket64/buildroot.build/build/keystone*
rm -rf build-rocket64/buildroot.build/build/host-keystone*

KEYSTONE_PLATFORM=rocket make

./mk-sd-image


sudo mount -t fuse.vmhgfs-fuse .host:/ /mnt/hgfs -o allow_other
IMAGE_DIR=$(realpath ./build-rocket64/buildroot.build/images)
cp $IMAGE_DIR/debian-riscv64.sd2.img /mnt/hgfs/zach/src/
