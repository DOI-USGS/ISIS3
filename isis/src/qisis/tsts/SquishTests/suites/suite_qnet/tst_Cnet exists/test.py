import os

def main():
    startApplication("qnet")
    
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open control network and cube list"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit"))
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/cube.lis")
    type(waitForObject(":_QListView"), "<Return>")

    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_7"), 100, 16, 0, Qt.LeftButton)
    waitForObjectItem(":stackedWidget.treeView_QTreeView_2", "moc3HandLockPoint1\\.net")



    doubleClickItem(":stackedWidget.treeView_QTreeView_2", "moc3HandLockPoint1\\.net", 58, 5, 0, Qt.LeftButton)
    waitFor("object.exists(':Navigator_CP1')", 20000)
    test.compare(findObject(":Navigator_CP1").text, "ssides1")
    test.compare(findObject(":Navigator_CP1").toolTip, "2 image(s) in point")
    waitFor("object.exists(':Navigator_CP2')", 20000)
    test.compare(findObject(":Navigator_CP2").text, "ssides2")
    test.compare(findObject(":Navigator_CP2").toolTip, "2 image(s) in point")
    waitFor("object.exists(':Navigator_CP3')", 20000)
    test.compare(findObject(":Navigator_CP3").text, "ssides3")
    test.compare(findObject(":Navigator_CP3").toolTip, "2 image(s) in point")
    waitFor("object.exists(':Navigator_CP19')", 20000)
    test.compare(findObject(":Navigator_CP19").text, "ssides19")
    test.compare(findObject(":Navigator_CP19").selected, False)
    test.compare(findObject(":Navigator_CP19").toolTip, "2 image(s) in point")
    test.compare(findObject(":Navigator_CP19").enabled, True)
    waitFor("object.exists(':Navigator_CP37')", 20000)
    test.compare(findObject(":Navigator_CP37").toolTip, "3 image(s) in point")
    test.compare(findObject(":Navigator_CP37").editable, False)
    test.compare(findObject(":Navigator_CP37").text, "ssides37")
    test.compare(findObject(":Navigator_CP37").enabled, True)
    doubleClickItem(":Navigator_List", "ssides1", 46, 8, 0, Qt.LeftButton)
    snooze(0.5)
    test.vp("InitialQnetToolView")
    clickButton(waitForObject(":Qnet Tool.Geom_QRadioButton"))
    test.vp("GeomQnetToolView")
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    clickButton(waitForObject(":qnet.Exit_QToolButton"))
    snooze(1)

