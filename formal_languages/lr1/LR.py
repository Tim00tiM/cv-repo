import copy
import os
from enum import Enum

from grammar import Grammar, Rule
from typing import TextIO
from graph import Graph, dfs_components, dfs


kStringEndSymbol = "$"
kRuleStartSymbol = "#"


class LR:

    class Action(Enum):
        ACCEPT = 1
        REJECT = 2
        SHIFT = 3
        REDUCE = 4

    class Situation(Rule):
        def __init__(self, rule: Rule, current_pos: int = -1, next_char: str = ""):
            super().__init__(rule=rule)
            self.current_pos = current_pos
            self.next_char = next_char

    grammar: Grammar
    table: list[dict[str, str]]
    goto: list[dict[str, int]]
    action: list[dict[str, str]]
    closures: list[list[Situation]]
    first: dict[str, set[str]]
    epsilon_list: list[str]

    def __init__(self):
        self.grammar = Grammar()
        self.table = []
        self.goto = []
        self.closures = []
        self.first = dict()
        self.epsilon_list = []


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


def calculate_firsts(lr_algo: LR):
    grammar = lr_algo.grammar
    epsilon_list = lr_algo.epsilon_list = epsilon_generative(grammar)
    grammar.terminals += kStringEndSymbol
    counter = 0
    str_to_int = dict()
    int_to_str = dict()
    for i in grammar.nonterminals + grammar.terminals:
        str_to_int[i] = counter
        int_to_str[counter] = i
        counter += 1
    graph = Graph(len(grammar.terminals) + len(grammar.nonterminals))
    for i in grammar.rules:
        current = 0
        while ((current == 0 or i.right_part[current - 1] in epsilon_list) and
               current < len(i.right_part)):
            if str_to_int[i.right_part[current]] not in graph.edges[str_to_int[i.left_part]]:
                graph.edges[str_to_int[i.left_part]].append(str_to_int[i.right_part[current]])
            current += 1
    components = []
    ids = [0 for i in range(len(graph.vertices))]
    correct_bypass = []
    dfs_components(graph, ids, components)
    component_graph = Graph(len(components))
    for j in graph.edges:
        for i in graph.edges[j]:
            if ids[j] != ids[i]:
                frm, to = sorted([ids[i], ids[j]])
                if to not in component_graph.edges[frm]:
                    component_graph.edges[frm].append(to)
    used = [False for _ in range(len(component_graph.vertices))]
    for i in range(len(component_graph.vertices)):
        dfs(i, ids, component_graph, correct_bypass, used)
    for i in correct_bypass:
        component_first = set()
        chars_of_component = [int_to_str[j] for j in components[i]]
        for j in chars_of_component:
            if j in grammar.terminals:
                component_first.add(j)
        for j in component_graph.edges[i]:
            component_first = component_first.union(lr_algo.first[int_to_str[components[j][0]]])
        for j in chars_of_component:
            lr_algo.first[j] = component_first
            if j in lr_algo.epsilon_list:
                lr_algo.first[j].add(kStringEndSymbol)


def first_for_symbols(symbols: str, lr_algo: LR) -> set[str]:
    ret = set()
    grammar = lr_algo.grammar
    for i in range(len(symbols)):
        if symbols[i] in grammar.terminals:
            ret.add(symbols[i])
            return ret
        ret = ret.union(lr_algo.first[symbols[i]])
        if symbols[i] not in lr_algo.epsilon_list:
            return ret
    ret.add(kStringEndSymbol)
    return ret


def closure(situations: list[LR.Situation], lr_algo: LR) -> list[LR.Situation]:
    first_time = True
    ret = situations
    grammar = lr_algo.grammar
    changed = situations
    new_changed = []
    while len(changed) > 0 or first_time:
        first_time = False
        for j in changed:
            if (len(j.right_part) == j.current_pos or
                    j.right_part[j.current_pos] not in grammar.nonterminals):
                continue
            first = first_for_symbols(j.right_part[j.current_pos + 1:] + j.next_char, lr_algo)
            for i in grammar.rules:
                if i.left_part != j.right_part[j.current_pos]:
                    continue
                for z in first:
                    new_situation = LR.Situation(Rule(left_part=i.left_part, right_part=i.right_part),
                                                 0, z)
                    if new_situation not in ret and new_situation not in new_changed:
                        new_changed.append(new_situation)
        ret += new_changed
        changed = new_changed
        new_changed = []
    return ret


