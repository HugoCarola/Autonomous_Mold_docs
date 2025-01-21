/*
 * Copyright (c) 2022 Grant Ramsay <grant.ramsay@hotmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//Includes 
//#include <zephyr/device.h>
//#include <zephyr/drivers/spi.h>
//#include <zephyr/net/ethernet.h>
//#include <zephyr/net/net_if.h>




int main(void)
{
    //printf("OLA");
	//Verificar dispositivos
	//const struct device *spi_dev = DEVICE_DT_GET(DT_NODELABEL(spi1));
    //const struct device *eth_dev = DEVICE_DT_GET(DT_NODELABEL(w5500));
	//const struct device *w5500_dev = device_get_binding(DT_LABEL(DT_NODELABEL(w5500)));

	//if (!w5500_dev) {
	//	printk("Failed to get W5500 device\n");
	//	return;
	//}

    //if (!device_is_ready(spi_dev)) {
    //    printk("SPI device not ready\n");
    //    return -1;
    //}

    //if (!device_is_ready(eth_dev)) {
    //    printk("W5500 Ethernet device not ready\n");
    //    return -1;
    //}

    //printk("W5500 initialized successfully\n");

    // Configurações (DHCP or IP estático)
    //struct net_if *iface = net_if_get_default();
    //net_dhcpv4_start(iface);
	return 0;
}
