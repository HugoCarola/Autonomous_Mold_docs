/* Networking DHCPv4 client */

/*
 * Copyright (c) 2017 ARM Ltd.
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
////////////////// exemplo 1 /////////////////////////////////////
//#include <zephyr/logging/log.h>
//LOG_MODULE_REGISTER(net_dhcpv4_client_sample, LOG_LEVEL_DBG);
/*
#include <zephyr/kernel.h>
#include <zephyr/linker/sections.h>
#include <errno.h>
#include <stdio.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_core.h>
#include <zephyr/net/net_context.h>
#include <zephyr/net/net_mgmt.h>

#ifdef IS_ENABLED(CONFIG_NET_L2_ETHERNET))
#include <zephyr/net/net_if.h>
#endif

static struct net_mgmt_event_callback mgmt_cb;

static void handler(struct net_mgmt_event_callback *cb,
		    uint32_t mgmt_event,
		    struct net_if *iface)
{
	int i = 0;

	if (mgmt_event != NET_EVENT_IPV4_ADDR_ADD) {
		return;
	}

	for (i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
		char buf[NET_IPV4_ADDR_LEN];

		if (iface->config.ip.ipv4->unicast[i].addr_type !=
							NET_ADDR_DHCP) {
			continue;
		}

		LOG_INF("Your address: %s",
			net_addr_ntop(AF_INET,
			    &iface->config.ip.ipv4->unicast[i].address.in_addr,
						  buf, sizeof(buf)));
		LOG_INF("Lease time: %u seconds",
			 iface->config.dhcpv4.lease_time);
		LOG_INF("Subnet: %s",
			net_addr_ntop(AF_INET,
				       &iface->config.ip.ipv4->netmask,
				       buf, sizeof(buf)));
		LOG_INF("Router: %s",
			net_addr_ntop(AF_INET,
						 &iface->config.ip.ipv4->gw,
						 buf, sizeof(buf)));
	}
}

void main(void)
{  
    struct net_if *iface;

	LOG_INF("Run dhcpv4 client");

	net_mgmt_init_event_callback(&mgmt_cb, handler,
				     NET_EVENT_IPV4_ADDR_ADD);
	net_mgmt_add_event_callback(&mgmt_cb);

	iface = net_if_get_default();

	net_dhcpv4_start(iface);
	
}
*/
/////////////////////////// exemplo 2 //////////////////////////
/*
#include <zephyr.h>
#include <net/socket.h>
#include <logging/log.h>

LOG_MODULE_REGISTER(tcp_server, LOG_LEVEL_INF);

#define SERVER_PORT 4242
#define RECV_BUFFER_SIZE 128

void main(void) {
    int server_fd, client_fd;
    struct sockaddr_in addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char recv_buffer[RECV_BUFFER_SIZE];
    int ret;

    // Cria o socket
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd < 0) {
        LOG_ERR("Falha ao criar socket: %d", errno);
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Vincula o socket ao endereço e porta
    ret = bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_ERR("Falha ao fazer bind: %d", errno);
        close(server_fd);
        return;
    }

    // Coloca o socket em modo de escuta
    ret = listen(server_fd, 5);
    if (ret < 0) {
        LOG_ERR("Falha ao colocar em escuta: %d", errno);
        close(server_fd);
        return;
    }

    LOG_INF("Servidor TCP ouvindo na porta %d", SERVER_PORT);

    while (1) {
        // Aceita conexões de clientes
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            LOG_ERR("Falha ao aceitar conexão: %d", errno);
            continue;
        }

        LOG_INF("Cliente conectado");

        // Recebe dados do cliente
        ret = recv(client_fd, recv_buffer, sizeof(recv_buffer), 0);
        if (ret > 0) {
            recv_buffer[ret] = '\0'; // Garante terminação da string
            LOG_INF("Recebido do cliente: %s", recv_buffer);

            // Envia resposta ao cliente
            send(client_fd, "Mensagem recebida!\n", 19, 0);
        }

        // Fecha a conexão com o cliente
        close(client_fd);
    }
}
*/
//CMake multiplos overlays
// set(DTC_OVERLAY_FILE "${CMAKE_CURRENT_SOURCE_DIR}/file1.overlay ${CMAKE_CURRENT_SOURCE_DIR}/file2.overlay")



