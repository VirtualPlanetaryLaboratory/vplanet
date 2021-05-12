# -*- coding: utf-8 -*-
import logging

logger = logging.getLogger("vplanet")
ch = logging.StreamHandler()
ch.setLevel(logging.ERROR)
formatter = logging.Formatter("%(levelname)s:%(message)s")
ch.setFormatter(formatter)
logger.addHandler(ch)
