#!/usr/bin/python3

import re
import textwrap

from typing import List
from pathlib import Path


PATTERN_TODO = re.compile(r'^\s*//\s*TODO\s*:\s*(.*)$', re.IGNORECASE)
PATTERN_CONT = re.compile(r'^\s*//\s*(.*)$')


def whattodo_file(path: str, text: List[str]):
    n = len(text)
    i = 0

    while i < n:
        line = text[i]

        m_todo = PATTERN_TODO.match(line)

        if not m_todo:
            i = i + 1
            continue

        print(f"{path}:{i + 1}")

        for line in textwrap.wrap(m_todo.group(1).strip(), width=80):
            print("    {}".format(line))

        i = i + 1

        while i < n:
            line = text[i]

            m_todo = PATTERN_TODO.match(line)

            if m_todo:
                break

            m_cont = PATTERN_CONT.match(line)

            if not m_cont:
                break;

            for line in textwrap.wrap(m_cont.group(1).strip(), width=80):
                print("    {}".format(line))

            i = i + 1

        print()


def main():
    root = Path("src")
    paths = [f for f in root.rglob("*") if f.is_file()]

    print()

    for path in paths:
        with open(path, "r", encoding="utf-8", errors="ignore") as file:
            text = file.readlines()

            whattodo_file(str(path), text)


if __name__ == "__main__":
    main()

