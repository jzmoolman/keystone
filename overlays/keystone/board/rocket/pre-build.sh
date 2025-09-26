#!/bin/sh

echo "executing prebuild.sh"
echo "$BR2_EXTERNAL_KEYSTONE_PATH $BUILDROOT_OVERLAYDIR"
cp -rf $BR2_EXTERNAL_KEYSTONE_PATH/board/rocket/rocket-sdk/rootfs/* $BUILDROOT_OVERLAYDIR/ | true
echo "executing done"
# Install udev rules & systemd units
