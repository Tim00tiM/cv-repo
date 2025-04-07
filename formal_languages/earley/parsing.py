from grammar import Grammar, Rule
import os
import sys


def earley_parse_grammar(file: str = ""):
    non_terminal_size, alphabet_size, rules_count = map(int, input().split())
    grammar = Grammar()
    non_terminals = input()
    grammar.nonterminals = non_terminals
    terminals = input()
    grammar.terminals = terminals

    for i in range(rules_count):
        rule = input()
        arr = rule.split("->")
        if len(arr[0].strip()) > 1:
            raise ValueError
        for j in arr[0].strip():
            if j not in grammar.terminals and j not in grammar.nonterminals:
                raise ValueError
        if len(arr) == 1 or arr[1].strip() == "":
            grammar.rules.append(Rule(left_part=arr[0].strip()))
        else:
            for j in arr[1].strip():
                if j not in grammar.terminals and j not in grammar.nonterminals:
                    raise ValueError
            grammar.rules.append(Rule(left_part=arr[0].strip(), right_part=arr[1].strip()))
    start_symbol = input()
    grammar.starting_symbol = start_symbol
    return grammar


def parse_test_data(grammar: Grammar):
    n = int(input())
    arr = []
    for i in range(n):
        temp = input()
        for j in temp:
            if j not in grammar.terminals:
                raise ValueError("incorrect symbols in word")
        arr.append(temp)
    return arr