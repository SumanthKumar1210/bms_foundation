import math


def calculate_soh(measured_capacity: float, rated_capacity: float) -> float:
    """Calculate battery state of health as a percentage.

    State of health is measured capacity divided by rated capacity. Both
    capacity values must use the same units. Measured capacity must be
    non-negative; values above rated capacity are allowed.
    """
    if not math.isfinite(measured_capacity) or not math.isfinite(rated_capacity):
        raise ValueError("Capacity values must be finite.")
    if rated_capacity <= 0:
        raise ValueError("Rated capacity must be greater than zero.")
    if measured_capacity < 0:
        raise ValueError("Measured capacity must be non-negative.")

    return measured_capacity / rated_capacity * 100
