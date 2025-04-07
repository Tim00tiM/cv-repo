from typing import Literal
import cipher_libb as cl
import tempfile
from pathlib import Path
import os
from aiogram.types.document import Document
from aiogram.types import FSInputFile


class Facade:

    @staticmethod
    async def vernam_message_crypt(message: Document, key: Document) -> FSInputFile:
        """
        facade for cipher_lib function
        :param message: Document to encrypt
        :param key: key to encrypt message
        :return:
        """
        vernam = cl.VernamCipher()
        temp_message = tempfile.NamedTemporaryFile(delete=False)
        temp_key = tempfile.NamedTemporaryFile(delete=False)
        temp_output = tempfile.NamedTemporaryFile(delete=False)
        await message.bot.download(message.file_id, destination=temp_message.name)
        await message.bot.download(key.file_id, destination=temp_key.name)
        vernam.encrypt(file_path=temp_message.name, note_path=temp_key.name, output_file_name=temp_output.name)
        temp_message.close()
        temp_key.close()
        os.remove(temp_message.name)
        os.remove(temp_key.name)
        return FSInputFile(temp_output.name)

    @staticmethod
    def caesar_message_crypt(message: str, shift: int, method: Literal["fixed", "brute", "encrypt"]) -> str:
        """
        facade for cipher_lib function
        :param message: message to manipulate
        :param shift: shift or amount of shifts for brute force method
        :param method: caesar type
        :return:
        """
        if method == "fixed":
            return cl.CaesarCipher.make_shift(message, -shift)
        if method == "encrypt":
            return cl.CaesarCipher.make_shift(message, shift)
        if method == "brute":
            caesar = cl.CaesarCipher()
            with tempfile.NamedTemporaryFile(mode="w+") as temp:
                temp.write(message)
                temp.flush()
                a = Path(temp.name)
                caesar.brute_force(temp.name, output=(a.parent / "bruted").__str__(), results=shift)
                with open(a.parent / "bruted", mode="r") as bruted:
                    text = bruted.read()
                os.remove(a.parent / "bruted")
            return text


    @staticmethod
    def vigenere_message_crypt(message: str, key: str, method: Literal["encrypt", "decrypt"]) -> str:
        """
        facade for cipher_lib function
        :param message: message to manipulate
        :param key: key used in ciphering/deciphering
        :param method: vigenere type
        :return:
        """
        vigenere = cl.VigenereCipher()
        with tempfile.NamedTemporaryFile(mode="w+") as temp:
            temp.write(message)
            temp.flush()
            a = Path(temp.name)
            if method == "decrypt":
                vigenere.decrypt(temp.name, key, output_file_name=(a.parent / "vigenere_crypt").__str__())
            if method == "encrypt":
                vigenere.encrypt(temp.name, key, output_file_name=(a.parent / "vigenere_crypt").__str__())
            with open(a.parent / "vigenere_crypt", mode="r") as crypted:
                text = crypted.read()
            os.remove(a.parent / "vigenere_crypt")
        return text
