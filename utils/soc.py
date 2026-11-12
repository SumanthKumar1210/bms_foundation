import math


def calculate_soc(remaining_capacity: float, total_capacity: float) -> float:
    """Calculate battery state of charge as a percentage.

    Both capacity values must use the same units. Remaining capacity must be
    between zero and total capacity, inclusive.
    """
    if not math.isfinite(remaining_capacity) or not math.isfinite(total_capacity):
        raise ValueError("Capacity values must be finite.")
    if total_capacity <= 0:
        raise ValueError("Total capacity must be greater than zero.")
    if remaining_capacity < 0 or remaining_capacity > total_capacity:
        raise ValueError("Remaining capacity must be between zero and total capacity.")

    return remaining_capacity / total_capacity * 100
