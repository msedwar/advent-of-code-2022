#!/usr/bin/env python3

import sys

from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Callable, List, Optional, Tuple

MAX_TOTAL_SIZE = 100000

def get_indent(indent: int) -> str:
    out_str = ""
    for _ in range(indent):
        out_str += "  "
    return out_str

@dataclass
class FileNode(ABC):
    name: str
    size: int

    @abstractmethod
    def print_node(self, indent: int) -> str:
        pass

    def __repr__(self) -> str:
        return f"({self.name}: {self.size})"

@dataclass(repr=False)
class File(FileNode):
    def print_node(self, indent: int) -> str:
        return f"{get_indent(indent)}{self}\n"

@dataclass(repr=False)
class Directory(FileNode):
    parent: Optional['Directory']
    children: List['FileNode']

    def print_node(self, indent: int) -> str:
        print_str = f"{get_indent(indent)}{self}\n"
        for child in self.children:
            print_str += child.print_node(indent + 1)
        return print_str

def get_next_line() -> str:
    return sys.stdin.readline().strip()

def parse_ls_entry(input_str: str, current: Directory) -> FileNode:
    ty, name = input_str.split(" ")
    
    if ty == "dir":
        return Directory(name, 0, current, [])
    else:
        return File(name, int(ty))

def parse_command(input_str: str, current: Optional[Directory]) -> Tuple[Directory, str]:
    input_args = input_str.split(" ")
    assert input_args[0] == "$"
    assert input_args[1] == "cd" or input_args[1] == "ls"

    if input_args[1] == "cd":
        path = input_args[2]
        if path == "..":
            assert current is not None
            assert current.parent is not None
            return (current.parent, get_next_line())
        elif path == "/":
            assert current is None
            return (Directory("/", 0, current, []), get_next_line())
        else:
            assert current is not None
            for child in current.children:
                if child.name == path:
                    assert isinstance(child, Directory)
                    return (child, get_next_line())
            assert False

    else: # ls
        assert current is not None
        next_line = get_next_line()
        while len(next_line) > 0 and not next_line.startswith("$"):
            current.children.append(parse_ls_entry(next_line, current))
            next_line = get_next_line()
        return (current, next_line)

def fixup_tree_sizes(node: Directory):
    total_size = 0
    for child in node.children:
        if isinstance(child, Directory):
            fixup_tree_sizes(child)
        total_size += child.size
    node.size = total_size

def collect_directories(current: Directory, predicate: Callable[[Directory], bool], directories: List[Directory]):
    if predicate(current):
        directories.append(current)
    for child in current.children:
        if isinstance(child, Directory):
            collect_directories(child, predicate, directories)

def less_than_max(node: Directory) -> bool:
    return node.size <= MAX_TOTAL_SIZE

def main():
    root: Optional[Directory] = None
    current: Optional[Directory] = None
    next_line = get_next_line()
    while True:
        if len(next_line) == 0:
            break
        current, next_line = parse_command(next_line, current)
        if current.parent is None:
            root = current
    
    fixup_tree_sizes(root)
    directories: List[Directory] = []
    collect_directories(root, less_than_max, directories)

    total = 0
    for directory in directories:
        total += directory.size
    print(f"Total size: {total}.")

if __name__ == "__main__":
    main()
