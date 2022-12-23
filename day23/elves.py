#!/usr/bin/env python3

import sys

from dataclasses import dataclass
from typing import List, Set, Tuple

def getMoves(round, x, y):
    northMoves = set([(x - 1, y - 1), (x, y - 1), (x + 1, y - 1)])
    southMoves = set([(x - 1, y + 1), (x, y + 1), (x + 1, y + 1)])
    westMoves = set([(x - 1, y + 1), (x - 1, y), (x - 1, y - 1)])
    eastMoves = set([(x + 1, y + 1), (x + 1, y), (x + 1, y - 1)])
    moves = [(northMoves, (x, y - 1)), (southMoves, (x, y + 1)), (westMoves, (x - 1, y)), (eastMoves, (x + 1, y))]
    round = round % 4
    return moves[round:] + moves[:round]


@dataclass
class Elf:
    x: int
    y: int

    def getProposedMove(self, round: int, coordMap: Set[Tuple[int, int]]) -> Tuple[int, int]:
        x = self.x
        y = self.y

        allMoves = set([(x - 1, y - 1), (x, y - 1), (x + 1, y - 1), (x - 1, y), (x + 1, y), (x - 1, y + 1), (x, y + 1), (x + 1, y + 1)])
        if not (coordMap & allMoves):
            return (x, y)
        
        moves = getMoves(round, x, y)
        for (checks, move) in moves:
            if not (coordMap & checks):
                return move
        
        return (x, y)


def createCoordMap(elves: List[Elf]) -> Set[Tuple[int, int]]:
    coords: Set[Tuple[int, int]] = set()
    for elf in elves:
        coords.add((elf.x, elf.y))
    return coords

def doRound(round: int, elves: List[Elf]):
    coordMap = createCoordMap(elves)

    # Get proposed moves.
    proposedCoords = set()
    proposedCoordMap = {}
    for elf in elves:
        move = elf.getProposedMove(round, coordMap)
        if move not in proposedCoords:
            proposedCoords.add(move)
            proposedCoordMap[move] = elf
        elif move in proposedCoordMap:
            del proposedCoordMap[move]
    
    # Move elves.
    moves = 0
    for move, elf in proposedCoordMap.items():
        if move[0] != elf.x or move[1] != elf.y:
            moves += 1
            elf.x = move[0]
            elf.y = move[1]
    
    return moves > 0

def getBounds(elves: List[Elf]) -> Tuple[int, int, int, int]:
    minX = 0
    maxX = 0
    minY = 0
    maxY = 0
    for elf in elves:
        if elf.x < minX:
            minX = elf.x
        if elf.x > maxX:
            maxX = elf.x
        if elf.y < minY:
            minY = elf.y
        if elf.y > maxY:
            maxY = elf.y
    
    return (minX, maxX, minY, maxY)

def main():
    elves: List[Elf] = []
    row = 0
    for line in sys.stdin:
        line = line.strip()
        for i, c in enumerate(line):
            if c == "#":
                elves.append(Elf(i, row))
        row += 1

    round = 0
    while doRound(round, elves):
        round += 1
    
    print(f"No elves moved in round {round + 1}")
        

if __name__ == "__main__":
    main()
