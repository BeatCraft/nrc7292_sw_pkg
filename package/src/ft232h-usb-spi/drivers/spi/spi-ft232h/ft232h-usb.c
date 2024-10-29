/* SPDX-License-Identifier: GPL-2.0 */
/*
 * FTDI FT232H USB-SPI bridge driver
 *
 * Copyright (C) Newracom, Inc. <www.newracom.com>
 *
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/driver.h>
#include <linux/spi/spi.h>
#include <linux/usb/ch9.h>
#include <linux/usb.h>

#include "ft232h-intf.h"


#define VID_FTDI	0x0403
//#define PID_FT232H	0x6014
// BC: 2024/10/08
// for bc-11ah-m2usb_spi 
#define PID_FT232H	0x6010

static bool ft232h_probed = false;
static int ft232h_diconnected = 0;

extern int ft232h_intf_probe(struct usb_interface *intf, const struct usb_device_id *id);
extern void ft232h_intf_disconnect(struct usb_interface *intf);

extern int ft232h_spi_probe(struct platform_device *pdev);
extern int ft232h_spi_remove(struct platform_device *pdev);


static struct usb_device_id ft232h_usb_table[] = {
	{ USB_DEVICE( VID_FTDI, PID_FT232H ), },
	{}
};

MODULE_DEVICE_TABLE(usb, ft232h_usb_table);

static int ft232h_usb_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	int ret;
dev_info(&intf->dev, "[%s]:\n", __func__);
	if (ft232h_probed) {
		dev_info(&intf->dev, "[%s] already probed...\n", __func__);
		return 0;
	}
	ret = ft232h_intf_probe(intf, id);
	if (ret == 0) {
		struct ft232h_intf_priv *priv = usb_get_intfdata(intf);

		ret = ft232h_spi_probe(priv->spi_pdev);
	}
	
	if (ret == 0) {
		ft232h_probed = true;
	}
dev_info(&intf->dev, "[%s]: done... %d\n", __func__, ret);
	return ret;
}

static void ft232h_usb_disconnect(struct usb_interface *intf)
{
	struct ft232h_intf_priv *priv = usb_get_intfdata(intf);
	dev_info(&intf->dev, "[%s]: p:%p %d\n", __func__, priv, ft232h_diconnected);
	if (priv) {
		ft232h_spi_remove(priv->spi_pdev);
		ft232h_intf_disconnect(priv->intf);
	}
	dev_info(&intf->dev, "[%s]: done...\n", __func__);
	ft232h_diconnected++;
}

static struct usb_driver ft232h_usb_driver = {
	.name		= KBUILD_MODNAME,
	.id_table	= ft232h_usb_table,
	.probe		= ft232h_usb_probe,
	.disconnect	= ft232h_usb_disconnect,
};

module_usb_driver(ft232h_usb_driver);

MODULE_ALIAS("ft232h-usb-spi");
MODULE_AUTHOR("Newracom, Inc. <www.newracom.com>");
MODULE_AUTHOR("Sangbeom Kim <sb.kim@newracom.com>");
MODULE_AUTHOR("Anatolij Gustschin <agust@denx.de>");
MODULE_DESCRIPTION("FTDI FT232H USB-SPI bridge driver");
MODULE_LICENSE("GPL v2");

