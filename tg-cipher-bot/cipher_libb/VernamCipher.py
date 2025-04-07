from .Cipher import *


class VernamCipher(Cipher):

    def encrypt(self, file_path: str, note_path: str,
                ext: str = ".cve", **kwargs: str) -> None:
        """
        Encrypt one file with another, another file should
         have size great or equal than first\n
        If file have name file.ext output file will have name file.cve.ext
        :param file_path: path to file
        :param note_path: path to note
        :param ext: extension of file if "``output_file_name``" not specified
        :param kwargs: supports "``output_file_name``" param,
         that overrides standard output file name
        :return:
        """
        if "output_file_name" in kwargs:
            output_name = kwargs["output_file_name"]
        else:
            name, extension = os.path.splitext(file_path)
            output_name = name + ext + extension
        with (open(file_path, "rb") as content,
              open(note_path, "rb") as note,
              open(output_name, "wb+") as result):
            bytes_to_write = bytearray()
            while True:
                i = content.read(1)
                j = note.read(1)
                if not i:
                    break
                bytes_to_write.append(ord(i) ^ ord(j))
            result.write(bytes_to_write)

    def decrypt(self, file_path: str, note_path: str,
                ext: str = ".dve", **kwargs: str) -> None:
        """
        Decrypt one file with another, another file should
         have size great or equal than first\n
        If file have name file.ext output file will have name file.dve.ext
        If file was encoded with file "``sample``",
         it can be decoded with file "``sample``" of same byte content.\n
        :param file_path: path to file
        :param note_path: path to note
        :param ext: extension of file if "``output_file_name``" not specified
        :param kwargs: supports "``output_file_name``" param,
         that overrides standard output file name
        :return:
        """

        if "output_file_name" in kwargs:
            self.encrypt(file_path, note_path, ext,
                         output_file_name=kwargs["output_file_name"])
        else:
            self.encrypt(file_path, note_path, ext)
