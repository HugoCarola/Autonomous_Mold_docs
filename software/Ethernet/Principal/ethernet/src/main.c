/*
 * Copyright (c) 2022 Grant Ramsay <grant.ramsay@hotmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT	wiznet_w5500

//Includes
#include <stdio.h>
#include <stdint.h>
//#include "nrf.h"
#include <string.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/drivers/ethernet/eth_nxp_enet.h>
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>
#include <zephyr/random/random.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/icmp.h>
#include <zephyr/net/net_pkt.h>
#include <ethernet/eth_stats.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/net/net_pkt.h>
#include <zephyr/syscalls/random.h>

#include "eth.h"
#include "eth_w5500_priv.h"

//#include <zephyr/net/ipv6.h>
//#include <zephyr/net/net_route.h>
//#include <zephyr/net/icmpv6.h>



LOG_MODULE_REGISTER(main, CONFIG_ETHERNET_LOG_LEVEL);

#define WIZNET_OUI_B0	0x00
#define WIZNET_OUI_B1	0x08
#define WIZNET_OUI_B2	0xdc

#define W5500_SPI_BLOCK_SELECT(addr)	(((addr) >> 16) & 0x1f)
#define W5500_SPI_READ_CONTROL(addr)	(W5500_SPI_BLOCK_SELECT(addr) << 3)
#define W5500_SPI_WRITE_CONTROL(addr)   \
	((W5500_SPI_BLOCK_SELECT(addr) << 3) | BIT(2))


#define Mensagem "1 2 3 4\n"
#define IP_ADDR "192.168.1.100"
#define NETMASK "255.255.255.0"
#define GATEWAY "192.168.1.1"  // Defina o Gateway (Roteador) para a sua rede


/* Variáveis para interface de rede */
struct net_if *iface;
struct ethernet_config cfg;

#define SPI_NODE DT_NODELABEL(spi3)



/* Função para inicializar a rede */
void init_network(void)
{
    LOG_INF("Iniciando rede com IP estático...");

    struct net_if *iface = net_if_get_default();
    
    if (iface == NULL) {
        LOG_ERR("Interface de rede não encontrada!");
        return;
    }

    LOG_INF("Interface de rede ativa com IP: 192.168.1.100");
}

/* Função para configurar o SPI */
void init_spi(void)
{
    LOG_INF("Inicializando SPI3 para W5500...");
    const struct device *spi_dev = DEVICE_DT_GET(DT_NODELABEL(spi3));

    if (!device_is_ready(spi_dev)) {
        LOG_ERR("Dispositivo SPI3 não está pronto!");
        return;
    }

    LOG_INF("SPI3 inicializado com sucesso.");
}

/*
void print_ipv6_addresses(void) {
    struct net_if *iface = net_if_get_default();
    if (iface == NULL) {
        printk("Interface de rede padrão não encontrada.\n");
        return;
    }

    struct net_if_ipv6 *ipv6 = &iface->config.ip.ipv6;
    
    if (ipv6 == NULL) {
        printk("IPv6 não configurado nesta interface.\n");
        return;
    }

    printk("Endereços IPv6 configurados:\n");

     Verificar e exibir o endereço IPv6 configurado na interface 
    for (int i = 0; i < NET_IF_MAX_IPV6_ADDR; i++) {
        struct net_if_addr *addr = &ipv6->unicast[i];

        if (addr->is_used) {
            char addr_str[NET_IPV6_ADDR_LEN];
            net_addr_ntop(AF_INET6, &addr->address.in6_addr, addr_str, sizeof(addr_str));
            printk("  -> %s\n", addr_str);
        }
    }
}

void configure_ipv6_gateway(void) {
    struct net_if *iface;
    struct in6_addr gateway_addr = { { 0x78, 0x3b, 0x10, 0x00, 0x84, 0x18, 0x20, 0x00 } }; // exemplo de endereço de gateway
    struct net_route *route;

    // Obtém a interface de rede padrão
    iface = net_if_get_default();

    // Cria uma rota para o gateway IPv6
    route = net_route_add(iface, &gateway_addr, NET_IPV6_ADDR_PREFIX_LEN, NET_ROUTE_DEFAULT);
    if (route) {
        printk("Rota IPv6 configurada para o gateway.\n");
    } else {
        printk("Falha ao configurar a rota IPv6.\n");
    }
}*/


