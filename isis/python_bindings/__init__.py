from .astroset import *

from pathlib import Path 
import os

try: 
    ISISROOT = Path(os.environ["ISISROOT"])
except KeyError: 
    raise EnvironmentError("Please set ISISROOT before importing anything")

def init_application(args): 
    appname = os.path.basename(args[0])
    args = args[1:]
   
    xmlpath = Path(appname).with_suffix(".xml")
    if not xmlpath.exists() or not xmlpath.is_file():
        xmlpath = ISISROOT / "bin" / "xml" / xmlpath
        print(xmlpath)
        if not xmlpath.exists(): 
            raise FileExistsError(f"{appname} does not have an XML file")

    return UserInterfaceFromList(str(xmlpath), args)