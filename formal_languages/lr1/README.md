## LR(1) алгоритм

### Работа с проектом

Проект поддерживает исполнение в двух режимах - [console](#консольный-режим) и [files](#файловый-режим).

Чтобы запустить в консольном режиме - ```python main.py console```

Чтобы запустить в файловом режиме - ```python main.py files ...filenames```



### Консольный режим

Работа происходит так, как описано в ТЗ:

>В первой строке содержатся 3 целых числа ∣N∣,∣Σ∣ и ∣P∣ - количество нетерминальных символов, терминальных символов и правил в порождающей грамматике. Все числа неотрицательные и не превосходят 100.
>
>Во второй строке содержатся ∣N∣ нетерминальных символов. Нетерминальные символы являются заглавными латинскими буквами.
>
>В третьей строке содержатся ∣Σ∣ символов алфавита. Символы являются строчными латинскими буквами, цифрами, скобками или знаками арифметических операций.
>
>В каждой из следующих P строк записано одно правило грамматики в формате левая часть правила -> правая части правила. ε в правой части правила обозначается отсутствием правой части (концом строки после ->).
>
>Следующая строка состоит из одного нетерминального символа --- стартового символа грамматики.
>
>Следующая строка состоит из одного целого числа m, 1 ≤ m ≤ 100000 - количества слов, принадлежности которых языку надо проверить.
>
>В каждой из следующих m строк содержится одно слово, состоящее из символов алфавита грамматики, принадлежность которого языку надо проверить. Суммарная длина всех слов не превосходит 10000000.


### Файловый режим

Принимает на вход файлы, в каждом из которых данные представлены в таком же виде,
который требуется для работы в консольном режиме.
Поддерживает работу с несколькими файлами, которые необходимо указать через пробел.

### Логирование алгоритма

При запуске в любом из режимов, на месте запуска в файле *место_запуска/logs/(имя_файла/console)*
будет создан текстовый файл, в котором каждый из шагов алгоритма будет описан.

### Тесты

Для запуска тестов необходимо установить ```requirements.txt```, после чего, находясь в папке с проектом, запустить ```pytest```:

```bash
cd путь_до_проекта
pip install -r requirements.txt
pytest
```

### Минимальные требования

```
python3.10
```

При несоответствии версий программ, корректность работы не гарантируется
