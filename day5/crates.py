#!/usr/bin/env python3

import re
import sys

# Example.
# crates = [
#     ["Z", "N"],
#     ["M", "C", "D"],
#     ["P"],
# ]

# Part 1.
crates = [
    [*"GDVZJSB"],
    [*"ZSMGVP"],
    [*"CLBSWTQF"],
    [*"HJGWMRVQ"],
    [*"CLSNFMD"],
    [*"RGCD"],
    [*"HGTRJDSQ"],
    [*"PFV"],
    [*"DRSTJ"],
]

def execute_move(num_containers, from_loc, to_loc):
    # Change to zero-indexed.
    from_loc -= 1
    to_loc -= 1
    
    # Take up to n from FROM container
    containers = crates[from_loc][-num_containers:]
    crates[from_loc] = crates[from_loc][:-len(containers)] or []

    # Reverse extend TO container.
    for c in reversed(containers):
        crates[to_loc].append(c)

def main():
    for line in sys.stdin:
        line = line.strip()
        # Parse move.
        move = [int(s) for s in re.findall("\d+", line)]
        execute_move(*move)
    
    final_key = ""
    for stack in crates:
        final_key += stack[-1]
    
    print(f"Final key: {final_key}")


if __name__ == "__main__":
    main()
