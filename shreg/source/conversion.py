app_name = "shreg"
import gettext
gettext.install(app_name)
_ = gettext.gettext

def uencode(text):
    return  unicode(text).encode("utf-8")

def encode(text):
    return text.encode('utf-8')

def decode(text):
    return text.decode("utf-8")
