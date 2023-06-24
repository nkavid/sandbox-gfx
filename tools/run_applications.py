import contextlib
import logging
import pathlib
import subprocess  # nosec
import sys
import time

from tools.logger import Logger

logger = Logger.get(logging.DEBUG)


class Process:
    def __init__(self, executable: pathlib.Path | str, args: list[str]) -> None:
        if isinstance(executable, pathlib.Path):
            self.name = executable.name

        elif isinstance(executable, str):
            self.name = executable

        try:
            # pylint: disable=consider-using-with
            self.process = subprocess.Popen([executable, *args])  # nosec # noqa: S603
        except FileNotFoundError:
            logger.exception(
                "'{expected_file}' not found",
                extra={"expected_file": executable},
            )
            sys.exit("fatal error")

    def poll(self) -> int | None:
        result = self.process.poll()
        if result is not None:
            logger.info(
                "'{process_name}' has stopped",
                extra={"process_name": self.name},
            )

        return result

    def terminate(self) -> None:
        self.process.terminate()


class URI:
    def __init__(self, schema: str, path: str) -> None:
        self.path = pathlib.Path(path)
        self.schema = schema

    def __del__(self) -> None:
        with contextlib.suppress(FileNotFoundError):
            self.path.unlink()

    def uri(self) -> str:
        return f"{self.schema}:{self.path}"


def main() -> None:
    logger.info("starting...")

    frame_rate = 5
    duration = 10
    size = "512x512"

    dummy = URI("file", f"build/dummy_{size}_{duration}s_{frame_rate}hz.mp4")

    stream = URI("unix", "/tmp/input_stream_socket")  # nosec # noqa: S108

    call_once(
        pathlib.Path("build/bin/muxing"),
        [
            "--output-uri",
            dummy.uri(),
            "--size",
            size,
            "--duration",
            str(duration),
            "--frame-rate",
            str(frame_rate),
        ],
    )

    processes = []

    processes.append(
        Process(
            "ffmpeg",
            [
                "-loglevel",
                "0",
                "-i",
                dummy.uri(),
                "-c",
                "h264",
                "-f",
                "mpegts",
                "-listen",
                "1",
                stream.uri(),
            ],
        )
    )

    time.sleep(0.5)

    processes.append(
        Process(
            "ffplay",
            [
                "-i",
                stream.uri(),
                "-autoexit",
            ],
        )
    )

    running = True

    while running:
        for process in processes:
            if process.poll() is not None:
                running = False

        time.sleep(1)

        if not running:
            for process in processes:
                process.terminate()


def call_once(executable: pathlib.Path, args: list[str]) -> None:
    subprocess.call([executable, *args])  # nosec # noqa: S603


if __name__ == "__main__":
    main()
    logger.info("exiting...")
