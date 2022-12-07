#!/usr/bin/env python3

def main():
    data = input("? ")
    assert len(data) >= 4
    
    for index in range(0, len(data) - 3):
        buf = data[index:index + 4]
        if len(set(buf)) == 4:
            break

    print(f"Signal position: {index + 4}")


if __name__ == "__main__":
    main()