void print_ipv4_addresses(void) {
    struct net_if *iface = net_if_get_default();
    struct net_if_addr_ipv4 *ipv4_config;
    char ip_addr[NET_IPV4_ADDR_LEN];

    if (iface == NULL) {
        printk("Interface de rede padrão não encontrada.\n");
        return;
    }

    // Obtenha a configuração IPv4 da interface
    ipv4_config = iface->config.ip.ipv4;
    if (!ipv4_config) {
        printk("Erro: Configuração IPv4 não encontrada para a interface\n");
        return;
    }

    // Verifique se um endereço IP está configurado
    //net_if_ipv4_addr_add(iface, CONFIG_NET_CONFIG_MY_IPV4_ADDR, )
    if (ipv4_config->ipv4.addr_state == NET_ADDR_PREFERRED) {
        net_addr_ntop(AF_INET, &ipv4_config->ipv4.address.in_addr, ip_addr, sizeof(ip_addr));
        printk("Endereço IP configurado: %s\n", ip_addr);
    } else {
        printk("Nenhum endereço IP configurado\n");
    }
}

void configIPV4(void){
    struct net_if *iface;
    struct in_addr ipaddr, netmask, gw;

    // Obtenha a interface de rede padrão
    iface = net_if_get_default();
    if (!iface) {
        printk("Erro: Não foi possível obter a interface de rede padrão\n");
        return;
    }

    // Configurar endereço IP
    if (net_addr_pton(AF_INET,CONFIG_NET_CONFIG_MY_IPV4_ADDR, &ipaddr) < 0) {
        printk("Erro ao converter endereço IP\n");
        return;
    }

    // Configurar máscara de sub-rede
    if (net_addr_pton(AF_INET, CONFIG_NET_CONFIG_MY_IPV4_NETMASK, &netmask) < 0) {
        printk("Erro ao converter máscara de sub-rede\n");
        return;
    }

    // Configurar gateway
    if (net_addr_pton(AF_INET, CONFIG_NET_CONFIG_MY_IPV4_GW, &gw) < 0) {
        printk("Erro ao converter endereço do gateway\n");
        return;
    }

    // Adicionar endereço IP à interface
    net_if_ipv4_addr_add(iface, &ipaddr, NET_ADDR_MANUAL, 0);

    // Configurar máscara de sub-rede
    net_if_ipv4_set_netmask(iface, &netmask);

    // Configurar gateway padrão
    net_if_ipv4_set_gw(iface, &gw);

    printk("Configuração de rede concluída!\n");

}

void pingParaGateway(){
    struct net_if *iface;
    struct sockaddr_in dst;  // Estrutura de destino para IPv4
    struct net_icmp_ping_params params;
    int ret;

    printk("Iniciando envio de ICMP Echo Request (ping)...\n");

    // Obter a interface de rede padrão
    iface = net_if_get_default();
    if (!iface) {
        printk("Erro: Não foi possível obter a interface de rede padrão.\n");
        return;
    }

    // Configurar o endereço de destino
    dst.sin_family = AF_INET;  // Família de endereços IPv4
    dst.sin_port = 0;          // ICMP não usa portas
    net_addr_pton(AF_INET, CONFIG_NET_CONFIG_MY_IPV4_GW, &dst.sin_addr);  // Converter string IP para binário

    // Configurar os parâmetros do Echo Request
    params.identifier = htons(0x1234);  // Identificador (valor arbitrário)
    params.sequence = htons(1);         // Número de sequência inicial

    // Enviar o ICMP Echo Request
    ret = net_icmp_send_echo_request(NULL, iface, (struct sockaddr *)&dst, NULL, NULL);
    if (ret < 0) {
        printk("Erro ao enviar ICMP Echo Request: %d\n", ret);
    } else {
        printk("ICMP Echo Request enviado para %s com sucesso!\n", CONFIG_NET_CONFIG_MY_IPV4_GW);
    }
}

