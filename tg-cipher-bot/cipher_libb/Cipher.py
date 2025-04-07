import os


class Cipher:

    _en_lowercase = list(map(chr, range(ord('a'), ord('z') + 1)))
    _en_uppercase = list(map(chr, range(ord('A'), ord('Z') + 1)))
    _en_letters = _en_lowercase + _en_uppercase
    _ru_lowercase = list("абвгдеёжзийклмнопрстуфхцчшщъыьэюя")
    _ru_uppercase = list("абвгдеёжзийклмнопрстуфхцчшщъыьэюя".upper())
    _ru_letters = _ru_lowercase + _ru_uppercase

    _supported_symbols = _en_letters + _ru_letters

    def output(self, filename: str, output_string: str) -> None:
        """
        :param filename: path to file to write
        :param output_string: string to write
        :return: None
        """

        with open(filename, "w+") as file:
            file.write(output_string)

    @staticmethod
    def shift(letter: str, shift: int, possible_chars: list) -> str:
        """
        shifts ``letter`` for ``shift`` positions
        :param letter:
        :param shift:
        :param possible_chars: ordered list of possible chars
        :return:
        """
        sign = 1 if shift > 0 else -1
        editable = letter.lower()
        editable_number = possible_chars.index(editable)
        editable_number += shift
        while editable_number < 0 or editable_number >= len(possible_chars):
            editable_number -= sign * len(possible_chars)
        if letter.islower():
            return possible_chars[editable_number]
        return possible_chars[editable_number].upper()
