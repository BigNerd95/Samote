# Samote 1.0 (BETA)
Porting of Samsung Network Remote Control Protocol for 2009 TVs (needs custom firmware like SamyGO)

With this program you can control 2009 Samsung TVs with Apps designed for newer models of Samsung TVs.

## Preamble

This is a BETA version, so there are some missing parts (like xml response server and control on frame length) and some missing functions (like static ip, now it only works with auto discovery).

#### Auto detect only (for now)
It does not work with static ip (in smartphone App) because the App requests an xml on port 52235, where there is another service listening, so I've to implement a rawsocket server to response the correct resource (still not had the time).

##### Workaround
With auto detect I can specify the port where to request the xml, so forcing it to another port, I can response the correct xml with a webserver (httpd from busybox).

#### Key Codes compatibility
If some commands doesn't work, you can modify the file key_codes.conf

To discover the commands of the remote App you can use my python utils (/uitls/ folder of this project)

---

## Usage 

1. Cross compile with a toolchain from SamyGO or use my precompiled binaries
2. Copy the content of /bin/ folder inside /mtd_rwarea/Samote/
3. Download busybox for ARM from (https://busybox.net/downloads/binaries/latest/) and put it in /mtd_rwarea/
4. Run busybox: `/mtd_rwarea/busybox-armv7l httpd -h /mtd_rwarea/Samote/upnp/`
5. Run Samote: `/mtd_rwarea/injectso $(pidof exeDSP) /mtd_rwarea/Samote/loader.so Game_Main /mtd_rwarea/Samote/`

