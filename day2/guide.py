#!/usr/bin/env python3

import sys

# States (match legend).
ROCK = 0
PAPER = 1
SCISSORS = 2

# Input code lookup.
LOOKUP = {
    "A": ROCK,
    "B": PAPER,
    "C": SCISSORS,
    "X": ROCK,
    "Y": PAPER,
    "Z": SCISSORS,
}

# Score part 1.
LOSE = 0
DRAW = 3
WIN = 6

# Score part 2.
SCORES = {
    ROCK: 1,
    PAPER: 2,
    SCISSORS: 3,
}

# Lookup table with columns =>
# Rows: You play
# Cols: Opponent plays (Rock, Paper, Scissors)
LEGEND = {
    ROCK: [DRAW, LOSE, WIN],
    PAPER: [WIN, DRAW, LOSE],
    SCISSORS: [LOSE, WIN, DRAW],
}

def main():
    score = 0
    for line in sys.stdin:
        line = line.strip()
        opp_in, you_in = line.split(" ")
        opp_move = LOOKUP[opp_in]
        you_move = LOOKUP[you_in]
        score += LEGEND[you_move][opp_move]
        score += SCORES[you_move]

    print(f"You scored {score} total points.")

if __name__ == "__main__":
    main()
