from tests.testing_helpers import ComponentManifestOverride


def override_manifest(manifest: ComponentManifestOverride) -> None:
    # These tests exercise header-only helpers. The hardware driver requires
    # ESP32/ESP8266 timer implementations that are unavailable on the host.
    manifest.resources = [
        resource
        for resource in manifest.resources
        if resource.resource in ("connection_status.h", "opentherm_macros.h")
    ]
