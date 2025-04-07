import os

from aiogram import F, Router
from aiogram.types import Message, CallbackQuery
from aiogram.filters import Command
from aiogram.methods.send_message import SendMessage
from aiogram.fsm.context import FSMContext
from aiogram.fsm.state import State, StatesGroup
from facade import Facade

from res.strings import *
import res.kb as kb

messages = dict()

router = Router()


class Form(StatesGroup):
    vernam_mess = State()
    vernam_calc = State()
    caesar_mess = State()
    caesar_calc = State()
    vigenere_mess = State()
    vigenere_calc = State()


@router.message(Command("start"))
async def start_handler(msg: Message, state: FSMContext):
    """
    handler for start command, print possible commands: cipher/decipher
    :param msg:
    :param state:
    :return:
    """
    await msg.answer(START_MSG, reply_markup=kb.menu)


@router.message(Command("help"))
async def help_handler(msg: Message, state: FSMContext):
    """
    handler for help command, print some info about purposes of bot
    :param msg:
    :param state:
    :return:
    """
    await msg.answer(HELP_MSG)


@router.callback_query(F.data == "cipher")
async def cipher_handler(callback: CallbackQuery, state: FSMContext):
    """
    print message with ciphering options
    :param callback:
    :param state:
    :return:
    """
    await callback.bot(SendMessage(chat_id=callback.message.chat.id,
                                   text=CHOOSE_CIPHER_MSG,
                                   reply_markup=kb.encrypt))
    await callback.answer()


@router.callback_query(F.data == "decipher")
async def decipher_handler(callback: CallbackQuery, state: FSMContext):
    """
    print message with deciphering options
    :param callback:
    :param state:
    :return:
    """
    await callback.bot(SendMessage(chat_id=callback.message.chat.id,
                                   text=CHOOSE_DECIPHER_MSG,
                                   reply_markup=kb.decrypt))
    await callback.answer()


@router.callback_query((F.data == "vernam_cipher") | (F.data == "vernam_decipher"))
async def vernam_init(callback: CallbackQuery, state: FSMContext):
    """
    first step of vernam ciphering/deciphering - bot print message, that he ready to accept file
    :param callback:
    :param state:
    :return:
    """
    await callback.bot(SendMessage(chat_id=callback.message.chat.id, text=VERNAM_ENCRYPT_MESSAGE))
    await state.set_state(Form.vernam_mess)
    if callback.data == "vernam_cipher":
        await state.set_data({"type": "cipher"})
    if callback.data == "vernam_decipher":
        await state.set_data({"type": "decipher"})
    await callback.answer()


@router.message(Form.vernam_mess)
async def vernam_message(msg: Message, state: FSMContext):
    """
    second step of vernam ciphering/deciphering - user sends file to cipher/decipher
    :param msg:
    :param state:
    :return:
    """
    file = msg.document
    if file is None:
        await msg.answer(text=ERROR_RETURN_TEXT)
        await state.clear()
        return
    messages[msg.chat.id] = file
    method = (await state.get_data())["type"]
    return_text = ERROR_RETURN_TEXT
    if method == "cipher":
        return_text = VERNAM_BEG_SECOND_FILE
    if method == "decipher":
        return_text = VERNAM_BEG_SECOND_FILE_DECIPHER
    await state.set_state(Form.vernam_calc)
    await msg.answer(text=return_text)


@router.message(Form.vernam_calc)
async def vernam_output(msg: Message, state: FSMContext):
    """
    third step of vernam ciphering/deciphering - user sends another file to cipher/decipher
    and bot sends in return ciphered/deciphered file
    :param msg:
    :param state:
    :return:
    """
    file = msg.document
    if file is None:
        await msg.answer(text=ERROR_RETURN_TEXT)
        await state.clear()
        return
    is_ok = False
    return_file = None
    try:
        return_file = await Facade.vernam_message_crypt(messages[msg.chat.id], file)
        is_ok = True
    except:
        pass
    await state.clear()
    messages.pop(msg.chat.id, "happens")
    if is_ok:
        await msg.answer_document(document=return_file)
        os.remove(return_file.path)
    else:
        await msg.answer(text=ERROR_RETURN_TEXT)


@router.callback_query(F.data == "caesar_decipher")
async def caesar_decipher_method(callback: CallbackQuery, state: FSMContext):
    """
    choose decipher method - brute force or fixed shift
    :param callback:
    :param state:
    :return:
    """
    await callback.bot(SendMessage(chat_id=callback.message.chat.id,
                                   text=CAESAR_TYPE_MESSAGE,
                                   reply_markup=kb.decrypt_caesar))
    await callback.answer()