#define WIZNET_OUI_B0	0x00
#define WIZNET_OUI_B1	0x08
#define WIZNET_OUI_B2	0xdc

#define W5500_SPI_BLOCK_SELECT(addr)	(((addr) >> 16) & 0x1f)
#define W5500_SPI_READ_CONTROL(addr)	(W5500_SPI_BLOCK_SELECT(addr) << 3)
#define W5500_SPI_WRITE_CONTROL(addr)   \
	((W5500_SPI_BLOCK_SELECT(addr) << 3) | BIT(2))

static int w5500_spi_read(const struct device *dev, uint32_t addr,
			  uint8_t *data, uint32_t len)
{
	const struct w5500_config *cfg = dev->config;
	int ret;
	/* 3 bytes as 0x010203 during command phase */
	uint8_t tmp[len + 3];

	uint8_t cmd[3] = {
		addr >> 8,
		addr,
		W5500_SPI_READ_CONTROL(addr)
	};
	const struct spi_buf tx_buf = {
		.buf = cmd,
		.len = ARRAY_SIZE(cmd),
	};
	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1,
	};
	const struct spi_buf rx_buf = {
		.buf = tmp,
		.len = ARRAY_SIZE(tmp),
	};
	const struct spi_buf_set rx = {
		.buffers = &rx_buf,
		.count = 1,
	};

	ret = spi_transceive_dt(&cfg->spi, &tx, &rx);

	if (!ret) {
		/* skip the default dummy 0x010203 */
		memcpy(data, &tmp[3], len);
	}

	return ret;
}

static int w5500_spi_write(const struct device *dev, uint32_t addr,
			   uint8_t *data, uint32_t len)
{
	const struct w5500_config *cfg = dev->config;
	int ret;
	uint8_t cmd[3] = {
		addr >> 8,
		addr,
		W5500_SPI_WRITE_CONTROL(addr),
	};
	const struct spi_buf tx_buf[2] = {
		{
			.buf = cmd,
			.len = ARRAY_SIZE(cmd),
		},
		{
			.buf = data,
			.len = len,
		},
	};
	const struct spi_buf_set tx = {
		.buffers = tx_buf,
		.count = ARRAY_SIZE(tx_buf),
	};

	ret = spi_write_dt(&cfg->spi, &tx);

	return ret;
}

static int w5500_readbuf(const struct device *dev, uint16_t offset, uint8_t *buf,
			 int len)
{
	uint32_t addr;
	int remain = 0;
	int ret;
	const uint32_t mem_start = W5500_Sn_RX_MEM_START;
	const uint16_t mem_size = W5500_RX_MEM_SIZE;

	offset %= mem_size;
	addr = mem_start + offset;

	if (offset + len > mem_size) {
		remain = (offset + len) % mem_size;
		len = mem_size - offset;
	}

	ret = w5500_spi_read(dev, addr, buf, len);
	if (ret || !remain) {
		return ret;
	}

	return w5500_spi_read(dev, mem_start, buf + len, remain);
}

static int w5500_writebuf(const struct device *dev, uint16_t offset, uint8_t *buf,
			  int len)
{
	uint32_t addr;
	int ret = 0;
	int remain = 0;
	const uint32_t mem_start = W5500_Sn_TX_MEM_START;
	const uint32_t mem_size = W5500_TX_MEM_SIZE;

	offset %= mem_size;
	addr = mem_start + offset;

	if (offset + len > mem_size) {
		remain = (offset + len) % mem_size;
		len = mem_size - offset;
	}

	ret = w5500_spi_write(dev, addr, buf, len);
	if (ret || !remain) {
		return ret;
	}

	return w5500_spi_write(dev, mem_start, buf + len, remain);
}

