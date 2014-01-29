OpenWrt-transfer client
================

This client is meant to be run on an OpenWrt device, and should 
be build with an appropriate SDK. The SDKs can be found in the 
[downloads directory of OpenWrt](http://downloads.openwrt.org/).

To compile, simply download and unpack the SDK, make a symbolic link
to this client folder under the SDK ```package``` dir 
(so the path to the client is: `path-to-SDK/package/client`), go to
the root of the SDK and hit run.

If something fails please refer to this brilliant tutorial on how
to compile programs for OpenWrt: 
http://manoftoday.wordpress.com/2007/10/11/writing-and-compiling-a-simple-program-for-openwrt/