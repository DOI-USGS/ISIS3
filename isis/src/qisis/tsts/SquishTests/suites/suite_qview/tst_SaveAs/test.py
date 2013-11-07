import os
import shutil

def main():
    # Backup current qview settings
    try:
        shutil.rmtree(os.path.expandvars('$HOME/.Isis/qview.squishbackup'))
    except Exception:
        pass
    try:
        os.mkdir(os.path.expandvars('$ISISROOT/src/qisis/tsts/SquishTests/output'))
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
    
    #  Save viewport as the "Full Image"
    activateItem(waitForObjectItem(":qview_QMenuBar", "File"))
    activateItem(waitForObjectItem(":_QMenu", "Save As..."))
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_2"), 105, 11, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "../src/qisis/tsts/SquishTests/output/saveAsFullImage.cub")
    clickButton(waitForObject(":Save As.Save_QPushButton"))

    waitFor("object.exists(':viewport2_progress')")
    waitFor('findObject(":viewport2_progress").text == \"100%\"')
    snooze(1.0);
    test.vp("afterSaveAsFullImageViewport")

    # Save viewport "As Is"
    mouseDrag(waitForObject(":qview.saveAsFullImage.cub @ 137.778% (Gray = 1)_Isis::ViewportMdiSubWindow"), 523, 22, 224, 19, 1, Qt.LeftButton)
    mouseClick(waitForObject(":viewport1_contents"), 235, 380, 0, Qt.LeftButton)
    mouseClick(waitForObject(":viewport1_contents"), 275, 29, 0, Qt.LeftButton)

    activateItem(waitForObjectItem(":qview_QMenuBar", "File"))
    activateItem(waitForObjectItem(":_QMenu", "Save As..."))
    clickButton(waitForObject(":Save As.Export As Is_QRadioButton"))
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_2"), 43, 14, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "../src/qisis/tsts/SquishTests/output/saveAsIs.cub")
    clickButton(waitForObject(":Save As.Save_QPushButton"))

    waitFor("object.exists(':viewport3_progress')")
    waitFor('findObject(":viewport3_progress").text == \"100%\"')
    snooze(1.0);
    test.vp("afterSaveAsIsViewport")

    # Save viewport as "Full resolution"
    mouseClick(waitForObject(":qview.I26649003RDR.crop.tr.lev2.cub @ 275.556% (Gray = 1)_Isis::ViewportMdiSubWindow"), 77, 19, 0, Qt.LeftButton)
    mouseDrag(waitForObject(":qview.I26649003RDR.crop.tr.lev2.cub @ 275.556% (Gray = 1)_Isis::ViewportMdiSubWindow"), 6, 2, 230, 298, 1, Qt.LeftButton)
    activateItem(waitForObjectItem(":qview_QMenuBar", "File"))
    activateItem(waitForObjectItem(":_QMenu", "Save As..."))
    clickButton(waitForObject(":Save As.Export Full Res_QRadioButton"))
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_2"), 108, 9, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "../src/qisis/tsts/SquishTests/output/saveAsFullRes.cub")
    clickButton(waitForObject(":Save As.Save_QPushButton"))

    waitFor("object.exists(':viewport4_progress')")
    waitFor('findObject(":viewport4_progress").text == \"100%\"')
    snooze(1.0);
    test.vp("afterSaveAsFullResViewport")
    
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
