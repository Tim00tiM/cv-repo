import asyncio
import logging
import os

from aiogram import Bot, Dispatcher
from aiogram.enums.parse_mode import ParseMode
from aiogram.methods import SetMyCommands
from aiogram.types import BotCommand

from dotenv import load_dotenv
from res.handlers import router


async def main():
    load_dotenv()
    bot = Bot(token=os.getenv("BOT_TOKEN"), parse_mode=ParseMode.HTML)
    await bot.set_my_commands([
        BotCommand(command="help", description="Вывести информацию о боте."),
        BotCommand(command="start", description="Начать работать с ботом.")
    ])
    dp = Dispatcher()
    dp.include_router(router)
    await dp.start_polling(bot, allowed_updates=dp.resolve_used_update_types())

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    asyncio.run(main())
