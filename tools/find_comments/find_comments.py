import os
import pathlib
import re

from pipe import where

ALLOWED_PATTERNS = [
    r"NOLINT\(",
    r"NOLINTNEXTLINE\(",
    r"NOLINTBEGIN\(",
    r"NOLINTEND\(",
    "IWYU pragma",
]


def is_allowed(line: str) -> bool:
    result = False

    for pattern in ALLOWED_PATTERNS:
        if re.findall(pattern, line):
            result = True

    return result


class FindComments:
    def __init__(self, directory: pathlib.Path) -> None:
        self.filepaths = []

        for root, _, files in os.walk(directory):
            for file_to_check in files:
                filepath = pathlib.Path(root) / file_to_check
                self.filepaths.append(filepath)

    def print_checks(self) -> int:
        print("============== RESULTS ===============")
        return self._check()

    def _check(self) -> int:
        occurences = 0
        for filepath in self.filepaths:

            if str(filepath).endswith(".cpp") or str(filepath).endswith(".hpp"):
                with filepath.open() as filedescriptor:
                    lines = filedescriptor.read().splitlines()

                    result = list(
                        lines
                        | where(lambda line: re.findall("//", line) != [])
                        | where(lambda line: not is_allowed(line))
                    )

                    for i, element in enumerate(result):
                        occurences = occurences + 1
                        print(filepath.name + ":" + str(i) + " " + element)

        return occurences
