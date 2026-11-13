# Example Description

Example for a wowlan client to keeping alive with the server, and waking up when receiving a wake-up pattern.

# HW Configuration

None

# SW configuration

1. Enable `CONFIG_STANDARD_TICKLESS` in menuconfig.
2. Set the conditions required for WoWLAN in `wowlan_set_parameter()`
3. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.
4. After boot, use the command `AT+WLCONN` to connect to the corresponding AP.

# Expect result

A wowlan tickless example thread will be started automatically when connect to the keepalive server.

# Note

None

# Supported IC

RTL8735C
RTL8720F
