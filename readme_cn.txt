a.进入uboot源代码目录后，执行以下操作:
    cp configs/ss928v100_defconfig .config(仅用于 spi nor/nand flash)
    (emmc启动时执行如下操作: cp configs/ss928v100_emmc_defconfig .config)
    (并口 nand 启动时执行如下操作：cp configs/ss928v100_nand_defconfig .config)
    make ARCH=arm CROSS_COMPILE=aarch64-mix210-linux- menuconfig
    make ARCH=arm CROSS_COMPILE=aarch64-mix210-linux- -j 20

    1.linux
        cd boot/uboot_tools/
        tar zxf regbin-v1.0.2.tgz
        cd regbin-v1.0.2/
        chmod +x regbin
        ./regbin ../SS928V100_demo_8L_LPDDR4x_3733M-8GB_32bitx2-A55_1400M-emmc.xlsm ../reg_info.bin
        cd ../../../
    2.windows
        Windows下进入到boot/uboot_tools/目录下打开对应单板的Excel文件,在main标签中点击"Generate reg bin file"按钮,生成reg_info.bin即为对应平台的表格文件。
    
    从boot/uboot_tools目录拷贝reg_info.bin到boot源代码目录,重命名为.reg
    cp boot/uboot_tools/reg_info.bin .reg

    make ARCH=arm CROSS_COMPILE=aarch64-mix210-linux- u-boot-z.bin

    生成的u-boot-ss928v100.bin即为“快速启动”使用的u-boot镜像

b.对于“非安全启动”方案，还需要进入boot目录，执行以下操作来制作 Boot Image：
    cd boot
    tar xf boot.tar.gz
    cd boot/gsl/
    将cfg.mk的第7行改为TEXT_BASE = $(shell printf "%#x" $$(($(RAM_BASE) + $(TEXT_OFST))))
    make CHIP=ss928v100
    cd ../image_map/
    cp ../gsl/pub/gsl.bin ./
    cp ../../../u-boot-ss928v100.bin ./u-boot-original.bin 
    cp ../../../.reg ./
	python3 oem/oem_quick_build.py
	image/oem/下生成的boot_image.bin即为“非安全启动”使用的Boot Image。