#!/usr/bin/env python3

import sys

# States (match legend).
ROCK = 0
PAPER = 1
SCISSORS = 2

# Score part 1.
LOSE = 0
DRAW = 3
WIN = 6

# Input code lookup.
LOOKUP = {
    "A": ROCK,
    "B": PAPER,
    "C": SCISSORS,
    "X": LOSE,
    "Y": DRAW,
    "Z": WIN,
}

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

# Lookup table with columns =>
# Rows: Opp play
# Cols: Result (Lose, Draw, Win)
REVERSE_LEGEND = {
    ROCK: [SCISSORS, ROCK, PAPER],
    PAPER: [ROCK, PAPER, SCISSORS],
    SCISSORS: [PAPER, SCISSORS, ROCK],
}

def main():
    score = 0
    for line in sys.stdin:
        line = line.strip()
        opp_in, end_in = line.split(" ")
        opp_move = LOOKUP[opp_in]
        end_state = LOOKUP[end_in]
        score += end_state
        score += SCORES[REVERSE_LEGEND[opp_move][end_state // 3]]

    print(f"You scored {score} total points.")

if __name__ == "__main__":
    main()
