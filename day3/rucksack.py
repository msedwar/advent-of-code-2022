#!/usr/bin/env python3

import sys

def get_score(letter):
    ascii_value = ord(letter)
    if ascii_value >= ord("a"):
        return ascii_value - ord("a") + 1
    else:
        return ascii_value - ord("A") + 27

def find_common(comp_list):
    assert len(comp_list) >= 2
    letters = set(comp_list[0])
    for comp in comp_list[1:]:
        comp_set = set(comp)
        # Could also do this manually with a map and element counts.
        letters = letters.intersection(comp_set)
    
    assert len(letters) == 1
    return letters.pop()

def main():
    total = 0
    lines = []
    for line in sys.stdin:
        line = line.strip()
        lines.append(line)

        if len(lines) == 3:
            common = find_common(lines)
            total += get_score(common)
            lines.clear()
    
    print(f"Total priorities of: {total}")

if __name__ == "__main__":
    main()
