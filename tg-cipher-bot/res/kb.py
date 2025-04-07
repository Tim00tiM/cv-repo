from aiogram.types import InlineKeyboardButton, InlineKeyboardMarkup

menu = [
    [InlineKeyboardButton(text="Зашифровать", callback_data="cipher"),
     InlineKeyboardButton(text="Расшифровать", callback_data="decipher")]
]
encrypt = [
    [InlineKeyboardButton(text="Шифр Цезаря", callback_data="caesar_cipher"),
     InlineKeyboardButton(text="Шифр Виженера", callback_data="vigenere_cipher")],
    [InlineKeyboardButton(text="Шифр Вернама", callback_data="vernam_cipher")]
]
decrypt = [
    [InlineKeyboardButton(text="Шифр Цезаря", callback_data="caesar_decipher"),
     InlineKeyboardButton(text="Шифр Виженера", callback_data="vigenere_decipher")],
    [InlineKeyboardButton(text="Шифр Вернама", callback_data="vernam_decipher")]
]
decrypt_caesar = [
    [InlineKeyboardButton(text="Знаю", callback_data="caesar_fixed"),
     InlineKeyboardButton(text="Не знаю", callback_data="caesar_brute")],
]
menu = InlineKeyboardMarkup(inline_keyboard=menu)
encrypt = InlineKeyboardMarkup(inline_keyboard=encrypt)
decrypt = InlineKeyboardMarkup(inline_keyboard=decrypt)
decrypt_caesar = InlineKeyboardMarkup(inline_keyboard=decrypt_caesar)