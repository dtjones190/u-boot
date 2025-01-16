/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2022 NXP
 */

#ifndef __IMX93_NITROGEN_SMARC_H
#define __IMX93_NITROGEN_SMARC_H

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

#define CFG_MFG_ENV_SETTINGS \
	CFG_MFG_ENV_SETTINGS_DEFAULT \
	"initrd_addr=0x83800000\0" \
	"initrd_high=0xffffffffffffffff\0" \
	"emmc_dev=0\0"\
	"sd_dev=1\0" \

#define NITROGEN_SMARC_DEFCONFIG	"imx93_nitrogen_smarc"

/* Initial environment variables */
#define CFG_EXTRA_ENV_SETTINGS		\
	CFG_MFG_ENV_SETTINGS \
	BOOTENV \
	AHAB_ENV \
	"board=nitrogen-smarc\0" \
	"soc_type=imx93\0" \
	"env_dev=" __stringify(CONFIG_SYS_MMC_ENV_DEV) "\0" \
	"env_part=" __stringify(CONFIG_SYS_MMC_ENV_PART) "\0" \
	"fastboot_raw_partition_bootloader=0x0 0x1ff0 mmcpart 1\0" \
	"fastboot_raw_partition_bootloader-env=0x1ff0 0x10 mmcpart 1\0" \
	"mcoreboot=load ${devtype} ${devnum}:1 ${mcore_loadaddr} ${mcore_image}; " \
		"dcache flush; bootaux " __stringify(TCML_BASE_MCORE_SEC_ADDR)"\0" \
	"mcore_image=m33_fw.bin\0" \
	"mcore_loadaddr=" __stringify(CONFIG_IMX_MCORE_TCM_ADDR) "\0" \
	"mcore_bootargs=clk-imx93.mcore_booted\0" \
	"scriptaddr=0x83500000\0" \
	"console=ttyLP0\0" \
	"fdt_addr=0x83000000\0"			\
	"fdt_high=0xffffffffffffffff\0"		\
	"fdt_file=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"mmcargs=setenv bootargs ${jh_clk} console=${console} root=${mmcroot} " \
        "cur_slot=${bootslot} U-Boot_ver=${u-boot_version}\0"\
	"netargs=setenv bootargs console=${console},115200 root=/dev/nfs rw " \
		"ip=dhcp nfsroot=${tftpserverip}:${nfsroot},v3,tcp\0" \
	"netboot=echo Booting from net ...; " \
		"run netargs;  " \
		"if test ${ip_dyn} = yes; then " \
			"setenv get_cmd dhcp; " \
		"else " \
			"setenv get_cmd tftp; " \
		"fi; " \
		"${get_cmd} ${loadaddr} ${tftpserverip}:Image; " \
		"if ${get_cmd} ${fdt_addr} ${tftpserverip}:${fdt_file}; then " \
			"booti ${loadaddr} - ${fdt_addr}; " \
		"else " \
			"echo WARN: Cannot load the DT; " \
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
	"upgradeu=setenv boot_scripts upgrade.scr; boot;" \
                "echo Upgrade failed!; setenv boot_scripts boot.scr;\0" \
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
	   "fi;" \
	"uboot_defconfig=" NITROGEN_SMARC_DEFCONFIG "\0"
/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR        0x80000000
#define CFG_SYS_INIT_RAM_SIZE        0x200000

#define CFG_SYS_SDRAM_BASE           0x80000000
#define PHYS_SDRAM                      0x80000000
#define PHYS_SDRAM_SIZE			0x80000000 /* 2GB DDR */

#define CFG_SYS_FSL_USDHC_NUM	2

/* Using ULP WDOG for reset */
#define WDOG_BASE_ADDR          WDG3_BASE_ADDR

#if defined(CONFIG_CMD_NET)
#define PHY_ANEG_TIMEOUT 20000
#endif

#ifdef CONFIG_IMX_MATTER_TRUSTY
#define NS_ARCH_ARM64 1
#endif

#endif
