Introduction
-------------
This application will create mvmdio-uio.ko kernel module that needs to
be inserted from userspace to access mdio bus from userspace

Compilation
------------
$ make make ARCH=arm64 CROSS_COMPILE=aarch64-marvell-linux-gnu- KDIR=<kernel_source>

DTS Modification
-----------------

On CN9130/Armada platforms:
/ {
        cpn-110-master {
                config-space {
                        mdio: mdio@12a200 {
                                #address-cells = <1>;
                                #size-cells = <0>;
                        };
                        switch {
                                compatible = "marvell,mvmdio-uio";
                                mii-bus = <&mdio>;
                        };
		};
	};
};

On CN83xx platforms:

&mrml_bridge {
	mdio-nexus@1,3 {
		mdio0: mdio0@87e005003800{
			...
		};
		mdio1: mdio1@87e005003880{
			...
		};
		mdio0_uio {
			compatible = "marvell,mvmdio-uio";
			mii-bus = <&mdio0>;
		};
		mdio1_uio {
			compatible = "marvell,mvmdio-uio";
			mii-bus = <&mdio1>;
		};
	};
};

On Target Board
----------------
1. Use Image and dtb file compiled with above modification to boot the board.
2. mvmdio_uio_sample program can be used to access mdio bus:
Usage:
        read claus 22: ./mvmdio_uio_sample 22 <bus> <phy> <reg>
        write claus 22: ./mvmdio_uio_sample 22 <bus> <phy> <reg> <data>
        read claus 45: ./mvmdio_uio_sample 45 <bus <phy> <device <reg>
        write claus 45: ./mvmdio_uio_sample 45 <bus <phy> <device> <reg> <data>
