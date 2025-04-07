from .Cipher import *


class CaesarCipher(Cipher):
    russian_frequency = {"о": 0.10983, "е": 0.08483, "а": 0.07998, "и": 0.07367, "н": 0.067, "т": 0.06318, "с": 0.05473,
                         "р": 0.04746, "в": 0.04533, "л": 0.04343, "к": 0.03486, "м": 0.03203, "д": 0.02977,
                         "п": 0.02804, "у": 0.02615, "я": 0.02001, "ы": 0.01898, "ь": 0.01735, "г": 0.01687,
                         "з": 0.01641, "б": 0.01592, "ч": 0.0145, "й": 0.01208, "х": 0.00966, "ж": 0.0094, "ш": 0.00718,
                         "ю": 0.00639, "ц": 0.00486, "щ": 0.00361, "э": 0.00331, "ф": 0.00267, "ъ": 0.00037,
                         "ё": 0.00013}

    english_frequency = {'e': 0.1200, 't': 0.0910, 'a': 0.0812, 'o': 0.0768,
                         'i': 0.0731, 'n': 0.0695, 's': 0.0628, 'r': 0.0602,
                         'h': 0.0592, 'd': 0.0432, 'l': 0.0398, 'u': 0.0288,
                         'c': 0.0271, 'm': 0.0261, 'f': 0.0230, 'y': 0.0211,
                         'w': 0.0209, 'g': 0.0203, 'p': 0.0182, 'b': 0.0149,
                         'v': 0.0111, 'k': 0.0069, 'x': 0.0017, 'q': 0.0011,
                         'j': 0.0010, 'z': 0.0007}

    @staticmethod
    def calculate_freq_difference(comp_freq: dict) -> float:
        """
        Calculate difference in frequencies between comp_freq and baseline frequency
        :param comp_freq:
        :return:
        """
        ret_float = 0.
        for i in comp_freq:
            if i in Cipher._en_lowercase:
                ret_float += abs(comp_freq[i] - CaesarCipher.english_frequency[i])
            if i in Cipher._ru_lowercase:
                ret_float += abs(comp_freq[i] - CaesarCipher.russian_frequency[i])
        return ret_float

    @staticmethod
    def get_frequency(message: str) -> dict:
        """
        Calculate frequency of supported symbols in message
        :param message:
        :return:
        """
        freq = dict()
        cnt = 0
        low_message = message.lower()
        for i in low_message:
            if i in Cipher._ru_lowercase or i in Cipher._en_lowercase:
                if i in freq:
                    freq[i] += 1
                else:
                    freq[i] = 1
                cnt += 1
        for j in Cipher._ru_lowercase:
            if j in freq:
                freq[j] = freq[j] / cnt
            else:
                freq[j] = 0
        for j in Cipher._en_lowercase:
            if j in freq:
                freq[j] = freq[j] / cnt
            else:
                freq[j] = 0
        return freq

    @staticmethod
    def make_shift(message: str, shift: int) -> str:
        """
        Move message to shift positions right.
        :param message:
        :param shift:
        :return:
        """
        ret_message = ""
        for i in message:
            if i in Cipher._en_letters:
                ret_message += Cipher.shift(i, shift, Cipher._en_lowercase)
            elif i in Cipher._ru_letters:
                ret_message += Cipher.shift(i, shift, Cipher._ru_lowercase)
            else:
                ret_message += i
        return ret_message

    def encrypt(self, file_path: str, shift: int,
                ext: str = ".cc", **kwargs: str) -> None:
        """
        File should contain only ru, en letters and punctuation.\n
        If file have name file.ext output file will have name file.cc.ext
        :param file_path: path to file
        :param shift: shift of cipher
        :param ext: extension of file if "``output_file_name``" not specified
        :param kwargs: supports "``output_file_name``" param,
         that overrides standard output file name
        :return:
        """
        encrypted = ""
        with open(file_path) as content:
            while True:
                i = content.read(1)
                if not i:
                    break
                if i in self._en_letters:
                    encrypted += self.shift(i, shift, self._en_lowercase)
                    continue
                if i in self._ru_letters:
                    encrypted += self.shift(i, shift, self._ru_lowercase)
                    continue
                encrypted += i
        if "output_file_name" in kwargs:
            output_name = kwargs["output_file_name"]
        else:
            name, extension = os.path.splitext(file_path)
            output_name = name + ext + extension
        self.output(output_name, encrypted)

    def decrypt(self, file_path: str, shift: int,
                ext: str = ".dc", **kwargs: str) -> None:
        """
        File should contain only ru, en letters and punctuation.\n
        If file was encoded with shift n, it will be decoded with shift n.\n
        If file have name file.ext output file will have name file.dc.ext
        :param file_path: path to file
        :param shift: shift of cipher
        :param ext: extension of file if "``output_file_name``" not specified
        :param kwargs: supports "``output_file_name``" param,
         that overrides standard output file name
        :return:
        """
        if "output_file_name" in kwargs:
            self.encrypt(file_path, -shift, ext,
                         output_file_name=kwargs["output_file_name"])
        else:
            self.encrypt(file_path, -shift, ext)

    def brute_force(self, filename: str, output: str, results: int = 1) -> None:
        """
        Try to decipher filename with results best matches. Write all to output
        :param filename:
        :param output:
        :param results:
        :return:
        """
        brute_force_strings = []
        message = ""
        with open(filename, 'r') as to_break:
            for i in to_break.readlines():
                message += i
        for i in range(0, 33):
            shifted_message = CaesarCipher.make_shift(message, -i)
            shifted_freq = CaesarCipher.get_frequency(shifted_message)
            shifted_freq_difference = CaesarCipher.calculate_freq_difference(shifted_freq)
            brute_force_strings.append(tuple([shifted_freq_difference, shifted_message]))
        brute_force_strings.sort(key=lambda x:x[0])
        true_strings = [brute_force_strings[0]]
        for i in range(1, len(brute_force_strings)):
            if brute_force_strings[i][1] != brute_force_strings[i - 1][1]:
                true_strings.append(brute_force_strings[i])
        with open(output, 'w+') as deciphered_text:
            for j in range(results):
                deciphered_text.write(true_strings[j][1])
                deciphered_text.write("\n\n")

