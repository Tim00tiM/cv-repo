import os

from grammar import Grammar, Rule
from typing import TextIO


class Earley:
    class Situation(Rule):
        pass

    pass


def epsilon_generative(grammar: Grammar):
    epsilon_list = []
    for i in grammar.rules:
        if i.right_part == "":
            epsilon_list.append(i.left_part)
    first_time = True
    changes = False
    while changes or first_time:
        changes = False
        first_time = False
        for i in grammar.rules:
            if i.left_part in epsilon_list:
                continue
            broken = False
            for j in i.right_part:
                if j not in epsilon_list:
                    broken = True
                    break
            if not broken:
                epsilon_list.append(i.left_part)
                changes = True
    return epsilon_list


def log_step(changes: list[Earley.Situation], step: int, name: str, file: TextIO):
    if len(changes) == 0:
        return
    file.write(f"found {len(changes)} new sitations via \"{name[0].upper() + name[1:]}\" at step {step}:\n")
    for i in range(len(changes)):
        file.write(f"{changes[i]}\n")


class Earley:
    class Situation(Rule):
        def __init__(self, rule: Rule, start_pos: int = -1, current_pos: int = -1):
            super().__init__(rule=rule)
            self.start_pos = start_pos
            self.current_pos = current_pos

        def __eq__(self, other):
            return (self.current_pos == other.current_pos and
                    self.start_pos == other.start_pos and
                    self.left_part == other.left_part and
                    self.right_part == other.right_part)

        def __repr__(self):
            ret = f"{self.left_part}->{self.right_part}"
            ret = (ret[:len(self.left_part) + 2 + self.current_pos] +
                   "." +
                   ret[len(self.left_part) + 2 + self.current_pos:])
            return ret

    def __init__(self):
        self.grammar = Grammar()
        self.epsilon_list = []
        self.starting_rule = Rule()

    def fit(self, grammar: Grammar):
        self.grammar = grammar
        self.epsilon_list = epsilon_generative(grammar)
        # grammar check have been done in parsing
        self.starting_rule = Rule(left_part="$", right_part=grammar.starting_symbol)
        self.grammar.rules.append(self.starting_rule)

    def scan(self, dynamic_steps: list[list[Situation]], step: int, symbol: str):
        new_changes = []
        for i in dynamic_steps[step - 1]:
            if i.current_pos == len(i.right_part):
                continue
            if i.right_part[i.current_pos] == symbol:
                new_situation = self.Situation(Rule(left_part=i.left_part, right_part=i.right_part), i.start_pos,
                                               i.current_pos + 1)
                new_changes.append(new_situation)
                dynamic_steps[step].append(new_situation)
        return new_changes

    def complete(self, dynamic_steps: list[list[Situation]], step: int, first_time: bool, changes: list[Situation]):
        new_changes = []
        if first_time:
            changes = dynamic_steps[step]
        for i in changes:
            if i.current_pos == len(i.right_part):
                for j in dynamic_steps[i.start_pos]:
                    if j.current_pos == len(j.right_part):
                        continue
                    if i.left_part != j.right_part[j.current_pos]:
                        continue
                    new_situation = self.Situation(Rule(right_part=j.right_part, left_part=j.left_part), j.start_pos,
                                                   j.current_pos + 1)
                    if new_situation not in dynamic_steps[step] and new_situation not in new_changes:
                        new_changes.append(new_situation)
                continue
            if i.right_part[i.current_pos] in self.epsilon_list:
                new_situation = self.Situation(Rule(right_part=i.right_part, left_part=i.left_part), i.start_pos,
                                               i.current_pos + 1)
                if new_situation not in dynamic_steps[step] and new_situation not in new_changes:
                    new_changes.append(new_situation)
        return new_changes

    def expand(self, dynamic_steps: list[list[Situation]], step: int, first_time: bool, changes: list[Situation]):
        new_changes = []
        if first_time:
            changes = dynamic_steps[step]
        for i in changes:
            if i.current_pos == len(i.right_part):
                continue
            if i.right_part[i.current_pos] in self.grammar.nonterminals:
                for j in self.grammar.rules:
                    if j.left_part == i.right_part[i.current_pos]:
                        new_situation = self.Situation(Rule(rule=j), step, 0)
                        if new_situation not in dynamic_steps[step] and new_situation not in new_changes:
                            new_changes.append(new_situation)
        return new_changes

    def closure(self, step: int, dynamic_steps: list[list[Earley.Situation]], file: TextIO) -> bool:
        changes = []
        first_time = True
        while first_time or len(changes) > 0:
            past_changes = changes
            complete_changes = self.complete(dynamic_steps, step, first_time, past_changes)
            expand_changes = self.expand(dynamic_steps, step, first_time, past_changes)
            changes = complete_changes + expand_changes
            log_step(complete_changes, step, "complete", file)
            log_step(expand_changes, step, "predict", file)
            dynamic_steps[step] += changes
            first_time = False

    def predict(self, string: str, file: TextIO):
        n = len(string)
        dynamic_steps = [[] for i in range(n + 1)]
        dynamic_steps[0].append(self.Situation(self.starting_rule, 0, 0))
        self.closure(0, dynamic_steps, file)
        for i in range(1, n + 1):
            scan_changes = self.scan(dynamic_steps, i, string[i - 1])
            log_step(scan_changes, i, "scan", file)
            self.closure(i, dynamic_steps, file)
        final_situation = self.Situation(self.starting_rule, 0, 1)
        return final_situation in dynamic_steps[n]
