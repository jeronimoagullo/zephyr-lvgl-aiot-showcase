# Install Mosquitto in Ubuntu
## Install Mosquitto server
```bash
sudo apt install mosquitto
```

## Install Mosquitto clients
```bash
sudo apt install mosquitto-clients
```

## Checkout installation
In one terminal:
```bash
mosquitto_sub -t "test"
```

Then, in another terminal:
```bash
mosquitto_pub -m "message from mosquitto_pub" -t "test"
```

You should see the message in the first terminal

# Mosquitto as a service
By default, the installation create a new systemd service named `mosquitto.service` which will be running continously the command `/usr/sbin/mosquitto -c /etc/mosquitto/mosquitto.conf`. You can get its info with the command `systemctl status mosquitto.service`.

Besides, you can restart, stop and start the service as required so that you can modify the configuration of `/etc/mosquitto/mosquitto.conf`:

```bash
systemctl restart mosquitto.service
systemctl stop mosquitto.service
systemctl start mosquitto.service
```

Another approach for testing would be to stop the service and run mosquitto manually with the following command:
```bash
mosquitto -v -c mosquitto.conf
```

I recommend this approach for testing the different configuration files of this repository.

# Use Mosquitto with different networks
In case that you are using different networks, for example, you are sharing your PC network to the board instead of using the router, you need to add this line to `mosquitto.conf`, so that mosquitto server can handle connections from other networks.

```
listener 1883 0.0.0.0
```

# Set up Username and Password
TODO

# Set up TLS
TODO