static int w5500_command(const struct device *dev, uint8_t cmd)
{
	uint8_t reg;
	uint64_t end = sys_clock_timeout_end_calc(K_MSEC(100));

	w5500_spi_write(dev, W5500_S0_CR, &cmd, 1);
	while (1) {
		w5500_spi_read(dev, W5500_S0_CR, &reg, 1);
		if (!reg) {
			break;
			}
		int64_t remaining = end - sys_clock_tick_get();
		if (remaining <= 0) {
			return -EIO;
			}
		k_busy_wait(W5500_PHY_ACCESS_DELAY);
		}
	return 0;
}

static int w5500_tx(const struct device *dev, struct net_pkt *pkt)
{
	struct w5500_runtime *ctx = dev->data;
	uint16_t len = net_pkt_get_len(pkt);
	uint16_t offset;
	uint8_t off[2];
	int ret;

	w5500_spi_read(dev, W5500_S0_TX_WR, off, 2);
	offset = sys_get_be16(off);

	if (net_pkt_read(pkt, ctx->buf, len)) {
		return -EIO;
	}

	ret = w5500_writebuf(dev, offset, ctx->buf, len);
	if (ret < 0) {
		return ret;
	}

	sys_put_be16(offset + len, off);
	w5500_spi_write(dev, W5500_S0_TX_WR, off, 2);

	w5500_command(dev, S0_CR_SEND);
	if (k_sem_take(&ctx->tx_sem, K_MSEC(10))) {
		return -EIO;
	}

	return 0;
}

static void w5500_rx(const struct device *dev)
{
	uint8_t header[2];
	uint8_t tmp[2];
	uint16_t off;
	uint16_t rx_len;
	uint16_t rx_buf_len;
	uint16_t read_len;
	uint16_t reader;
	struct net_buf *pkt_buf = NULL;
	struct net_pkt *pkt;
	struct w5500_runtime *ctx = dev->data;
	const struct w5500_config *config = dev->config;

	w5500_spi_read(dev, W5500_S0_RX_RSR, tmp, 2);
	rx_buf_len = sys_get_be16(tmp);

	if (rx_buf_len == 0) {
		return;
	}

	w5500_spi_read(dev, W5500_S0_RX_RD, tmp, 2);
	off = sys_get_be16(tmp);

	w5500_readbuf(dev, off, header, 2);
	rx_len = sys_get_be16(header) - 2;

	pkt = net_pkt_rx_alloc_with_buffer(ctx->iface, rx_len,
			AF_UNSPEC, 0, K_MSEC(config->timeout));
	if (!pkt) {
		eth_stats_update_errors_rx(ctx->iface);
		return;
	}

	pkt_buf = pkt->buffer;

	read_len = rx_len;
	reader = off + 2;

	do {
		size_t frag_len;
		uint8_t *data_ptr;
		size_t frame_len;

		data_ptr = pkt_buf->data;

		frag_len = net_buf_tailroom(pkt_buf);

		if (read_len > frag_len) {
			frame_len = frag_len;
		} else {
			frame_len = read_len;
		}

		w5500_readbuf(dev, reader, data_ptr, frame_len);
		net_buf_add(pkt_buf, frame_len);
		reader += frame_len;

		read_len -= frame_len;
		pkt_buf = pkt_buf->frags;
	} while (read_len > 0);

	if (net_recv_data(ctx->iface, pkt) < 0) {
		net_pkt_unref(pkt);
	}

	sys_put_be16(off + 2 + rx_len, tmp);
	w5500_spi_write(dev, W5500_S0_RX_RD, tmp, 2);
	w5500_command(dev, S0_CR_RECV);
}