/*
 * Copyright (c) 2022 Grant Ramsay <grant.ramsay@hotmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
//Includes
#include <stdio.h>
#include <stdint.h>
//#include "nrf.h"
#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/drivers/ethernet/eth_nxp_enet.h>
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/device.h>

LOG_MODULE_REGISTER(tcp_server, LOG_LEVEL_INF);

#define SERVER_PORT 4242
#define RECV_BUFFER_SIZE 128

#define SPI_INSTANCE  0  // SPI instance index
#define W5500_CS_PIN  22 // Chip Select (CS) pin

#define Mensagem "1 2 3 4\n"

// Para testes
static const struct device *spi_dev;
static const struct device *gpio_dev;

#define SPI_OP_READ 0x00

 Configuração do SPI3 

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

 Variáveis para interface de rede 
struct net_if *iface;
struct ethernet_config cfg;

#define SPI_NODE DT_NODELABEL(spi3)

static const struct device *spi_dev = DEVICE_DT_GET(SPI_NODE);
static struct spi_config spi_cfg = {
    .operation = SPI_OP_MODE_MASTER | SPI_TRANSFER_MSB | SPI_WORD_SET(8),
    .frequency = 8000000,  8 MHz 
    .slave     = 0,
};

 Função para inicializar a rede 
void init_network(void)
{
    LOG_INF("Iniciando rede com IP estático...");

    iface = net_if_get_default();

    if (iface == NULL) {
        LOG_ERR("Interface de rede não encontrada!");
        return;
    }

    LOG_INF("Interface de rede ativa com IP: 192.168.1.100");
}

 Função para configurar o SPI 
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



int main(void)
{
    LOG_INF("Testes");
    
    uint8_t tx_buf[] = {0x00, 0x00};  Comando SPI 
    uint8_t rx_buf[2] = {0};

    const struct spi_buf tx_spi_buf = {.buf = tx_buf, .len = sizeof(tx_buf)};
    const struct spi_buf rx_spi_buf = {.buf = rx_buf, .len = sizeof(rx_buf)};
    
    const struct spi_buf_set tx = {.buffers = &tx_spi_buf, .count = 1};
    const struct spi_buf_set rx = {.buffers = &rx_spi_buf, .count = 1};
    
    if (!device_is_ready(spi_dev)) {
        LOG_ERR("Erro: SPI3 não está pronto");
    }

    LOG_INF("Testar a comunicacao SPI...");
    int ret1 = spi_transceive(spi_dev, &spi_cfg, &tx, &rx);
    if (ret1 == 0) {
        LOG_INF("Sucesso: RX[0] = 0x%x RX[1] = 0x%x", rx_buf[0], rx_buf[1]);
    } else {
        LOG_ERR("Erro: %d\n", ret1);
    }

    //////////////////////// Testes w5500 //////////////////////////////////////////
    LOG_INF("Iniciando teste de comunicação com o W5500...");

    struct net_if *iface = net_if_get_default();
    net_dhcpv4_start(iface);
    if (iface == NULL) {
        LOG_ERR("Nenhuma interface de rede encontrada");
        return -1;
    }

    // Verificar se a interface está ativa
    if (!net_if_is_up(iface)) {
        LOG_ERR("Interface de rede não está ativa");
    } else {
        LOG_INF("Interface de rede está ativa");
    }

    LOG_INF("Testes terminados");
    k_sleep(K_MSEC(500));
    ///////////////////////// Comunicação //////////////////////////////////////////
	LOG_INF("COMEÇO");

	int server_fd, client_fd;
    struct sockaddr_in addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char recv_buffer[RECV_BUFFER_SIZE];
    int ret;

    // Cria o socket
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd < 0) {
        LOG_ERR("Falha ao criar socket: %d", errno);
        return -1;
    }
    

    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Vincula o socket ao endereço e porta
    ret = bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_ERR("Falha ao fazer bind: %d", errno);
        close(server_fd);
        return -1;
    }


    // Coloca o socket em modo de escuta
    ret = listen(server_fd, 5);
    if (ret < 0) {
        LOG_ERR("Falha ao colocar em escuta: %d", errno);
        close(server_fd);
        return -1;
    }

    LOG_INF("Servidor TCP ouvindo na porta %d", SERVER_PORT);

    while(1) {
        LOG_INF("ENTREI NO WHILE");





        // Aceita conexões de clientes
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            LOG_ERR("Falha ao aceitar conexão: %d", errno);
            continue;
        }

        LOG_INF("Cliente conectado");

        // Recebe dados do cliente
        ret = recv(client_fd, recv_buffer, sizeof(recv_buffer), 0);
        if (ret > 0) {
            recv_buffer[ret] = '\0'; // Garante terminação da string
            LOG_INF("Recebido do cliente: %s", recv_buffer);

            // Envia resposta ao cliente
            send(client_fd, Mensagem, 19, 0);
        }

        // Fecha a conexão com o cliente
        close(client_fd);
    }
	
	return 0;
}
*/