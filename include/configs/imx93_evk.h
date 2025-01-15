/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2022 NXP
 */

#ifndef __IMX93_EVK_H
#define __IMX93_EVK_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>
#include <version.h>
#include "imx_env.h"

#define CFG_SYS_UBOOT_BASE	\
	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#ifdef CONFIG_AHAB_BOOT
#define AHAB_ENV "sec_boot=yes\0"
#else
#define AHAB_ENV "sec_boot=no\0"
#endif

#ifdef CONFIG_DISTRO_DEFAULTS
#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 0) \
	func(MMC, mmc, 1) \
	func(USB, usb, 0)

#include <config_distro_bootcmd.h>
#else
#define BOOTENV
#endif

#define JH_ROOT_DTB    "imx93-11x11-evk-root.dtb"

#define JAILHOUSE_ENV \
	"jh_root_dtb=" JH_ROOT_DTB "\0" \
	"jh_mmcboot=setenv fdtfile ${jh_root_dtb}; " \
		    "setenv jh_clk clk_ignore_unused mem=1248MB kvm-arm.mode=nvhe; " \
		    "if run loadimage; then run mmcboot;" \
		    "else run jh_netboot; fi; \0" \
	"jh_netboot=setenv fdtfile ${jh_root_dtb}; " \
		    "setenv jh_clk clk_ignore_unused mem=1248MB kvm-arm.mode=nvhe; run netboot; \0 "

#define SR_IR_V2_COMMAND \
	"nodes=/usbg1 /usbg2 /wdt-reboot /rm67199_panel /dsi-host /display-subsystem /soc@0/bus@44000000/dma-controller@44000000 /soc@0/bus@44000000/sai@443b0000 /soc@0/bus@44000000/mqs1 /soc@0/bus@44000000/bbnsm@44440000 /soc@0/bus@44000000/system-controller@44460000 /soc@0/bus@44000000/tmu@44482000 /soc@0/bus@44000000/micfil@44520000 /soc@0/bus@42000000/dma-controller@42000000 /soc@0/bus@44000000/i3c-master@44330000 /soc@0/bus@42000000/i3c-master@42520000 /soc@0/bus@42000000/sai@42650000 /soc@0/bus@42000000/sai@42660000 /soc@0/bus@42000000/mqs2 /soc@0/bus@42000000/xcvr@42680000 /soc@0/bus@42000000/flexio@425c0000 /soc@0/bus@42800000/epxp@4ae20000 /soc@0/bus@42800000/camera /soc@0/efuse@47510000 /soc@0/system-controller@4ac10000 /soc@0/ldb@4ac10020 /soc@0/phy@4ac10024 /soc@0/ele-mu /soc@0/dsi@4ae10000 /soc@0/lcd-controller@4ae30000 /soc@0/blk-ctrl@4e010000 /soc@0/memory-controller@4e300000 /soc@0/bus@44000000/i2c@44350000/pmic@25 /imx93-lpm  \0" \
	"sr_ir_v2_cmd=cp.b ${fdtcontroladdr} ${fdt_addr_r} 0x10000;"\
	"fdt addr ${fdt_addr_r};"\
	"fdt resize 0x400;"\
	"fdt set /soc@0/bus@44000000/i2c@44350000/gpio@34 compatible adi,adp5585;" \
	"for i in ${nodes}; do fdt rm ${i}; done \0"

#define CFG_MFG_ENV_SETTINGS \
	CFG_MFG_ENV_SETTINGS_DEFAULT \
	"initrd_addr=0x83800000\0" \
	"initrd_high=0xffffffffffffffff\0" \
	"emmc_dev=0\0"\
	"sd_dev=1\0" \

