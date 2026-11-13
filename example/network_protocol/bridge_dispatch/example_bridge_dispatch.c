#include <stdio.h>
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"

#if defined(CONFIG_ARM_CORE_CM4_NP) && defined(CONFIG_WHC_DEV_TCPIP_KEEPALIVE)
#include "bridge_dispatch.h"

#define KA_SERVER_IP    			"192.168.10.126"
#define KA_SERVER_PORT  			8899
#define KA_HEARTBEAT_INTERVAL_MS  	5000
#define KA_HEARTBEAT_MSG          	"_alive"

static int         g_ex_ka_sock = -1;

static int example_ka_connect(void)
{
	struct sockaddr_in server_addr;

	g_ex_ka_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_ex_ka_sock < 0) {
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(KA_SERVER_IP);
	server_addr.sin_port = htons(KA_SERVER_PORT);
	if (connect(g_ex_ka_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
		goto fail;
	}
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "ka connected %s:%d\n", KA_SERVER_IP, KA_SERVER_PORT);
	return 0;
fail:
	close(g_ex_ka_sock);
	g_ex_ka_sock = -1;
	return -1;
}

static void example_ka_task(void *param)
{
	(void)param;

	/* wait until the NP has an IP */
	while (1) {
		u8 *ip = lwip_get_ip(NETIF_WLAN_STA_INDEX);
		if (ip != NULL && (ip[0] | ip[1] | ip[2] | ip[3]) != 0) {
			break;
		}
		rtos_time_delay_ms(100);
	}

	/* try to connect once */
	if (example_ka_connect() != 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "ka connect failed, give up\n");
		rtos_task_delete(NULL);
		return;
	}

	while (1) {
		if (write(g_ex_ka_sock, KA_HEARTBEAT_MSG, sizeof(KA_HEARTBEAT_MSG) - 1) <= 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "ka heartbeat send err %d\n", errno);
			break;
		}
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "ka heartbeat sent\n");
		rtos_time_delay_ms(KA_HEARTBEAT_INTERVAL_MS);
	}

	close(g_ex_ka_sock);
	g_ex_ka_sock = -1;
	rtos_task_delete(NULL);
}

void example_bridge_dispatch(void)
{
	bridge_dispatch_init();
	if (rtos_task_create(NULL, "example_ka_sock", example_ka_task, NULL, 4096, 2) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "example ka task fail\n");
	}
}

#endif /* CONFIG_ARM_CORE_CM4_NP && CONFIG_WHC_DEV_TCPIP_KEEPALIVE */
