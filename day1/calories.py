#!/usr/bin/env python3

import heapq
import sys

def main():
    best_elves = []
    current_elf = 0
    for line in sys.stdin:
        line = line.strip()
        if len(line) == 0:
            heapq.heappush(best_elves, current_elf)
            current_elf = 0
        else:
            item = int(line)
            current_elf += item
    
    total = sum(heapq.nlargest(3, best_elves))
    print(f"Three highest calorie elves have {total} calories.")

if __name__ == "__main__":
    main()
