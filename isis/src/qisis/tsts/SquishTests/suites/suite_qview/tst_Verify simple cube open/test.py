
def main():
    # Backup current qview settings
    try:
        shutil.rmtree(os.path.expandvars('$HOME/.Isis/qview.squishbackup'))
    except Exception:
        pass
    
    try:
        os.rename(os.path.expandvars('$HOME/.Isis/qview'), os.path.expandvars('$HOME/.Isis/qview.squishbackup'))
    except Exception:
        pass
    startApplication("qview")
    clickButton(waitForObject(":qview.Open_QToolButton"))
    snooze(0.5)
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/I26649003RDR.crop.tr.lev2.cub")
    clickButton(waitForObject(":Open.Open_QPushButton"))
    waitFor("object.exists(':viewport1_progress')")
    waitFor('findObject(":viewport1_progress").text == \"100%\"')
    test.vp("viewportContents")
    
    sendEvent("QCloseEvent", waitForObject(":qview_Isis::ViewportMainWindow"))
    
    # Restore original qview settings
    try:
        shutil.rmtree(os.path.expandvars('$HOME/.Isis/qview'))
    except Exception:
        pass
    
    try:
        os.rename(os.path.expandvars('$HOME/.Isis/qview.squishbackup'), os.path.expandvars('$HOME/.Isis/qview'))
    except Exception:
        pass


