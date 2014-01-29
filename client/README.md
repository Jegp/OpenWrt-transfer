OpenWrt-transfer client
================

This client is meant to be run on an OpenWrt device, and should 
be build with an appropriate SDK. The SDKs can be found in the 
[downloads directory of OpenWrt](http://downloads.openwrt.org/). 
I am currently on ```Attitude Adjustment``` v. 12.09 with an ar71xx device, found in 
`attitude_adjustment/12.09/ar71xx/generic/OpenWrt-SDK-ar71xx-for-linux-i486-gcc-4.6-linaro_uClibc-0.9.33.2.tar.bz2`.

To compile, simply download and unpack the SDK, make a symbolic link
to this client folder under the SDK ```package``` dir 
(so the path to the client is: `path-to-SDK/package/client`), go to
the root of the SDK and hit `make`.

If something fails please refer to this brilliant tutorial on how
to compile programs for OpenWrt: 
http://manoftoday.wordpress.com/2007/10/11/writing-and-compiling-a-simple-program-for-openwrt/