/* Initial environment variables */
#define CFG_EXTRA_ENV_SETTINGS		\
	JAILHOUSE_ENV \
	CFG_MFG_ENV_SETTINGS \
	BOOTENV \
	AHAB_ENV \
	SR_IR_V2_COMMAND \
	"prepare_mcore=setenv mcore_clk clk-imx93.mcore_booted;\0" \
	"scriptaddr=0x83500000\0" \
	"kernel_addr_r=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"image=Image\0" \
	"splashimage=0x90000000\0" \
	"console=ttyLP0,115200 earlycon\0" \
	"fdt_addr_r=0x83000000\0"			\
	"fdt_addr=0x83000000\0"			\
	"fdt_high=0xffffffffffffffff\0"		\
	"cntr_addr=0x98000000\0"			\
	"cntr_file=os_cntr_signed.bin\0" \
	"boot_fit=no\0" \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"bootm_size=0x10000000\0" \
	"mmcdev=" __stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=1\0" \
	"mmcroot=/dev/mmcblk1p2 rootwait rw\0" \
	"mmcautodetect=yes\0" \
	"mmcargs=setenv bootargs ${jh_clk} console=${console} root=${mmcroot} " \
        "cur_slot=${bootslot} U-Boot_ver=${u-boot_version}\0"\
	"loadbootscript=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${script};\0" \
	"bootscript=echo Running bootscript from mmc ...; " \
		"source\0" \
	"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	"loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${fdtfile}\0" \
	"loadcntr=fatload mmc ${mmcdev}:${mmcpart} ${cntr_addr} ${cntr_file}\0" \
	"auth_os=auth_cntr ${cntr_addr}\0" \
	"boot_os=booti ${loadaddr} - ${fdt_addr_r};\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if test ${sec_boot} = yes; then " \
			"if run auth_os; then " \
				"run boot_os; " \
			"else " \
				"echo ERR: failed to authenticate; " \
			"fi; " \
		"else " \
			"if test ${boot_fit} = yes || test ${boot_fit} = try; then " \
				"bootm ${loadaddr}; " \
			"else " \
				"if run loadfdt; then " \
					"run boot_os; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi;" \
		"fi;\0" \
	"netargs=setenv bootargs ${jh_clk} ${mcore_clk} console=${console} " \
		"root=/dev/nfs " \
		"ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp\0" \
	"netboot=echo Booting from net ...; " \
		"run netargs;  " \
		"if test ${ip_dyn} = yes; then " \
			"setenv get_cmd dhcp; " \
		"else " \
			"setenv get_cmd tftp; " \
		"fi; " \
		"if test ${sec_boot} = yes; then " \
			"${get_cmd} ${cntr_addr} ${cntr_file}; " \
			"if run auth_os; then " \
				"run boot_os; " \
			"else " \
				"echo ERR: failed to authenticate; " \
			"fi; " \
		"else " \
			"${get_cmd} ${loadaddr} ${image}; " \
			"if test ${boot_fit} = yes || test ${boot_fit} = try; then " \
				"bootm ${loadaddr}; " \
			"else " \
				"if ${get_cmd} ${fdt_addr_r} ${fdtfile}; then " \
					"run boot_os; " \
				"else " \
					"echo WARN: Cannot load the DT; " \
				"fi; " \
			"fi;" \
		"fi;\0" \
	"bootslot=dualA\0" \
	"u-boot_version=null\0" \
	"usb_port=1\0" \
	"post_opt=saveenv;\0" \
	"adjustbootsource=if test ${bootslot} = dualA || test ${bootslot} = singlenormal; then run adjustbootsourceA; fi;" \
        "if test ${bootslot} = dualB; then run adjustbootsourceB; fi\0" \
	"altbootusb=echo Boot Fail! Get into usb fastboot download.;fastboot usb ${usb_port}\0" \
	"altbootsingle=if test ${bootslot} = singlerescue; then run altbootusb; fi; if test ${bootslot} = singlenormal; then run swuboot; fi\0" \
	"adjustbootsourceB=echo RootFs Slot B; setenv mmcpart 3; setenv mmcroot /dev/mmcblk${mmcdev}p4 rootwait rw\0" \
	"adjustbootsourceA=echo RootFs Slot A; setenv mmcpart 1; setenv mmcroot /dev/mmcblk${mmcdev}p2 rootwait rw\0" \
	"altbootRollbackB=echo Rolling back to slot dualB;setenv bootslot dualB;run post_opt;run bootcmd\0" \
	"altbootRollbackA=echo Rolling back to slot dualA;setenv bootslot dualA;run post_opt;run bootcmd\0" \
	"altbootdual=if test ${bootslot} = dualA ; then run altbootRollbackB; fi; if test ${bootslot} = dualB ; then run altbootRollbackA; fi;\0" \
	"swuargs=setenv bootargs console=${console},${baudrate} earlycon=${earlycon},${baudrate} " \
        "cur_slot=${bootslot} U-Boot_ver=${u-boot_version}\0"\
	"loadswuimage=mmc read  ${loadaddr} 0x4000 0x15000\0" \
	"loadswufdt=mmc read  ${fdt_addr} 0x19000 0x200\0" \
	"loadswuramdisk=mmc read  ${initrd_addr} 0x1B000 0x21000\0" \
        "swuboot=echo swuboot ramdisk;run loadswuimage;run loadswufdt;run loadswuramdisk;run swuargs;booti ${loadaddr} ${initrd_addr} ${fdt_addr}\0" \
        "altbootcmd=if test ${bootslot} = singlerescue ||  test ${bootslot} = singlenormal; then run altbootsingle; fi;" \
	"if test ${bootslot} = dualA ||  test ${bootslot} = dualB; then run altbootdual; fi\0" \
	"bsp_bootcmd=echo Running BSP bootcmd ...; " \
	   "if test ${bootslot} = singlerescue;then run swuboot; fi;" \
	   "run adjustbootsource;" \
		"mmc dev ${mmcdev}; if mmc rescan; then " \
		   "if run loadbootscript; then " \
			   "run bootscript; " \
		   "else " \
			   "if test ${sec_boot} = yes; then " \
				   "if run loadcntr; then " \
					   "run mmcboot; " \
				   "else run netboot; " \
				   "fi; " \
			    "else " \
				   "if run loadimage; then " \
					   "run mmcboot; " \
				   "else run netboot; " \
				   "fi; " \
				"fi; " \
		   "fi; " \
	   "fi;"

/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR        0x80000000
#define CFG_SYS_INIT_RAM_SIZE        0x200000

#define CFG_SYS_SDRAM_BASE           0x80000000
#define PHYS_SDRAM                      0x80000000
#define PHYS_SDRAM_SIZE			0x80000000 /* 2GB DDR */

/* Using ULP WDOG for reset */
#define WDOG_BASE_ADDR          WDG3_BASE_ADDR

#if defined(CONFIG_CMD_NET)
#define PHY_ANEG_TIMEOUT 20000
#endif

#ifdef CONFIG_IMX_MATTER_TRUSTY
#define NS_ARCH_ARM64 1
#endif

#ifdef CONFIG_ANDROID_SUPPORT
#include "imx93_evk_android.h"
#endif

#endif
