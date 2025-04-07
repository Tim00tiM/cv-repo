from .CaesarCipher import *


class VigenereCipher(Cipher):

    @staticmethod
    def vigenere_shift(letter: str, key_char: str,
                       possible_chars: list, sign: int) -> str:
        """
        Match letter and key_char in Vigenere cipher way.
        :param letter: to cipher
        :param key_char: ciphering char
        :param possible_chars: alphabet of ciphering
        :param sign: encode or decode
        :return:
        """
        lower_shift = key_char.lower()
        try:
            int_shift = possible_chars.index(lower_shift)
        except ValueError:
            return letter
        return Cipher.shift(letter, sign * int_shift, possible_chars)

    def encrypt(self, file_path: str, key: str, sign: int = 1,
                ext: str = ".cvg", **kwargs: str) -> None:
        """
        File should contain only ru or only en letters and punctuation.\n
        Choice of language is based on key language\n
        If file have name file.ext output file will have name file.cvg.ext
        :param file_path: path to file
        :param key: key of cipher
        :param sign: do not touch
        :param ext: extension of file if "``output_file_name``" not specified
        :param kwargs: supports "``output_file_name``" param,
         that overrides standard output file name
        :return:
        """
        encrypted = ""
        key_pos = 0
        with open(file_path) as content:
            while True:
                i = content.read(1)
                if not i:
                    break
                if i in self._en_letters:
                    encrypted += self.vigenere_shift(i, key[key_pos],
                                                     self._en_lowercase, sign)
                    key_pos = (key_pos + 1) % len(key)
                    continue
                if i in self._ru_letters:
                    encrypted += self.vigenere_shift(i, key[key_pos],
                                                     self._ru_lowercase, sign)
                    key_pos = (key_pos + 1) % len(key)
                    continue
                encrypted += i
        if "output_file_name" in kwargs:
            output_name = kwargs["output_file_name"]
        else:
            name, extension = os.path.splitext(file_path)
            output_name = name + ext + extension
        self.output(output_name, encrypted)

    def decrypt(self, file_path: str, key: str,
                ext: str = ".dvg", **kwargs: str) -> None:
        """
        File should contain only ru or only en letters and punctuation.\n
        Choice of language is based on key language\n
        If file was encoded with key "``sample``",
         it can be decoded with key "``sample``".\n
        If file have name file.ext output file will have name file.dvg.ext
        :param file_path: path to file
        :param key: key of cipher
        :param ext: extension of file if "``output_file_name``" not specified
        :param kwargs: supports "``output_file_name``" param,
         that overrides standard output file name
        :return:
        """
        if "output_file_name" in kwargs:
            self.encrypt(file_path, key, -1, ext,
                         output_file_name=kwargs["output_file_name"])
        else:
            self.encrypt(file_path, key, -1, ext)
