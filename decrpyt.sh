#!/bin/sh

DISK_UUID="/dev/disk/by-uuid/5A7D-10C9"

# wait five seconds for the usb to be mounted
sleep 5

if [ -b $DISK_UUID ]; then
    USB_HASH=$(dd if=$DISK_UUID bs=512 skip=1 count=4 2>/dev/null | md5sum | cut -d " " -f1)
    HWID_HASH=$(hwidgen)
    echo "${USB_HASH}${HWID_HASH}"
    exit
fi

/lib/cryptsetup/askpass "Decryption failed. Try with password: "
