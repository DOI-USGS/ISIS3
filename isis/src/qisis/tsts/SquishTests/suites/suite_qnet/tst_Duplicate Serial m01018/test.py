import os

# This test is designed to look for an error that was fixed in mantis ticket #1018. The issue was
# having two images with the same serial number.
def main():
    try:
        os.mkdir(os.path.expandvars('$ISISROOT/src/qisis/tsts/SquishTests/output'))
    except Exception:
        pass
    try:
        os.unlink(os.path.expandvars('$ISISROOT/src/qisis/tsts/SquishTests/output/') + 'DuplicateSerialm01018.net')
    except Exception:
        pass
    
    startApplication("qnet")
    
    
    # Open cube list (tr.lis)
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open control network and cube list"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/m01018/tr.lis")
    type(waitForObject(":_QListView"), "<Return>")

    # Open control net (enceladus_sp_no390.net)
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_7"), 100, 16, 0, Qt.LeftButton)
    waitForObjectItem(":stackedWidget.treeView_QTreeView_2", "enceladus\\_sp\\_no390\\.net")
    doubleClickItem(":stackedWidget.treeView_QTreeView_2", "enceladus\\_sp\\_no390\\.net", 58, 5, 0, Qt.LeftButton)
    
    # Open ground source (N1602275390_1_spjig.cub) -- expect warning for duplicate SNs
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open Ground Source"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_2"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "../src/qisis/tsts/SquishTests/input/m01018/N1602275390_1_spjig.cub")
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "<Return>")
    
    # Click OK on warning
    clickButton(waitForObject(":Cannot set ground source.OK_QPushButton"))
    
    
    # Open cube list (tr_no390.lis)
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open control network and cube list"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/m01018/tr_no390.lis")
    type(waitForObject(":_QListView"), "<Return>")

    # Open control net (enceladus_sp_no390.net)
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_7"), 95, 13, 0, Qt.LeftButton)
    waitForObjectItem(":stackedWidget.treeView_QTreeView_2", "enceladus\\_sp\\_no390\\.net")
    doubleClickItem(":stackedWidget.treeView_QTreeView_2", "enceladus\\_sp\\_no390\\.net", 58, 5, 0, Qt.LeftButton)
    
    # Open ground source (N1602275390_1_spjig.cub) -- there should be no warning this time
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open Ground Source"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_2"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "../src/qisis/tsts/SquishTests/input/m01018/N1602275390_1_spjig.cub")
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "<Return>")
    
    # Right click on ground source to create new point
    mouseClick(waitForObject(":qnet.viewport_QWidget"), 36, 125, Qt.NoModifier, Qt.RightButton)

    # Name the new point grnd1
    mouseClick(waitForObject(":Point ID:_QLineEdit"), 55, 13, 0, Qt.LeftButton)
    type(waitForObject(":Point ID:_QLineEdit"), "grnd1")
    clickButton(waitForObject(":Create Fixed or Constrained ControlPoint.OK_QPushButton"))

    # Save measure/save point
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))
    
    # Save resulting network
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Save Control Network As..."))
    clickButton(waitForObject(":Choose filename to save under.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_3"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), "src/qisis/tsts/SquishTests/output/DuplicateSerialm01018.net")
    snooze(0.5)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), "<Return>")
    
    # Exit
    activateItem(waitForObjectItem(":qnet_QMenuBar_2", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Exit"))
    snooze(1)
    