static void w5500_thread(const struct device *dev)
{
	uint8_t ir;
	struct w5500_runtime *ctx = dev->data;
	const struct w5500_config *config = dev->config;

	while (true) {
		k_sem_take(&ctx->int_sem, K_FOREVER);

		while (gpio_pin_get_dt(&(config->interrupt))) {
			/* Read interrupt */
			w5500_spi_read(dev, W5500_S0_IR, &ir, 1);

			if (ir) {
				/* Clear interrupt */
				w5500_spi_write(dev, W5500_S0_IR, &ir, 1);

				LOG_DBG("IR received");

				if (ir & S0_IR_SENDOK) {
					k_sem_give(&ctx->tx_sem);
					LOG_DBG("TX Done");
				}

				if (ir & S0_IR_RECV) {
					w5500_rx(dev);
					LOG_DBG("RX Done");
				}
			}
		}
	}
}

static void w5500_iface_init(struct net_if *iface)
{
	const struct device *dev = net_if_get_device(iface);
	struct w5500_runtime *ctx = dev->data;

	net_if_set_link_addr(iface, ctx->mac_addr,
			     sizeof(ctx->mac_addr),
			     NET_LINK_ETHERNET);

	if (!ctx->iface) {
		ctx->iface = iface;
	}

	ethernet_init(iface);
}

static enum ethernet_hw_caps w5500_get_capabilities(const struct device *dev)
{
	ARG_UNUSED(dev);

	return ETHERNET_LINK_10BASE_T | ETHERNET_LINK_100BASE_T
#if defined(CONFIG_NET_PROMISCUOUS_MODE)
		| ETHERNET_PROMISC_MODE
#endif
	;
}

static int w5500_set_config(const struct device *dev,
			    enum ethernet_config_type type,
			    const struct ethernet_config *config)
{
	struct w5500_runtime *ctx = dev->data;

	switch (type) {
	case ETHERNET_CONFIG_TYPE_MAC_ADDRESS:
		memcpy(ctx->mac_addr,
			config->mac_address.addr,
			sizeof(ctx->mac_addr));
		w5500_spi_write(dev, W5500_SHAR, ctx->mac_addr, sizeof(ctx->mac_addr));
		LOG_INF("%s MAC set to %02x:%02x:%02x:%02x:%02x:%02x",
			dev->name,
			ctx->mac_addr[0], ctx->mac_addr[1],
			ctx->mac_addr[2], ctx->mac_addr[3],
			ctx->mac_addr[4], ctx->mac_addr[5]);

		/* Register Ethernet MAC Address with the upper layer */
		net_if_set_link_addr(ctx->iface, ctx->mac_addr,
			sizeof(ctx->mac_addr),
			NET_LINK_ETHERNET);

		return 0;
	case ETHERNET_CONFIG_TYPE_PROMISC_MODE:
		if (IS_ENABLED(CONFIG_NET_PROMISCUOUS_MODE)) {
			uint8_t mode;
			uint8_t mr = W5500_S0_MR_MF;

			w5500_spi_read(dev, W5500_S0_MR, &mode, 1);

			if (config->promisc_mode) {
				if (!(mode & BIT(mr))) {
					return -EALREADY;
				}

				/* disable MAC filtering */
				WRITE_BIT(mode, mr, 0);
			} else {
				if (mode & BIT(mr)) {
					return -EALREADY;
				}

				/* enable MAC filtering */
				WRITE_BIT(mode, mr, 1);
			}

			return w5500_spi_write(dev, W5500_S0_MR, &mode, 1);
		}

		return -ENOTSUP;
	default:
		return -ENOTSUP;
	}
}

static int w5500_hw_start(const struct device *dev)
{
	uint8_t mode = S0_MR_MACRAW | BIT(W5500_S0_MR_MF);
	uint8_t mask = IR_S0;

	/* configure Socket 0 with MACRAW mode and MAC filtering enabled */
	w5500_spi_write(dev, W5500_S0_MR, &mode, 1);
	w5500_command(dev, S0_CR_OPEN);

	/* enable interrupt */
	w5500_spi_write(dev, W5500_SIMR, &mask, 1);

	return 0;
}

