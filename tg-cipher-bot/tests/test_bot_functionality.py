import datetime
from tempfile import NamedTemporaryFile
from typing import Dict, Any
import pytest
from res.handlers import *
from aiogram.fsm.storage.base import BaseStorage
from aiogram.types import User, Chat, FSInputFile, Document
import shutil


class MyStrorage(BaseStorage):
    async def close(self) -> None:
        pass

    def __init__(self):
        self.storage = dict()
        self.state = ""

    async def get_state(self, key: str):
        return self.state

    async def set_state(self, key: str, state: str = None) -> None:
        self.state = state

    async def set_data(self, key: str, data: Dict[str, Any]) -> None:
        self.storage = data

    async def get_data(self, key: str) -> Dict[str, Any]:
        return self.storage


def insta_ret(*a, **b):
    return


async def async_insta_ret(*a, **b):
    return


async def mocked_answer(self, text=""):
    self.text = text


async def mocked_answer_document(self, document):
    shutil.copy(document.path, "/tmp/verik")
    self.text = "/tmp/verik"


class MyBot:
    async def __call__(self, *a, **b):
        return

    async def download(self, from_file: str, destination: str):
        with open(from_file, "rb") as ff, open(destination, "wb") as tf:
            tf.write(ff.read())


async def caesar_cycle(callback, state, to_encrypt, to_encrypt_shift, final_message):
    await caesar_message(callback, state)
    await caesar_shift(to_encrypt, state)
    await caesar_output(to_encrypt_shift, state)
    assert to_encrypt_shift.text == final_message


async def vigenere_cycle(callback, state, to_encrypt, key, final_message):
    await vigenere_init(callback, state)
    await vigenere_message(to_encrypt, state)
    await vigenere_output(key, state)
    assert key.text == final_message


async def vernam_cycle(callback, state, to_encrypt, key, final_message):
    await vernam_init(callback, state)
    await vernam_message(to_encrypt, state)
    await vernam_output(key, state)
    check_vernam_correctness(key, final_message)


def patch_all(monkeypatch):
    monkeypatch.setattr(Message, "answer", mocked_answer)
    monkeypatch.setattr(Message, "answer_document", mocked_answer_document)
    monkeypatch.setattr(Message, "_check_frozen", insta_ret)
    monkeypatch.setattr(CallbackQuery, "_check_frozen", insta_ret)
    monkeypatch.setattr(CallbackQuery, "bot", MyBot())
    monkeypatch.setattr(CallbackQuery, "answer", async_insta_ret)
    monkeypatch.setattr(Document, "_check_frozen", insta_ret)


def check_vernam_correctness(msg, answer):
    with open(msg.text, "rb") as result:
        for i in range(len(answer)):
            q = result.read(1)
            z = answer[i].to_bytes(1, 'big')
            assert q == z


@pytest.fixture
def init_service(monkeypatch):
    patch_all(monkeypatch)
    fake_chat = Chat(id=1, type="private")
    purple = User(first_name="Purple", id=1, is_bot=False)
    to_encrypt = Message(chat=fake_chat, message_id=1,
                         date=datetime.datetime.now(), text="")
    callback = CallbackQuery(chat_instance="?", from_user=purple, id="1", message=to_encrypt)
    state = FSMContext(key="null", storage=MyStrorage())
    to_encrypt_shift = Message(chat=fake_chat, message_id=1, date=datetime.datetime.now(), text="1")
    key = Message(chat=fake_chat, message_id=1, date=datetime.datetime.now(), text="")
    return [fake_chat, purple, callback, state, to_encrypt, to_encrypt_shift, key]


@pytest.mark.asyncio
async def test_caesar_encrypt(init_service):
    fake_chat, purple, callback, state, to_encrypt, to_encrypt_shift, _ = init_service
    callback.data = "caesar_cipher"
    to_encrypt.text = "  Я люблю есть и спать!? 1000-7"
    final_message = "  А мявмя ётуэ й трбуэ!? 1000-7"
    await caesar_cycle(callback, state, to_encrypt, to_encrypt_shift, final_message)


@pytest.mark.asyncio
async def test_caesar_decrypt(init_service):
    fake_chat, purple, callback, state, to_encrypt, to_encrypt_shift, _ = init_service
    to_encrypt.text = "  А мявмя ётуэ й трбуэ!? 1000-7"
    to_encrypt_shift.text = "1"
    callback.data = "caesar_fixed"
    final_message = "  Я люблю есть и спать!? 1000-7"
    await caesar_cycle(callback, state, to_encrypt, to_encrypt_shift, final_message)


@pytest.mark.asyncio
async def test_caesar_brute(init_service):
    fake_chat, purple, callback, state, to_encrypt, to_encrypt_shift, _ = init_service
    to_encrypt.text = "  А мявмя ётуэ й трбуэ!? 1000-7"
    to_encrypt_shift.text = "3"
    callback.data = "caesar_brute"
    final_message = '  С юруюр чдео ы двтео!? 1000-7\n' \
                    '\n  Я люблю есть и спать!? 1000-7\n' \
                    '\n  П ьосьо хвгм щ варгм!? 1000-7\n\n'
    await caesar_cycle(callback, state, to_encrypt, to_encrypt_shift, final_message)


@pytest.mark.asyncio
async def test_vigenere_encrypt(init_service):
    fake_chat, purple, callback, state, to_encrypt, _, key = init_service
    callback.data = "vigenere_cipher"
    to_encrypt.text = "В голове туман"
    key.text = "стакан"
    final_message = 'У хоцопц еучаы'
    await vigenere_cycle(callback, state, to_encrypt, key, final_message)


@pytest.mark.asyncio
async def test_vigenere_decrypt(init_service):
    fake_chat, purple, callback, state, to_encrypt, _, key = init_service
    callback.data = "vigenere_decipher"
    to_encrypt.text = 'У хоцопц еучаы'
    key.text = "стакан"
    final_message = "В голове туман"
    await vigenere_cycle(callback, state, to_encrypt, key, final_message)


@pytest.mark.asyncio
async def test_vernam_encrypt(init_service):
    fake_chat, purple, callback, state, to_encrypt, _, key = init_service
    callback.data = "vernam_cipher"
    to_ci = b'\x00\x01\x02\xff\x10'
    note_text = b'\xff\xfe\x40\x10\x15\x77\x89\x15'
    with open("/tmp/ver_test", 'wb+') as dump, open("/tmp/ver_note", 'wb+') as note:
        dump.write(to_ci)
        note.write(note_text)
    to_encrypt.document = Document(path="/tmp/ver_test", file_id="/tmp/ver_test", file_unique_id="1")
    key.document = Document(path="/tmp/ver_note", file_id="/tmp/ver_note", file_unique_id="2")
    to_encrypt.document._bot = MyBot()
    key.document._bot = MyBot()
    final_message = b'\xff\xff\x42\xef\x05'
    await vernam_cycle(callback, state, to_encrypt, key, final_message)


# @pytest.mark.asyncio
# async def test_vernam_decrypt(init_service):
#     fake_chat, purple, callback, state, to_encrypt, _, key = init_service
#     callback.data = "vernam_decipher"
#     to_encrypt.text = 'У хоцопц еучаы'
#     key.text = "стакан"
#     final_message = "В голове туман"
#     await vigenere_cycle(callback, state, to_encrypt, key, final_message)