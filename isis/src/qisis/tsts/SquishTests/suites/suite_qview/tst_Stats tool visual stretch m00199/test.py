import shutil
import os

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
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/approvalStatusTest.cub")
    clickButton(waitForObject(":Open.Open_QPushButton"))
    
    # Finished with initial run
    clickButton(waitForObject(":qview_statisticsToolButton"))
    mouseClick(waitForObject(":viewport1"), 280, 325, 0, Qt.LeftButton)
    waitFor("object.exists(':Statistics.Hide Display_QCheckBox')", 20000)
    test.compare(findObject(":Statistics.Hide Display_QCheckBox").text, "Hide Display")
    test.compare(findObject(":Statistics.Hide Display_QCheckBox").checked, True)
    test.compare(findObject(":Statistics.Hide Display_QCheckBox").visible, True)
    test.compare(findObject(":Statistics.Hide Display_QCheckBox").checkable, True)
    waitFor("object.exists(':Statistics.Hide Display_QCheckBox')", 20000)
    test.compare(findObject(":Statistics.Hide Display_QCheckBox").text, "Hide Display")
    test.compare(findObject(":Statistics.Hide Display_QCheckBox").checked, True)
    test.compare(findObject(":Statistics.Hide Display_QCheckBox").visible, True)
    test.compare(findObject(":Statistics.Hide Display_QCheckBox").checkable, True)
    waitFor("object.exists(':Statistics.Minimum_QLabel')", 20000)
    test.compare(findObject(":Statistics.Minimum_QLabel").text, "Minimum: 1.73616e+06")
    waitFor("object.exists(':Statistics.Maximum_QLabel')", 20000)
    test.compare(findObject(":Statistics.Maximum_QLabel").text, "Maximum: 1.7362e+06")
    waitFor("object.exists(':Statistics.Average_QLabel')", 20000)
    test.compare(findObject(":Statistics.Average_QLabel").text, "Average: 1.73618e+06")
    waitFor("object.exists(':Statistics.Standard Dev_QLabel')", 20000)
    test.compare(findObject(":Statistics.Standard Dev_QLabel").text, "Standard Dev: 14.881756")
    clickButton(waitForObject(":Statistics.Hide Display_QCheckBox"))
    test.vp("initialDnDisplay")

    clickButton(waitForObject(":Display Mode.Show Pixel Values_QRadioButton"))
    test.vp("stretchedDnDisplay")
    
    sendEvent("QCloseEvent", waitForObject(":qview_Isis::ViewportMainWindow"))


    snooze(1)
    
    # Restore original qview settings
    try:
        shutil.rmtree(os.path.expandvars('$HOME/.Isis/qview'))
    except Exception:
        pass
    
    try:
        os.rename(os.path.expandvars('$HOME/.Isis/qview.squishbackup'), os.path.expandvars('$HOME/.Isis/qview'))
    except Exception:
        pass


