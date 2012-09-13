

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
    clickButton(waitForObject(":qview_bandToolButton"))
    waitFor("object.exists(':qview_QStackedWidget')", 20000)
    test.compare(findObject(":qview_QStackedWidget").currentIndex, 0)
    waitFor("object.exists(':qview.RGB_QRadioButton')", 20000)
    test.compare(findObject(":qview.RGB_QRadioButton").enabled, False)
    waitFor("object.exists(':qview.Gray_QRadioButton')", 20000)
    test.compare(findObject(":qview.Gray_QRadioButton").enabled, False)
    waitFor("object.exists(':qview.to All Viewports_QToolButton')", 20000)
    test.compare(findObject(":qview.to All Viewports_QToolButton").enabled, False)
    waitFor("object.exists(':qview_QComboBox')", 20000)
    test.compare(findObject(":qview_QComboBox").enabled, False)
    waitFor("object.exists(':qview.to All Viewports_QStackedWidget')", 20000)
    test.compare(findObject(":qview.to All Viewports_QStackedWidget").currentIndex, 0)
    waitFor("object.exists(':qview_QStackedWidget_2')", 20000)
    test.compare(findObject(":qview_QStackedWidget_2").currentIndex, 0)
    waitFor("object.exists(':qview_QSpinBox')", 20000)
    test.compare(findObject(":qview_QSpinBox").enabled, False)
    waitFor("object.exists(':qview_QSpinBox_2')", 20000)
    test.compare(findObject(":qview_QSpinBox_2").enabled, False)
    waitFor("object.exists(':qview_QSpinBox_3')", 20000)
    test.compare(findObject(":qview_QSpinBox_3").enabled, False)
    waitFor("object.exists(':qview_QLabel')", 20000)
    test.compare(findObject(":qview_QLabel").enabled, False)
    waitFor("object.exists(':qview_QLabel_2')", 20000)
    test.compare(findObject(":qview_QLabel_2").enabled, False)
    waitFor("object.exists(':qview_QLabel_3')", 20000)
    test.compare(findObject(":qview_QLabel_3").enabled, False)
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
