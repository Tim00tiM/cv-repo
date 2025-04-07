import os
import sys
# from tempfile import TemporaryFile

from earley import Earley
from parsing import earley_parse_grammar, parse_test_data

kTrailing_Equals = 20


def tackle_one_input(string: str):
    erly = Earley()
    try:
        grammar = earley_parse_grammar()
    except ValueError:
        print("Incorrect grammar")
        return
    erly.fit(grammar)
    os.makedirs("logs", exist_ok=True)
    file = open("logs/" + string.split("/")[-1], "w")
    for step, i in enumerate(parse_test_data(grammar), start=1):
        file.write("=" * kTrailing_Equals + f"Test case #{step}" + "=" * kTrailing_Equals + "\n")
        file.write(f"Word: {i}\n")
        if res := erly.predict(i, file):
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
