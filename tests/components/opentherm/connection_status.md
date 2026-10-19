# Boiler connection status

Enable the optional connectivity sensor alongside the existing OpenTherm sensors:

```yaml
opentherm:
  in_pin: 17
  out_pin: 16
  connection_timeout: 10s

binary_sensor:
  - platform: opentherm
    connected:
      name: "Boiler Connected"
      id: boiler_connected
    fault_indication:
      name: "Boiler Fault"
```

`connected` is OFF at startup, until the first verified reply from the boiler.
It turns ON immediately when a reply matches the request's message ID and type.
The protocol layer must first accept the frame's encoding and parity.
`DATA_INVALID` and `UNKNOWN_DATAID` replies also confirm communication, even though
the requested value or command is not supported.

The sensor turns OFF when no verified reply arrives for `connection_timeout`
(default 10 seconds, minimum 1 second). The timeout is checked once per second,
so the transition can occur up to one second after the deadline. A fresh reply
turns it ON again immediately. One lost packet does not change the status.
Both normal and synchronous communication modes use the same monitor.

`connected` measures OpenTherm communication, independently of Wi-Fi, the
Home Assistant API, boiler faults, flame state, and heating demand. It does not
send reset commands or change the boiler's settings. A local timer failure or
shutdown sets the connection sensor to OFF immediately.

The existing `on_press` and `on_release` binary-sensor automations can be used
to update a display or react to recovery and connection loss.
