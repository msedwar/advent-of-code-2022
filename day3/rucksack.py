#!/usr/bin/env python3

import sys

def get_score(letter):
    ascii_value = ord(letter)
    if ascii_value >= ord("a"):
        return ascii_value - ord("a") + 1
    else:
        return ascii_value - ord("A") + 27

def find_common(comp1, comp2):
    letters = set(comp1)
    for c in comp2:
        if c in letters:
            return c
    return None

def main():
    total = 0
    for line in sys.stdin:
        line = line.strip()
        half = len(line)//2
        comp1, comp2 = line[:half], line[half:]
        common = find_common(comp1, comp2)
        total += get_score(common)
    
    print(f"Total priorities of: {total}")

if __name__ == "__main__":
    main()