@router.callback_query((F.data == "caesar_fixed") | (F.data == "caesar_brute") | (F.data == "caesar_cipher"))
async def caesar_message(callback: CallbackQuery, state: FSMContext):
    """
    first step of caesar ciphering/deciphering - bot print message, that he ready to accept message
    :param callback:
    :param state:
    :return:
    """
    await callback.bot(SendMessage(chat_id=callback.message.chat.id, text=CAESAR_MESSAGE))
    await state.set_state(Form.caesar_mess)
    if callback.data == "caesar_fixed":
        await state.set_data({"type": "fixed"})
    if callback.data == "caesar_brute":
        await state.set_data({"type": "brute"})
    if callback.data == "caesar_cipher":
        await state.set_data({"type": "encrypt"})
    await callback.answer()


@router.message(Form.caesar_mess)
async def caesar_shift(msg: Message, state: FSMContext):
    """
    second step of caesar ciphering/deciphering -
    bot print message, that he ready to accept shift/number of shifts
    :param msg:
    :param state:
    :return:
    """
    messages[msg.chat.id] = msg.text
    await state.set_state(Form.caesar_calc)
    method = (await state.get_data())["type"]
    if method == "brute":
        await msg.answer(text=CAESAR_BRUTE_COUNT_BEG)
    else:
        await msg.answer(text=CAESAR_SHIFT_BEG)


@router.message(Form.caesar_calc)
async def caesar_output(msg: Message, state: FSMContext):
    """
    third step of caesar ciphering/deciphering -
    bot print ciphered/deciphered message
    :param msg:
    :param state:
    :return:
    """
    return_text = ERROR_RETURN_TEXT
    try:
        shift = int(msg.text)
        method = (await state.get_data())["type"]
        return_text = Facade.caesar_message_crypt(messages[msg.chat.id], shift, method)
    except:
        pass
    await state.clear()
    messages.pop(msg.chat.id, "happens")
    await msg.answer(text=return_text)


@router.callback_query((F.data == "vigenere_decipher") | (F.data == "vigenere_cipher"))
async def vigenere_init(callback: CallbackQuery, state: FSMContext):
    """
    first step of vigenere ciphering/deciphering -
    bot print message, that he ready to accept message
    :param callback:
    :param state:
    :return:
    """
    if callback.data == "vigenere_decipher":
        response = VIGENERE_DECRYPT_MESSAGE
        new_item = {"type": "decrypt"}
    else:
        response = VIGENERE_ENCRYPT_MESSAGE
        new_item = {"type": "encrypt"}
    await callback.bot(SendMessage(chat_id=callback.message.chat.id, text=response))
    await state.set_state(Form.vigenere_mess)
    await state.set_data(new_item)
    await callback.answer()


@router.message(Form.vigenere_mess)
async def vigenere_message(msg: Message, state: FSMContext):
    """
    second step of vigenere ciphering/deciphering -
    bot print message, that he ready to accept key for ciphering/deciphering
    :param msg:
    :param state:
    :return:
    """
    text = msg.text
    if (await state.get_data())["type"] == "decrypt":
        response = VIGENERE_DECRYPT_KEY_MESSAGE
    else:
        response = VIGENERE_ENCRYPT_KEY_MESSAGE
    messages[msg.chat.id] = text
    await state.set_state(Form.vigenere_calc)
    await msg.answer(text=response)


@router.message(Form.vigenere_calc)
async def vigenere_output(msg: Message, state: FSMContext):
    """
    third step of vigenere ciphering/deciphering -
    bot print ciphered/deciphered message
    :param msg:
    :param state:
    :return:
    """
    return_text = ERROR_RETURN_TEXT
    try:
        method = (await state.get_data())["type"]
        return_text = Facade.vigenere_message_crypt(messages[msg.chat.id], msg.text, method)
    except:
        pass
    await state.clear()
    messages.pop(msg.chat.id, "happens")
    await msg.answer(text=return_text)


@router.message(Command("cancel"))
async def cancel_command(msg: Message, state: FSMContext):
    """
    clear all states and reset bot
    :param msg:
    :param state:
    :return:
    """
    await state.clear()
    messages.pop(msg.chat.id, "will probably happen")
    await msg.answer(text=CANCEL_MESSAGE)


@router.message()
async def message_handler(msg: Message, state: FSMContext):
    """
    if user send messages outside any stated and commands,
    he will receive this message
    :param msg:
    :param state:
    :return:
    """
    await msg.answer(f"Не понимаю. Блокирую!\nСмартфон Vivo")