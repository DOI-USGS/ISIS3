import os
import shutil

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
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/lub2675j.342.lev1.cub")
    clickButton(waitForObject(":Open.Open_QPushButton"))
    waitFor("object.exists(':viewport1_progress')")
    waitFor('findObject(":viewport1_progress").text == \"100%\"')
    
    clickButton(waitForObject(":qview.Open_QToolButton"))
    snooze(0.5)
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/I26649003RDR.crop.tr.lev2.cub")
    clickButton(waitForObject(":Open.Open_QPushButton"))
    waitFor("object.exists(':viewport2_progress')")
    waitFor('findObject(":viewport2_progress").text == \"100%\"')
    
    clickButton(waitForObject(":qview_QToolButton"))
    mouseDrag(waitForObject(":viewport2_contents"), 251, 161, 11, 11, 1, Qt.LeftButton)
    test.vp("firstViewportNoRubberBand")
    
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


