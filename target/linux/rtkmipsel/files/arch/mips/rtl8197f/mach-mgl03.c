 /*
 *  Xiaomi Gateway v3 (MGL03) support
 *
 *  Copyright (C) 2020 Mantas Pucka <mantas@8devices.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <generated/autoconf.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>

#include "bspchip.h"
#include "dev-gpio-buttons.h"
#include "dev_leds_gpio.h"
#include "gpio.h"
#include "machtypes.h"
#include "spi.h"


#define MGL03_BUTTONS_POLL_INTERVAL		100
#define MGL03_BUTTONS_DEBOUNCE_INTERVAL	(3 * MGL03_BUTTONS_POLL_INTERVAL)

static struct gpio_led mgl03_leds_gpio[] __initdata = {
	{
		.name		= "mgl03:green:status",
		.gpio		= BSP_GPIO_PIN_G6,
		.active_low	= 1,
	},

};

static struct gpio_keys_button mgl03_buttons[] __initdata = {
	{
		.desc		= "default",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = MGL03_BUTTONS_DEBOUNCE_INTERVAL,
		.gpio		= BSP_GPIO_PIN_H0,
		.active_low	= 1,
	}
};

struct gpio mgl03_phy_reset_pin_data = {
	.gpio           = BSP_GPIO_PIN_H2,
	.flags          = GPIOF_ACTIVE_LOW,
	.label          = "GPIO_H2",
};

static struct platform_device __initdata *mgl03_devs[] = {
	&rtl819x_phy_reset_pin,
};


#define SET_PINMUX(reg, field, val)\
	REG32(reg) = (REG32(reg) & (~(0xF << field))) | (val << field)

static void mgl03_set_sd_pinmux(void)
{
	SET_PINMUX(BSP_PIN_MUX_SEL15, 28, 0); // MMC_D0
	SET_PINMUX(BSP_PIN_MUX_SEL15, 24, 0); // MMC_D1
	SET_PINMUX(BSP_PIN_MUX_SEL15, 20, 0); // MMC_D2
	SET_PINMUX(BSP_PIN_MUX_SEL15, 16, 0); // MMC_D3
	SET_PINMUX(BSP_PIN_MUX_SEL16,  8, 0); // MMC_CD
	SET_PINMUX(BSP_PIN_MUX_SEL16,  4, 0); // MMC_CLK
	SET_PINMUX(BSP_PIN_MUX_SEL16,  0, 0); // MMC_CMD
}

#ifdef CONFIG_RTL819X_DW_SPI0
static struct spi_board_info dw_spi_devices[] __initdata = {
	{
		.modalias = "m25p80",
		.max_speed_hz = 500000,//48000000,
		.bus_num = 1,
		.chip_select = 0,
		.mode = SPI_MODE_3,
		.irq = -1,
	}
};

struct dw_spi_pdata {
        int num_cs;
        int *cs_gpios;
};

int dw_spi0_cs_gpios[] = {
	24,	//GPIOD[0]
	20,	//GPIOC[4]
};

struct dw_spi_pdata dw_spi0_pdata = {
	.num_cs = 2,
	.cs_gpios = &dw_spi0_cs_gpios,
};

static void mgl03_set_spi_pinmux(void)
{
	int i;

	rtl819x_device_spi0.dev.platform_data= &dw_spi0_pdata;

//	SET_PINMUX(BSP_PIN_MUX_SEL16, 16, 1); // GPIOC[4] SPI0_CS1N
	SET_PINMUX(BSP_PIN_MUX_SEL16, 12, 1); // GPIOC[5] SPI0_TXD
	SET_PINMUX(BSP_PIN_MUX_SEL16,  8, 1); // GPIOC[6] SPI0_RXD
	SET_PINMUX(BSP_PIN_MUX_SEL16,  4, 1); // GPIOC[7] SPI0_CLK
//	SET_PINMUX(BSP_PIN_MUX_SEL16,  0, 1); // GPIOD[0] SPI0_CS0N

	for(i = 0; i < ARRAY_SIZE(dw_spi0_cs_gpios); i++){
		rtl819x_gpio_pin_enable(dw_spi0_cs_gpios[i]);
		rtl819x_gpio_direction_out(dw_spi0_cs_gpios[i], 1);
	}
}
#endif //CONFIG_RTL819X_DW_SPI0

static void __init mgl03_setup(void)
{
	int i;

	rtl819x_phy_reset_pin.dev.platform_data = &mgl03_phy_reset_pin_data;

	platform_add_devices(mgl03_devs, ARRAY_SIZE(mgl03_devs));

	rtl819x_register_leds_gpio(-1, ARRAY_SIZE(mgl03_leds_gpio),
		mgl03_leds_gpio);

	for (i=0; i<ARRAY_SIZE(mgl03_leds_gpio); i++) {
		rtl819x_gpio_pin_enable(mgl03_leds_gpio[i].gpio);
	}

	for (i=0; i<ARRAY_SIZE(mgl03_buttons); i++) {
		rtl819x_gpio_pin_enable(mgl03_buttons[i].gpio);
	}
	rtl819x_add_device_gpio_buttons(-1, MGL03_BUTTONS_POLL_INTERVAL,
				       ARRAY_SIZE(mgl03_buttons),
				       mgl03_buttons);

#ifdef CONFIG_RTL819X_DW_SPI0
	mgl03_set_spi_pinmux();
	spi_register_board_info(dw_spi_devices, ARRAY_SIZE(dw_spi_devices));
#else
	mgl03_set_sd_pinmux();
#endif
}

MIPS_MACHINE(RTL8197_MACH_MGL03, "MGL03", "Xiaomi Gateway v3 (MGL03)",
			 mgl03_setup);
