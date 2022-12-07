#!/usr/bin/env python3

MESSAGE_SIZE = 14

def main():
    data = input("? ")
    assert len(data) >= MESSAGE_SIZE
    
    for index in range(0, len(data) - MESSAGE_SIZE + 1):
        buf = data[index:index + MESSAGE_SIZE]
        if len(set(buf)) == MESSAGE_SIZE:
            break

    print(f"Signal position: {index + MESSAGE_SIZE}")


if __name__ == "__main__":
    main()