static int w5500_hw_stop(const struct device *dev)
{
	uint8_t mask = 0;

	/* disable interrupt */
	w5500_spi_write(dev, W5500_SIMR, &mask, 1);
	w5500_command(dev, S0_CR_CLOSE);

	return 0;
}

static struct ethernet_api w5500_api_funcs = {
	.iface_api.init = w5500_iface_init,
	.get_capabilities = w5500_get_capabilities,
	.set_config = w5500_set_config,
	.start = w5500_hw_start,
	.stop = w5500_hw_stop,
	.send = w5500_tx,
};

static int w5500_hw_reset(const struct device *dev)
{
	int ret;
	uint8_t mask = 0;
	uint8_t tmp = MR_RST;

	ret = w5500_spi_write(dev, W5500_MR, &tmp, 1);
	if (ret < 0) {
		return ret;
	}

	k_msleep(5);
	tmp = MR_PB;
	w5500_spi_write(dev, W5500_MR, &tmp, 1);

	/* disable interrupt */
	return w5500_spi_write(dev, W5500_SIMR, &mask, 1);
}

static void w5500_gpio_callback(const struct device *dev,
				struct gpio_callback *cb,
				uint32_t pins)
{
	struct w5500_runtime *ctx =
		CONTAINER_OF(cb, struct w5500_runtime, gpio_cb);

	k_sem_give(&ctx->int_sem);
}

/*
static void w5500_set_macaddr(const struct device *dev)
{
	struct w5500_runtime *ctx = dev->data;

#if DT_INST_PROP(0, zephyr_random_mac_address)
	 override vendor bytes 
	memset(ctx->mac_addr, '\0', sizeof(ctx->mac_addr));
	ctx->mac_addr[0] = WIZNET_OUI_B0;
	ctx->mac_addr[1] = WIZNET_OUI_B1;
	ctx->mac_addr[2] = WIZNET_OUI_B2;
	if (ctx->generate_mac) {
		ctx->generate_mac(ctx->mac_addr);
	}
#endif
	memset(ctx->mac_addr, '\0', sizeof(ctx->mac_addr));
	ctx->mac_addr[0] = WIZNET_OUI_B0;
	ctx->mac_addr[1] = WIZNET_OUI_B1;
	ctx->mac_addr[2] = WIZNET_OUI_B2;
	if (ctx->generate_mac) {
		ctx->generate_mac(ctx->mac_addr);
	}
	w5500_spi_write(dev, W5500_SHAR, ctx->mac_addr, sizeof(ctx->mac_addr));
}
*/

static void w5500_memory_configure(const struct device *dev)
{
	int i;
	uint8_t mem = 0x10;

	/* Configure RX & TX memory to 16K */
	w5500_spi_write(dev, W5500_Sn_RXMEM_SIZE(0), &mem, 1);
	w5500_spi_write(dev, W5500_Sn_TXMEM_SIZE(0), &mem, 1);

	mem = 0;
	for (i = 1; i < 8; i++) {
		w5500_spi_write(dev, W5500_Sn_RXMEM_SIZE(i), &mem, 1);
		w5500_spi_write(dev, W5500_Sn_TXMEM_SIZE(i), &mem, 1);
	}
}

static void w5500_random_mac(uint8_t *mac_addr)
{
	gen_random_mac(mac_addr, WIZNET_OUI_B0, WIZNET_OUI_B1, WIZNET_OUI_B2);
}

