import argparse
import pathlib
import sys

from tools.find_comments import find_comments


def main() -> None:
    directory = parse_arguments()
    application = find_comments.FindComments(directory)
    occurences = application.print_checks()
    sys.stdout.write(str(directory) + ": total occurences: " + str(occurences))


def parse_arguments() -> pathlib.Path:
    parser = argparse.ArgumentParser(description="find comment stuff")

    parser.add_argument(
        "--directory", dest="directory", help="directory with files to check"
    )

    args = parser.parse_args()

    if args.directory is None:
        msg = "Missing directory argument"
        raise ValueError(msg)

    return pathlib.Path(args.directory)


if __name__ == "__main__":
    main()
