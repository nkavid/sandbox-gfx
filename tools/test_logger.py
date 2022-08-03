import logging

from tools.logger import Logger


def test_logger() -> None:
    logger = Logger.get(logging.DEBUG, "test")
    logger.info("this is informative")
    logger.debug("this is informative")
    logger.warning("this is a warning")
    logger.error("this is not very good")
    logger.critical("this is bad sad bad")
