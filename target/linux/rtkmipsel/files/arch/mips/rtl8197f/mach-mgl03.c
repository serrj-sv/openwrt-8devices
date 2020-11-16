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

#include "bspchip.h"
#include "dev-gpio-buttons.h"
#include "dev_leds_gpio.h"
#include "gpio.h"
#include "machtypes.h"

#define MGL03_BUTTONS_POLL_INTERVAL		100
#define MGL03_BUTTONS_DEBOUNCE_INTERVAL	(3 * MGL03_BUTTONS_POLL_INTERVAL)

static struct gpio_led mgl03_leds_gpio[] __initdata = {
	{
		.name		= "mgl03:rgb_blue",
		.gpio		= BSP_GPIO_PIN_G6,
		.active_low	= 0,
	},
	{
		.name		= "mgl03:rgb_green",
		.gpio		= BSP_GPIO_PIN_H0,
		.active_low	= 0,
	},
	{
		.name		= "mgl03:rgb_red",
		.gpio		= BSP_GPIO_PIN_H1,
		.active_low	= 0,
	}
};

static struct gpio_keys_button mgl03_buttons[] __initdata = {
	{
		.desc		= "default",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = MGL03_BUTTONS_DEBOUNCE_INTERVAL,
		.gpio		= BSP_GPIO_PIN_A7,
		.active_low	= 0,
	}
};

static struct platform_device __initdata *mgl03_devs[] = {
	&rtl819x_phy_reset_pin,
};



static void __init mgl03_setup(void)
{
	int i;

	platform_add_devices(mgl03_devs, ARRAY_SIZE(mgl03_devs));

	rtl819x_register_leds_gpio(-1, ARRAY_SIZE(mgl03_leds_gpio),mgl03_leds_gpio);

	for (i=0; i<ARRAY_SIZE(mgl03_leds_gpio); i++) {
		rtl819x_gpio_pin_enable(mgl03_leds_gpio[i].gpio);
	}

	for (i=0; i<ARRAY_SIZE(mgl03_buttons); i++) {
		rtl819x_gpio_pin_enable(mgl03_buttons[i].gpio);
	}

	rtl819x_add_device_gpio_buttons(-1, MGL03_BUTTONS_POLL_INTERVAL,
				       ARRAY_SIZE(mgl03_buttons),
				       mgl03_buttons);

	rtl819x_gpio_pin_enable(BSP_GPIO_PIN_E0);
}

MIPS_MACHINE(RTL8197_MACH_MGL03, "MGL03", "Xiaomi Gateway v3 (MGL03)",
			 mgl03_setup);
