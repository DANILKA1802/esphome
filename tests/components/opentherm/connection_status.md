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

## Missing boiler readings

On connection loss, all configured OpenTherm numeric sensor readings are
published as `NaN`; binary readings such as flame, fault and CH activity have
their state invalidated. The connection sensor itself remains OFF. This also
works when the optional `connected` entity is not configured.

Home Assistant can then show the readings as unknown instead of keeping an old
temperature or reporting that the flame is OFF. This is a per-reading unknown
state, not a disconnection of the ESPHome device: Wi-Fi, the API and the climate
entity can still work.

Readings become valid individually when their next matching, positive reply
arrives. The initial query sequence runs again after communication recovers, so
startup-only readings also receive fresh values. A negative reply invalidates
only the readings associated with that message; its payload is never published
as measurement data. Unrelated replies and echoed requests are discarded.

Control entities (`number`, `switch`, `output`) and room-temperature inputs keep
their values. They represent requested settings or external inputs, not fresh
measurements from the boiler. Numeric invalidation passes through the configured
sensor filters, so filters should not discard or replace `NaN` if this behavior
is required.

`connected` measures OpenTherm communication, independently of Wi-Fi, the
Home Assistant API, boiler faults, flame state, and heating demand. It does not
send reset commands or change the boiler's settings. A local timer failure or
shutdown sets the connection sensor to OFF immediately.

The existing `on_press` and `on_release` binary-sensor automations can be used
to update a display or react to recovery and connection loss.
