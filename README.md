## 1. SDK 依赖（Ubuntu 18.04）
	apt-get install gcc make gettext zlib1g-dev libncurses5-dev bison flex bc texinfo tree cmake flex

## 2. 配置内核
	cd $sdk_path/source/kernel/linux-4.4.y
	make ARCH=arm hi3798mv100_defconfig
	make ARCH=arm menuconfig
	make ARCH=arm savedefconfig
	cp defconfig arch/arm/configs/hi3798mv100_defconfig
	make distclean

## 3. 编译 SDK
	cd HiSTBLinuxV100R005C00SPC050
	source env.sh
	cp configs/$chip_type/xx_cfg.mak cfg.mak
 	make menuconfig
	make build -j 8

## 4. 自定义 bootargs

	4.1 The default bootargs.bin will be generate in $out_dir/image/ according to the bootargs_xx.txt .
	    bootargs_xx.txt is in $sdk_path/configs/$chip_type/prebuilts (this path may change in future ,customer can execute "find . -name bootargs* " in $sdk_path/configs to search it) 
	    
	4.2 For bootargs customize, customer can directly modify the bootargs_xx.txt ,then use the command "mkbootargs" to generate the bootargs.bin. below gives a demonstration:
	    mkbootargs  -s 128k -r bootargs.txt  -o bootargs.bin
	    
	4.3 Customer also can set bootargs directly in boot console. 
	    such as(the parameter should be modified by customer):
	    setenv bootcmd 'nand read 1000000 c00000 800000;bootm 1000000'
      	    setenv bootargs 'mem=1G console=ttyAMA0,115200 root=/dev/mtdblock5 rootfstype=yaffs2 mtdparts=hi_sfc:1M(boot);hinand:4M(baseparam),4M(pqparam),4M(logo),8M(kernel),96M(rootfs),-(others) mmz=ddr,0,0,300M'
      	    saveenv
      	4.4 For more detail ,please refer to <Linux Development Environment User Guide>.
