import os

# This tests using multiple ground source files in qnet. 

def main():
    
    startApplication("qnet")

    # Open sub.lis    
    clickButton(waitForObject(":qnet.Open control network  cube list_QToolButton"))
    clickButton(waitForObject(":Select a list of cubes.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit"), "src/qisis/tsts/SquishTests/input/qnetMultipleGroundSource/sub.lis")
    type(waitForObject(":_QListView"), "<Return>")

    #  Open sub.net
    waitForObjectItem(":stackedWidget.treeView_QTreeView_2", "sub\\.net")
    clickItem(":stackedWidget.treeView_QTreeView_2", "sub\\.net", 45, 5, 0, Qt.LeftButton)
    clickButton(waitForObject(":Select a control network.Open_QPushButton"))
    #  Edit Point h1104_0000_2
    waitForObjectItem(":Navigator_List", "h1104\\_0000\\_2")
    clickItem(":Navigator_List", "h1104\\_0000\\_2", 45, 10, 0, Qt.LeftButton)
    clickButton(waitForObject(":Control Network Navigator.Modify Point_QPushButton"))
    #  Open correct ground source for pt 1104  (f637a69.lev1.slo.cub)
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open Ground Source"))
    mouseClick(waitForObject(":Open ground source.lookInCombo_QComboBox"), 257, 7, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Open ground source.lookInCombo_QComboBox", "/tmp/Work/qnet\\_1491\\_1493\\_1655/isis/src/qisis/tsts/SquishTests/input/qnetMultipleGroundSource"), 195, 0, 0, Qt.LeftButton)
    waitForObjectItem(":stackedWidget.treeView_QTreeView_3", "f637a69\\.lev1\\_slo\\.cub")
    clickItem(":stackedWidget.treeView_QTreeView_3", "f637a69\\.lev1\\_slo\\.cub", 76, 5, 0, Qt.LeftButton)
    clickButton(waitForObject(":Open ground source.Open_QPushButton"))
    #  Now edit pt h5125_0000_1, which does not exist on current ground source
    waitForObjectItem(":Navigator_List", "h5125\\_0000\\_1")
    clickItem(":Navigator_List", "h5125\\_0000\\_1", 50, 4, 0, Qt.LeftButton)
    sendEvent("QMouseEvent", waitForObject(":Control Network Navigator.Modify Point_QPushButton"), QEvent.MouseButtonPress, 46, 7, Qt.LeftButton, 1, 0)
    sendEvent("QMouseEvent", waitForObject(":Control Network Navigator.Modify Point_QPushButton"), QEvent.MouseButtonRelease, 46, 6, Qt.LeftButton, 0, 0)
    #  Test for warning message
    test.vp("VP1")
    clickButton(waitForObject(":Warning.OK_QPushButton"))
    #  Now re-open pt h1104, close ground source window and screen shot right measure to make sure ground source measure closed
    waitForObjectItem(":Navigator_List", "h1104\\_0000\\_2")
    clickItem(":Navigator_List", "h1104\\_0000\\_2", 29, 6, 0, Qt.LeftButton)
    clickButton(waitForObject(":Control Network Navigator.Modify Point_QPushButton"))
    mouseClick(waitForObject(":qnet.f637a69.lev1_slo.cub @ 41.196% (Gray = 1)_Isis::ViewportMdiSubWindow"), 522, 20, 0, Qt.LeftButton)
    test.vp("VP2")
    #  Open pt h5125, and appropriate ground source and screen shot of right measure 
    waitForObjectItem(":Navigator_List", "h5125\\_0000\\_1")
    clickItem(":Navigator_List", "h5125\\_0000\\_1", 24, 8, 0, Qt.LeftButton)
    clickButton(waitForObject(":Control Network Navigator.Modify Point_QPushButton"))
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open Ground Source"))
    mouseClick(waitForObject(":Open ground source.lookInCombo_QComboBox"), 276, 9, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Open ground source.lookInCombo_QComboBox", "/tmp/Work/qnet\\_1491\\_1493\\_1655/isis/src/qisis/tsts/SquishTests/input/qnetMultipleGroundSource"), 224, 6, 0, Qt.LeftButton)
    waitForObjectItem(":stackedWidget.treeView_QTreeView_3", "f637a22\\.lev1\\_slo\\.cub")
    clickItem(":stackedWidget.treeView_QTreeView_3", "f637a22\\.lev1\\_slo\\.cub", 57, 6, 0, Qt.LeftButton)
    clickButton(waitForObject(":Open ground source.Open_QPushButton"))
    test.vp("VP3")
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))

    activateItem(waitForObjectItem(":qnet.File_QMenu", "Exit"))

