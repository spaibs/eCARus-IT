__author__ = 'ga38sor'


def to_8_chars(text):
    if len(text) < 8:
        for i in range(0, 8 - len(text)):
            text = "0" + text
    return text
