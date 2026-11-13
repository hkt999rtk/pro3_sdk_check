# Example Description

This example demonstrates a keep-alive heartbeat socket routed to the NP core by
**bridge_dispatch**.

In keep-alive bridge mode the NP owns its own lwIP/DHCP client. `bridge_dispatch_init()`
installs a custom RX routing rule: any TCP flow whose destination port falls in the NP's
local-port range is delivered to the NP lwIP; everything else follows the normal rules
(app traffic to the host, ARP/DNS/broadcast to both). Because the TCP ephemeral local-port
range is split per core, every socket the NP opens gets a port in the NP range and is
therefore routed to the NP automatically.

The example then just opens a TCP socket to a server and periodically sends a small
application-layer heartbeat. The NP keeps the connection alive by itself, so the host
(CA32/AP) can sleep while the link stays up.

What the example thread does (NP core only):

1. `bridge_dispatch_init()` — install the NP-range TCP redirect rule.
2. Wait until the NP has an IP (its own DHCP client, after `AT+WLCONN`).
3. Open a TCP socket and `connect()` to the keep-alive server. The local port is
   auto-assigned in the NP range, so the redirect rule keeps this flow on the NP.
4. Every `KA_HEARTBEAT_INTERVAL_MS`, send `KA_HEARTBEAT_MSG`. On a send failure the socket
   is closed and the task exits.

# HW Configuration

None

# SW configuration

1. `CONFIG_WHC_DEV_TCPIP_KEEPALIVE` is enabled by default for WHC_IPC.
2. In `example_bridge_dispatch.c`, set the server and heartbeat parameters:
   * `KA_SERVER_IP` / `KA_SERVER_PORT` — the keep-alive server address.
   * `KA_HEARTBEAT_INTERVAL_MS` — heartbeat period (default 5000 ms).
   * `KA_HEARTBEAT_MSG` — heartbeat payload (default `"_alive"`).
3. Build and download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
4. Start a matching server on the host, e.g. `nc -l -p 8899`.
5. After boot, use `AT+WLCONN=<ssid>,<pw>,...` to connect to the AP.

# Expect result

The example thread starts automatically on the NP core. After the STA connects and
the NP obtains an IP, the console shows:

```
[$]wifi got ip:"x.x.x.x"
ka connected <server>:<port>
ka heartbeat sent
...
```

The server receives one heartbeat (`_alive`) every `KA_HEARTBEAT_INTERVAL_MS`. 

# Note

* The keep-alive socket and its heartbeat are customer application code; this example
  is a reference.

# Supported IC

RTL8735C
RTL8720F
