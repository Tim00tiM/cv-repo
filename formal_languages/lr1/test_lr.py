import pytest
import sys
from main import *

def dry(**kwargs):
    forbidden = [" ", "\n"]
    new_str = ""
    if "string" in kwargs:
        string = kwargs["string"]
    if "array" in kwargs:
        string = "".join(kwargs["array"])
    for i in string:
        if i not in forbidden:
            new_str += i
    return new_str


def test_lr(capsys):
    tests = ["xd", "ddx", "incorrect_left_part", "incorrect_right_part", "arifm"]
    for i in tests:
        sys.stdin = open("tests/" + i, "r")
        tackle_one_input("tests/" + i)
        captured_output = capsys.readouterr()
        with open(f"tests/{i}.ans", "r") as answers:
            assert dry(array=answers.readlines()) == dry(string=captured_output.out)