def goto(closed_set: list[LR.Situation], char: str, lr_algo: LR):
    goto_set = []
    for i in closed_set:
        if i.right_part == "":
            continue
        if i.current_pos == len(i.right_part):
            continue
        if i.right_part[i.current_pos] == char:
            new_situation = copy.copy(i)
            new_situation.current_pos += 1
            goto_set.append(new_situation)
    return closure(goto_set, lr_algo)


class LR:
    grammar: Grammar
    table: list[dict[str, str]]
    goto: list[dict[str, int]]
    action: list[dict[str, list[tuple]]]
    closures: list[list[LR.Situation]]
    first: dict[str, set[str]]
    epsilon_list: list[str]

    class Action(Enum):
        ACCEPT = 1
        REJECT = 2
        SHIFT = 3
        REDUCE = 4

    class Situation(Rule):
        def __init__(self, rule: Rule, current_pos: int = -1, next_char: str = ""):
            super().__init__(rule=rule)
            self.current_pos = current_pos
            self.next_char = next_char

        def __eq__(self, other):
            return (self.current_pos == other.current_pos and
                    self.next_char == other.next_char and
                    self.left_part == other.left_part and
                    self.right_part == other.right_part)

        def __repr__(self):
            ret = f"{self.left_part}->{self.right_part}, {self.next_char}"
            ret = (ret[:len(self.left_part) + 2 + self.current_pos] +
                   "." +
                   ret[len(self.left_part) + 2 + self.current_pos:])
            return ret

    def __init__(self):
        self.grammar = Grammar()
        self.table = []
        self.goto = []
        self.action = []
        self.closures = []
        self.first = dict()
        self.epsilon_list = []
        self.starting_rule = Rule()

    def print_first(self, file: TextIO):
        file.write(f"First:\n")
        for i in self.first:
            file.write(f"{i}: {{{', '.join(self.first[i])}}}\n")
        file.write(f"\n")

    def print_closures(self, file: TextIO):
        file.write(f"Closures:\n")
        for i in range(len(self.closures)):
            file.write(f"{i}: {{{', '.join([('(' + j.__repr__() + ')') for j in self.closures[i]])}}}\n")
        file.write(f"\n")

    def print_gotos(self, file: TextIO, action_table: list[dict[str, str]]):
        file.write(f"Gotos:\n")
        digits_in_len = len(str(len(self.closures)))
        file.write(" " * digits_in_len + "   ")
        headers = ['{0: <{1}}'.format(i, 12 + digits_in_len) for i in action_table[0]]
        file.write(f"{''.join(headers)}")
        file.write(f"\n")
        for i in range(len(self.goto)):
            action = {j: action_table[i][j][:1].upper() + action_table[i][j][1:] for j in action_table[i]}
            line = ['{0: <{1}}'.format(action[j], 12 + digits_in_len) for j in action_table[i]]
            file.write(f"{i: >{len(str(len(self.closures)))}}   {''.join(line)}\n")
        file.write(f"\n")
        
    def calculate_gotos(self):
        changed = []
        new_changed = []
        first_time = True
        symbols = self.grammar.terminals + self.grammar.nonterminals
        new_situation = LR.Situation(self.starting_rule, 0, kStringEndSymbol)
        changed.append(closure([new_situation], self))
        self.closures += changed
        gotos = [dict()]
        current_goto = 0
        while len(changed) > 0 or first_time:
            first_time = False
            for i in changed:
                for j in symbols:
                    goto_one_step = goto(i, j, self)
                    if len(goto_one_step) < 1:
                        gotos[current_goto][j] = -1
                        continue
                    first_index = -2
                    second_index = -2
                    try:
                        first_index = self.closures.index(goto_one_step)
                    except:
                        first_index = -1
                    try:
                        second_index = new_changed.index(goto_one_step)
                    except:
                        second_index = -1
                    if first_index > -1:
                        gotos[current_goto][j] = first_index
                        continue
                    if second_index > -1:
                        gotos[current_goto][j] = len(self.closures) + second_index
                        continue
                    gotos[current_goto][j] = len(self.closures) + len(new_changed)
                    new_changed.append(goto_one_step)
                    gotos.append(dict())
                current_goto += 1
            self.closures += new_changed
            changed = new_changed
            new_changed = []
        self.goto = gotos

    def calculate_actions(self) -> list[dict[str, str]]:
        terminals = self.grammar.terminals
        nonterminals = self.grammar.nonterminals
        for i in range(len(self.closures)):
            self.action.append(dict())
            for j in terminals:
                self.action[i][j] = []
        final_situation = LR.Situation(self.starting_rule, 1, kStringEndSymbol)
        action_table = [dict() for i in range(len(self.closures))]
        for i in range(len(self.closures)):
            for j in terminals:
                for z in self.closures[i]:
                    if z.current_pos == len(z.right_part) and z.next_char == j:
                        self.action[i][j].append((LR.Action.REDUCE,
                                                  Rule(left_part=z.left_part, right_part=z.right_part)))
                        action_table[i][j] = f"reduce {len(z.right_part)} {z.left_part}"
                    if z.current_pos < len(z.right_part) and z.right_part[z.current_pos] == j:
                        if len(self.action[i][j]) == 1 and self.action[i][j][0][0] == LR.Action.SHIFT:
                            continue
                        self.action[i][j].append((LR.Action.SHIFT, self.goto[i][j]))
                        action_table[i][j] = f"shift {self.goto[i][j]}"
                if len(self.action[i][j]) > 1:
                    raise TypeError("Грамматика не LR(1)")
                if len(self.action[i][j]) == 0:
                    self.action[i][j].append((LR.Action.REJECT,))
                    action_table[i][j] = f"reject"
            if final_situation in self.closures[i]:
                action_table[i][kStringEndSymbol] = "accept"
                self.action[i][kStringEndSymbol] = [(LR.Action.ACCEPT,)]
        for i in range(len(self.closures)):
            for j in nonterminals:
                if self.goto[i][j] == -1:
                    action_table[i][j] = "reject"
                else:
                    action_table[i][j] = f"shift {self.goto[i][j]}"
        return action_table

    def fit(self, grammar: Grammar, file: TextIO):
        self.grammar = grammar
        self.starting_rule = Rule(left_part=kRuleStartSymbol, right_part=grammar.starting_symbol)
        calculate_firsts(self)
        self.calculate_gotos()
        action_table = self.calculate_actions()
        self.print_first(file)
        self.print_closures(file)
        self.print_gotos(file, action_table)

    def predict(self, string: str, file: TextIO) -> bool:
        string = string + kStringEndSymbol
        lrstack = [0]
        pos = 0
        file.write(f"Переходы по замыканиям:\n")
        while pos < len(string):
            q = lrstack[-1]
            action = self.action[q][string[pos]][0]
            if action[0] == LR.Action.SHIFT:
                file.write(f"{q} ->{string[pos]} ")
                lrstack.append(string[pos])
                pos += 1
                lrstack.append(action[1])
                continue
            if action[0] == LR.Action.REDUCE:
                file.write(f"{q}\n")
                for i in range(len(action[1].right_part) * 2):
                    lrstack.pop()
                q_new = lrstack[-1]
                file.write(f"reduce to {q_new}\n")
                A = action[1].left_part
                next_state = self.goto[q_new][A]
                if next_state == -1:
                    return False
                lrstack.append(A)
                lrstack.append(next_state)
                file.write(f"{q_new} ->{A} ")

            if action[0] == LR.Action.ACCEPT:
                file.write(f"{q}\n")
                return True
            if action[0] == LR.Action.REJECT:
                file.write(f"{q}\n")
                return False
