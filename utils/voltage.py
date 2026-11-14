import math


OVER_VOLTAGE_THRESHOLD = 650.0
UNDER_VOLTAGE_THRESHOLD = 350.0


def monitor_battery_voltage(voltage: float) -> tuple[bool, bool]:
    """Return over-voltage and under-voltage error flags, in that order.

    The over-voltage error is set above 650 V, and the under-voltage
    error is set below 350 V. Measurements at either threshold are in range.
    """
    if not math.isfinite(voltage):
        raise ValueError("Battery voltage must be finite.")

    over_voltage_error = voltage > OVER_VOLTAGE_THRESHOLD
    under_voltage_error = voltage < UNDER_VOLTAGE_THRESHOLD
    return over_voltage_error, under_voltage_error