static int w5500_init(const struct device *dev)
{
	int err;
	uint8_t rtr[2];
	const struct w5500_config *config = dev->config;
	struct w5500_runtime *ctx = dev->data;

	if (!spi_is_ready_dt(&config->spi)) {
		LOG_ERR("SPI master port %s not ready", config->spi.bus->name);
		return -EINVAL;
	}

	if (!device_is_ready(config->interrupt.port)) {
		LOG_ERR("GPIO port %s not ready", config->interrupt.port->name);
		return -EINVAL;
	}

	if (gpio_pin_configure_dt(&config->interrupt, GPIO_INPUT)) {
		LOG_ERR("Unable to configure GPIO pin %u", config->interrupt.pin);
		return -EINVAL;
	}

	gpio_init_callback(&(ctx->gpio_cb), w5500_gpio_callback,
			   BIT(config->interrupt.pin));

	if (gpio_add_callback(config->interrupt.port, &(ctx->gpio_cb))) {
		return -EINVAL;
	}

	gpio_pin_interrupt_configure_dt(&config->interrupt,
					GPIO_INT_EDGE_FALLING);

	if (config->reset.port) {
		if (!device_is_ready(config->reset.port)) {
			LOG_ERR("GPIO port %s not ready", config->reset.port->name);
			return -EINVAL;
		}
		if (gpio_pin_configure_dt(&config->reset, GPIO_OUTPUT)) {
			LOG_ERR("Unable to configure GPIO pin %u", config->reset.pin);
			return -EINVAL;
		}
		gpio_pin_set_dt(&config->reset, 0);
		k_usleep(500);
	}

	err = w5500_hw_reset(dev);
	if (err) {
		LOG_ERR("Reset failed");
		return err;
	}

	w5500_set_macaddr(dev);
	w5500_memory_configure(dev);

	/* check retry time value */
	w5500_spi_read(dev, W5500_RTR, rtr, 2);
	if (sys_get_be16(rtr) != RTR_DEFAULT) {
		LOG_ERR("Unable to read RTR register");
		return -ENODEV;
	}

	k_thread_create(&ctx->thread, ctx->thread_stack,
			CONFIG_ETH_W5500_RX_THREAD_STACK_SIZE,
			(k_thread_entry_t)w5500_thread,
			(void *)dev, NULL, NULL,
			K_PRIO_COOP(CONFIG_ETH_W5500_RX_THREAD_PRIO),
			0, K_NO_WAIT);

	LOG_INF("W5500 Initialized");

	return 0;
}

/*
static struct w5500_runtime w5500_0_runtime = {
#if NODE_HAS_VALID_MAC_ADDR(DT_DRV_INST(0))
	.mac_addr = DT_INST_PROP(0, local_mac_address),
#endif
	.generate_mac = w5500_random_mac,
	.tx_sem = Z_SEM_INITIALIZER(w5500_0_runtime.tx_sem,
					1,  UINT_MAX),
	.int_sem  = Z_SEM_INITIALIZER(w5500_0_runtime.int_sem,
				      0, UINT_MAX),
};*/

static const struct w5500_config w5500_0_config = {
	.spi = SPI_DT_SPEC_INST_GET(0, SPI_WORD_SET(8), 0),
	.interrupt = GPIO_DT_SPEC_INST_GET(0, int_gpios),
	.reset = GPIO_DT_SPEC_INST_GET_OR(0, reset_gpios, { 0 }),
	.timeout = CONFIG_ETH_W5500_TIMEOUT,
};

/*
ETH_NET_DEVICE_DT_INST_DEFINE(0,
		    w5500_init, NULL,
		    &w5500_0_runtime, &w5500_0_config,
		    CONFIG_ETH_INIT_PRIORITY, &w5500_api_funcs, NET_ETH_MTU);
*/

int main(void)
{
    LOG_INF("Iniciando aplicação W5500 com IP estático...");

    /* Inicializar SPI */
    init_spi();
    
    configIPV4();
	

    /* Inicializar rede */
    init_network();
    
    ////////////// Teste /////////////////////////////
    print_ipv4_addresses();

    pingParaGateway();

    /* Loop principal */
    while (1) {
        LOG_INF("Aplicação a rodar...");
        k_sleep(K_SECONDS(5));
    }

    return -1;
}
