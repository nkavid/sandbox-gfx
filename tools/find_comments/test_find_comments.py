import pathlib
import shutil

from tools.find_comments import find_comments


class DummyFiles:
    def __init__(self) -> None:
        root = pathlib.Path(__file__).parent
        self.dummies = root / "dummies"
        self.dummies.mkdir()
        with open(self.dummies / "dummy.hpp", "w", encoding="utf-8") as dummy_file:
            dummy_file.write(
                "// NOLINT()\n"
                + "\n"
                + "// NOLINT\n"
                + "// NOLINTBEGIN(somerule)\n"
                + "\n"
                + "// NOLINT (\n"
                + "// NOLINTNEXTLINE(somerule)\n"
                + "\n"
                + "// NOLINTEND(somerule)\n"
                + "// NOLINTEND\n"
                + "\n"
            )
        with open(self.dummies / "dummy.cpp", "w", encoding="utf-8") as dummy_file:
            dummy_file.write(
                "for(int i; i < 1; ++i) // NOLINT (\n"
                + "int main() // NOLINT()\n"
                + "\n"
                + "#include <somestuff> // IWYU pragma: keep\n"
                + "\n"
                + "\n"
                + "for(int i; i < 1; ++i) // NOLINTEND(somerule)\n"
                + "struct S // NOLINTEND\n"
                + "\n"
            )

    def __del__(self) -> None:
        shutil.rmtree(self.dummies)

    def get(self) -> pathlib.Path:
        return self.dummies


def test_find_comments() -> None:
    dummy_files = DummyFiles()
    find_comments_instance = find_comments.FindComments(dummy_files.get())
    occurences = find_comments_instance.print_checks()
    assert occurences == 5
