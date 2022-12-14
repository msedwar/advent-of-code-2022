#!/usr/bin/env python3

import sys

def range_fully_contains(range1, range2):
    range1_lower = range1[0] <= range2[0]
    range1_higher = range1[1] >= range2[1]
    range2_lower = range2[0] <= range1[0]
    range2_higher = range2[1] >= range1[1]
    return range1_lower == range1_higher or range2_lower == range2_higher # Either range1 contains range 2, or vice versa.

def range_overlaps(range1, range2):
    if range1[0] <= range2[0]:
        lower_range = range1
        higher_range = range2
    else:
        lower_range = range2
        higher_range = range1
    
    return lower_range[1] >= higher_range[0]

def string_to_range(s):
    in1, in2 = s.split("-")
    return (int(in1), int(in2))

def main():
    total = 0
    for line in sys.stdin:
        line = line.strip()
        range_in1, range_in2 = line.split(",")
        range1 = string_to_range(range_in1)
        range2 = string_to_range(range_in2)
        if range_overlaps(range1, range2):
            total += 1
    
    print(f"{total} assignment ranges overlap.")


if __name__ == "__main__":
    main()
