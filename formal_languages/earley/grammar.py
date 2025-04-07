class Rule:
    def __init__(self, **kwargs):
        if "rule" in kwargs:
            self.left_part = kwargs["rule"].left_part
            self.right_part = kwargs["rule"].right_part
        elif "left_part" in kwargs:
            self.left_part = kwargs["left_part"]
            if "right_part" in kwargs:
                self.right_part = kwargs["right_part"]
            else:
                self.right_part = ""
        else:
            self.left_part = ""
            self.right_part = ""

    def __eq__(self, other):
        return self.right_part == other.right_part and self.left_part == other.left_part

    def __repr__(self):
        return f"{self.left_part}->{self.right_part}"


class Grammar:
    def __init__(self):
        self.nonterminals = []
        self.terminals = []
        self.rules = []
        self.starting_symbol = "S"

    def __repr__(self):
        return self.rules.__repr__()
