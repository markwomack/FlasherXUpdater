#!/usr/bin/bash

# This script will take a copy of a file, prepend the
# original file size and crc to it, and then send the
# modified file contents to a USB port.
#
# Example of usage:
#   ./usbSend.sh /dev/ttyACM0 somefilename
#                ------------ ------------
#                      ^           ^ file contents to send over usb
#                      | target usb port to send content to
#

# modified file that will be sent
f=$(echo $2.usb)

# copy the original file to/over the modified file
cp $2 $f

# get size of file
s=$(echo $(stat -c "%s" $2)$(echo "!"))

# get crc of file
crc=$(echo $(crc32 $2)$(echo "!"))

# prepend the size and crc to the file
sed -i "1s/^/$s$crc/" $f

# send the modified file to the usb port
cat $f>$1
