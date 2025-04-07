import os
import sys

from LR import LR
from parsing import parse_grammar, parse_test_data

kTrailing_Equals = 20


def tackle_one_input(string: str):
    lr_algo = LR()
    try:
        grammar = parse_grammar()
    except ValueError:
        print("Incorrect grammar")
        return
    os.makedirs("logs", exist_ok=True)
    file = open("logs/" + string.split("/")[-1], "w")
    lr_algo.fit(grammar, file)
    for step, i in enumerate(parse_test_data(), start=1):
        file.write("=" * kTrailing_Equals + f"Test case #{step}" + "=" * kTrailing_Equals + "\n")
        file.write(f"Word: {i}\n")
        if res := lr_algo.predict(i, file):
            print("Yes")
        else:
            print("No")
        file.write(f"Result: {'Yes' if res else 'No'}\n\n")


def main():
    if sys.argv[1] == "files":
        for step, i in enumerate(sys.argv[2:], start=1):
            print(f"Test #{step} ({i})")
            sys.stdin = open(i, "r")
            tackle_one_input(i)
    elif sys.argv[1] == "console":
        tackle_one_input("console")


if __name__ == '__main__':
    main()
