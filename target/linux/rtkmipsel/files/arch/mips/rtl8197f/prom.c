/*
 * Realtek Semiconductor Corp.
 *
 * bsp/prom.c
 *     bsp early initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <asm/addrspace.h>
#include <asm/bootinfo.h>

#include "bspcpu.h"
#include "bspchip.h"

#define PARAM_ADDR      0x81f00000

extern char arcs_cmdline[];

#ifdef CONFIG_EARLY_PRINTK
static int promcons_output __initdata = 0;

void unregister_prom_console(void)
{
	if (promcons_output)
		promcons_output = 0;
}

void disable_early_printk(void)
    __attribute__ ((alias("unregister_prom_console")));

void prom_putchar(char c)
{
	unsigned int busy_cnt = 0;

	do
	{
		/* Prevent Hanging */
		if (busy_cnt++ >= 30000)
		{
			/* Reset Tx FIFO */
			REG8(BSP_UART0_FCR) = BSP_TXRST | BSP_CHAR_TRIGGER_14;
			return;
		}
	} while ((REG8(BSP_UART0_LSR) & BSP_LSR_THRE) == BSP_TxCHAR_AVAIL);

	/* Send Character */
	REG8(BSP_UART0_THR) = c;
	return;
}

static int bsp_serial_init(void)
{
	REG32(BSP_UART0_IER) = 0;

	REG32(BSP_UART0_LCR) = BSP_LCR_DLAB;
	REG32(BSP_UART0_DLL) = BSP_UART0_BAUD_DIVISOR & 0x00ff;
	REG32(BSP_UART0_DLM) = (BSP_UART0_BAUD_DIVISOR & 0xff00) >> 8;
//REG32(BSP_UART0_SCR) = 0xA0030; // 57600, 115200
//REG32(BSP_UART0_STSR) = 0xC0; // 57600, 115200
	REG32(BSP_UART0_LCR) = BSP_CHAR_LEN_8;
	return 0;
}
#endif

const char *get_system_type(void)
{
	return "RTL8197F";
}

//void __init bsp_free_prom_memory(void)
void __init prom_free_prom_memory(void) //mips-ori
{
}

static __init void prom_init_cmdline(void)
{
  char *bl_cmdline = (char *)PARAM_ADDR;

  //If parameters passing from bootloader
	if (strncmp(bl_cmdline,"board=",6) == 0) {
    strcpy(arcs_cmdline, bl_cmdline);
  //If on stock firmware
  } else if (strncmp(bl_cmdline,"root=",5) == 0) {
    strcpy(arcs_cmdline, "board=MGL03 "
                         "mtdparts=rtk_nand:640k(bootloader),"
                                            "128k(boot_info),"
                                              "128k(factory),"
                                             "128k(mtd_oops),"
                                                    "1M(bbt),"
                                                "3M(linux_1),"
                                              "25M(rootfs_1),"
                                                "3M(linux_2),"
                                              "25M(rootfs_2),"
                                                "1M(homekit),"
                                                "57472k(ubi) ");
    strcat(arcs_cmdline, bl_cmdline);
  //Hardcode OpenWRT NAND default
  } else {
    strcpy(arcs_cmdline, "board=MGL03 console=ttyS0,38400 "
                         "hwpart=0x80000 mtdparts=rtk_nand:512k(boot),"
                                                        "256k(hwpart),"
                                                           "128k(cfg),"
                                                          "128k(oops),"
                                                             "1M(bbt),"
                                                           "2M(linux),"
                                                         "10M(rootfs),"
                                                          "100M(ubi)");
  };
}

/* Do basic initialization */
void __init prom_init(void) // mips-ori
{
	u_long mem_size;
	bsp_serial_init(); // for debug
	prom_init_cmdline();
	switch (REG32(0xB800000C) & 0x0F) {
		case 0x06:
		case 0x0C:
			mem_size =  (32 << 20);
			break;
		case 0x04:
		case 0x0A:
			mem_size =  (64 << 20);
			break;
		case 0x05:
		case 0x0B:
			mem_size =  (128 << 20);
			break;
		default:
			//mem_size = cpu_mem_size;
			mem_size = (REG32(0xB8000F00) << 20);
	}

#ifdef  CONFIG_RTL8198C_OVER_256MB
	mem_size = (256 << 20);
	add_memory_region(0, mem_size, BOOT_MEM_RAM);
	add_memory_region(0x30000000, 256 * 1024 * 1024, BOOT_MEM_RAM);

#else
	//printk("mem_size=%d MB\n", mem_size>>20);
	add_memory_region(0, mem_size, BOOT_MEM_RAM);
#endif
}
