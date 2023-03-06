from .astroset import *

from pathlib import Path 
import os

try: 
    ISISROOT = Path(os.environ["ISISROOT"])
except KeyError: 
    raise EnvironmentError("Please set ISISROOT before importing anything")

def init_application(args): 
    # normally if no args are inputed, it fires the GUI 
    # but python doesn't support ISIS GUIs so it'll print 
    # the help string instead. 
    if len(args) <= 1:
        args.append("-H")

    appname = args[0]
    args = args[1:]

    xmlpath = Path(appname).with_suffix(".xml")

    if not xmlpath.is_file():
        xmlpath = ISISROOT / "bin" / "xml" / xmlpath
        if not xmlpath: 
            raise FileExistsError(f"{appname} does not have an XML file")

    return UserInterfaceFromList(str(xmlpath), args)

