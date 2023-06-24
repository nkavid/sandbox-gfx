import logging

# ruff: noqa


class CustomFormatter(logging.Formatter):
    grey = "\x1b[37;20m"
    magenta = "\x1b[35;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"
    green = "\x1b[32;20m"
    reset = "\x1b[0m"

    _format = "[%(levelname)s] " + reset + "%(filename)s" + " - " + "%(message)s"

    FORMATS = {
        logging.DEBUG: magenta + _format,
        logging.INFO: green + _format,
        logging.WARNING: yellow + _format,
        logging.ERROR: red + _format,
        logging.CRITICAL: bold_red + _format,
    }

    def format(self, record: logging.LogRecord):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(record)


class Logger:
    logging.basicConfig()
    logging.root.setLevel(logging.DEBUG)
    streamHandler = logging.StreamHandler()
    streamHandler.setFormatter(CustomFormatter())
    logger = logging.getLogger("gfx")
    logger.addHandler(streamHandler)
    logger.propagate = False

    @staticmethod
    def get(loglevel, name: str | None = None):
        logging.root.setLevel(loglevel)
        if name is None:
            return Logger.logger
        return Logger.logger.getChild(name)
