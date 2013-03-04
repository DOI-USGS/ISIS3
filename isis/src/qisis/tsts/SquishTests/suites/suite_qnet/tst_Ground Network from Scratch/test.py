import os

# This test is designed to hit a large portion of qnet's functionality, but also covers some of
# qview's functionality. This includes 2 different ways to create ground points, locking and
# ignoring points, setting apriori sigmas, filtering on point properties, and sorting measures.
def main():
    try:
        os.mkdir(os.path.expandvars('$ISISROOT/src/qisis/tsts/SquishTests/output'))
    except Exception:
        pass
    try:
        os.unlink(os.path.expandvars('$ISISROOT/src/qisis/tsts/SquishTests/output/') + 'GroundNetworkFromScratch.net')
    except Exception:
        pass
    
    startApplication("qnet")

    # Open Mini.lis / no network    
    clickButton(waitForObject(":qnet.Open control network  cube list_QToolButton"))
    clickButton(waitForObject(":Select a list of cubes.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit"), "src/qisis/tsts/SquishTests/input/Ground/Mini.lis")
    type(waitForObject(":_QListView"), "<Return>")

    clickButton(waitForObject(":Select a control network.Cancel_QPushButton"))
    
    # Open Ground Source
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open Ground Source"))
    clickButton(waitForObject(":Open ground source.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_2"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "src/qisis/tsts/SquishTests/input/Ground/Shade_Mola_Radius.cub")
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "<Return>")
    
    # Open Radius Source
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open Radius Source"))
    clickButton(waitForObject(":Open DEM.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_4"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_4"), "src/qisis/tsts/SquishTests/input/Ground/Mola_Radius.cub")
    type(waitForObject(":fileNameEdit_QLineEdit_4"), "<Return>")
    
    # Switch navigator to 'Cubes' from default of 'Points'
    mouseClick(waitForObject(":Control Network Navigator_QComboBox"), 129, 9, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Control Network Navigator_QComboBox", "Cubes"), 118, 7, 0, Qt.LeftButton)

    # Highlight (shift-select) all cubes and click view cubes in navigator
    waitForObjectItem(":Navigator_List", "I01812006RDR\\.cub")
    clickItem(":Navigator_List", "I01812006RDR\\.cub", 115, 6, 0, Qt.LeftButton)
    type(waitForObject(":Navigator_List"), "<Shift>")
    waitForObjectItem(":Navigator_List", "I26586032RDR\\.cub")
    clickItem(":Navigator_List", "I26586032RDR\\.cub", 111, 2, 33554432, Qt.LeftButton)
    clickButton(waitForObject(":Control Network Navigator.View Cube(s)_QPushButton"))
    
    # Link All and verify it happened
    activateItem(waitForObjectItem(":qnet_QMenuBar", "Window"))
    activateItem(waitForObjectItem(":qnet.Window_QMenu", "Link All"))
    
    # Attempt verification of viewports being linked
    waitFor("object.exists(':qnet.Link_QToolButton')", 20000)
    test.compare(findObject(":qnet.Link_QToolButton").checked, True)
    mouseDrag(waitForObject(":qnet.I18649010RDR.cub @ 6.90423% (Gray = 1)_Isis::ViewportMdiSubWindow"), 288, 10, 1064, 0, 1, Qt.LeftButton)
    waitFor("object.exists(':qnet.Link_QToolButton')", 20000)
    test.compare(findObject(":qnet.Link_QToolButton").checked, True)
    
    # Link all via keyboard shortcuts
    type(waitForObject(":qnet_Isis::MdiCubeViewport"), "<Ctrl+Shift+U>")
    type(waitForObject(":qnet_Isis::MdiCubeViewport"), "<Ctrl+Shift+L>")
    
    # Select the mola cube, open the find tool, enter a sample/line
    mouseClick(waitForObject(":qnet.Shade_Mola_Radius.cub @ 1.57881% (Gray = 1)_Isis::ViewportMdiSubWindow"), 305, 11, 0, Qt.LeftButton)
    clickButton(waitForObject(":qnet_QToolButton"))
    clickButton(waitForObject(":qnet_QToolButton_2"))
    mouseClick(waitForObject(":Sample_QLineEdit"), 77, 12, 0, Qt.LeftButton)
    type(waitForObject(":Sample_QLineEdit"), "3148")
    mouseClick(waitForObject(":Line_QLineEdit"), 78, 15, 0, Qt.LeftButton)
    type(waitForObject(":Line_QLineEdit"), "13608")
    clickButton(waitForObject(":Find Latitude/Longitude Coordinate.Ok_QPushButton"))
    clickButton(waitForObject(":Find Latitude/Longitude Coordinate.Close_QPushButton"))

    # Switch back to qnet tool, right click on radius cube to create point
    sendEvent("QWheelEvent", waitForObject(":qnet.viewport_QWidget_5"), 249, 246, 120, 0, 1)
    mouseClick(waitForObject(":qnet.viewport_QWidget_5"), 249, 246, 0, Qt.MidButton)
    clickButton(waitForObject(":qnet_QToolButton_3"))
    clickButton(waitForObject(":qnet_QToolButton_4"))
    clickButton(waitForObject(":qnet_QToolButton_5"))
    clickButton(waitForObject(":qnet.Statistics_QToolButton_2"))
    mouseClick(waitForObject(":qnet.viewport_QWidget_5"), 238, 260, Qt.NoModifier, Qt.RightButton)

    # Name the new point "grnd1"
    type(waitForObject(":Point ID:_QLineEdit"), "grnd1")
    clickButton(waitForObject(":Create Fixed or Constrained ControlPoint.OK_QPushButton"))

    # Switch point view to geom
    clickButton(waitForObject(":Qnet Tool.Geom_QRadioButton"))

    # Adjust and save measure
    mouseClick(waitForObject(":VP2"), 155, 150, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))

    # Change left to radius cube, right to I18337016RDR, find and save
    mouseClick(waitForObject(":Left Measure_QComboBox"), 162, 16, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Left Measure_QComboBox", "Shade\\_Mola\\_Radius\\.cub"), 155, 4, 0, Qt.LeftButton)
    mouseClick(waitForObject(":Right Measure_QComboBox"), 151, 8, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "I18337016RDR\\.cub"), 147, 9, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Find_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))

    # Confirm
    clickButton(waitForObject(":Qnet Tool Save Measure.Yes_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))

    # Switch right to "I19273007RD", find, register, save
    mouseClick(waitForObject(":Right Measure_QComboBox"), 152, 4, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "I19273007RDR\\.cub"), 144, 7, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Find_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Register_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))

    # Switch right to "I26586032RDR", find, register, OK on error, save, register, OK on error, save
    mouseClick(waitForObject(":Right Measure_QComboBox"), 156, 8, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "I26586032RDR\\.cub"), 141, 7, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Find_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Register_QPushButton"))
    clickButton(waitForObject(":Error.OK_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Register_QPushButton"))
    clickButton(waitForObject(":Error.OK_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))

    # Edit lock the point and save
    clickButton(waitForObject(":Control Point.Edit Lock Point_QCheckBox"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))
    clickButton(waitForObject(":qnet_QToolButton"))
    sendEvent("QWheelEvent", waitForObject(":Isis::ViewportMdiSubWindow - I26586032RDR.cub"), 269, 329, 120, 0, 1)
    mouseClick(waitForObject(":Isis::ViewportMdiSubWindow - I26586032RDR.cub"), 269, 329, 0, Qt.MidButton)
    clickButton(waitForObject(":qnet.Statistics_QToolButton_2"))

    # Create a point on I26586032RDR on smaller crater below large crater in view
    snooze(0.5)
    nativeMouseClick(waitForObject(":Isis::ViewportMdiSubWindow - I26586032RDR.cub"), 350, 25, Qt.NoModifier, Qt.LeftButton)
    snooze(0.5)

    mouseClick(waitForObject(":Isis::CubeViewport - I26586032RDR.cub"), 270, 335, Qt.NoModifier, Qt.RightButton)

    # Name the new point "grnd2"
    mouseClick(waitForObject(":Point ID:_QLineEdit_2"), 55, 6, 0, Qt.LeftButton)
    type(waitForObject(":Point ID:_QLineEdit_2"), "grnd2")
    clickButton(waitForObject(":Create New ControlPoint.OK_QPushButton"))

    # Switch right measure to "I26586032RDR", adjust and save.
    mouseClick(waitForObject(":Right Measure_QComboBox"), 298, 9, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "I26586032RDR\\.cub"), 286, 6, 0, Qt.LeftButton)
    mouseClick(waitForObject(":VP2"), 149, 153, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))

    # Switch right measure to "I18337016RDR", find and save
    mouseClick(waitForObject(":Right Measure_QComboBox"), 314, 11, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "I18337016RDR\\.cub"), 310, 4, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Find_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))

    # Switch right measure to "I19273007RDR", find and save
    mouseClick(waitForObject(":Right Measure_QComboBox"), 218, 11, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "I19273007RDR\\.cub"), 209, 5, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Find_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))
    mouseClick(waitForObject(":Control Point.PointType:_QComboBox"), 19, 9, 0, Qt.LeftButton)

    # Change CP type to constrained, save, verify apriori text fields are correct
    mouseClick(waitForObjectItem(":Control Point.PointType:_QComboBox", "Constrained"), 33, 3, 0, Qt.LeftButton)
    mouseClick(waitForObject(":VP2"), 158, 155, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))
    waitFor("object.exists(':Control Point.Apriori Latitude QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Latitude QLabel").text, "Apriori Latitude:  14.7617")
    waitFor("object.exists(':Control Point.Apriori Longitude QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Longitude QLabel").text, "Apriori Longitude:  300.416")
    waitFor("object.exists(':Control Point.Apriori Radius QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Radius QLabel").text, "Apriori Radius:  3393992.75 <meters>")
    waitFor("object.exists(':Control Point.Apriori Latitude Sigma QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Latitude Sigma QLabel").text, "Apriori Latitude Sigma:  Null")
    waitFor("object.exists(':Control Point.Apriori Longitude Sigma QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Longitude Sigma QLabel").text, "Apriori Longitude Sigma:  Null")
    waitFor("object.exists(':Control Point.Apriori Radius Sigma QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Radius Sigma QLabel").text, "Apriori Radius Sigma:  Null")

    # Switch navigator back from cubes to points
    mouseClick(waitForObject(":Control Network Navigator_QComboBox"), 203, 12, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Control Network Navigator_QComboBox", "Points"), 192, 3, 0, Qt.LeftButton)
    waitForObjectItem(":Navigator_List", "grnd1")

    # Highlight grnd1
    clickItem(":Navigator_List", "grnd1", 60, 4, 67108864, Qt.LeftButton)

    # Set apriori/sigmas on grnd1
    clickButton(waitForObject(":Control Network Navigator.Set Apriori/Sigmas_QPushButton"))
    waitForObjectItem(":EditLocked Points.editLockPointsListBox_QListWidget", "grnd1")
    clickItem(":EditLocked Points.editLockPointsListBox_QListWidget", "grnd1", 7, 4, 0, Qt.LeftButton)
    mouseClick(waitForObject(":latSigmaEdit_QLineEdit"), 46, 17, 0, Qt.LeftButton)
    type(waitForObject(":latSigmaEdit_QLineEdit"), "100")
    mouseClick(waitForObject(":lonSigmaEdit_QLineEdit"), 20, 5, 0, Qt.LeftButton)
    type(waitForObject(":lonSigmaEdit_QLineEdit"), "200")
    mouseClick(waitForObject(":radiusSigmaEdit_QLineEdit"), 32, 5, 0, Qt.LeftButton)
    type(waitForObject(":radiusSigmaEdit_QLineEdit"), "300")
    clickButton(waitForObject(":Set Apriori Point and Constraints.Set Apriori_QPushButton"))
    clickButton(waitForObject(":Set Apriori Point and Constraints.Close_QPushButton"))

    # Open "grnd1" (grnd2 was open)
    waitForObjectItem(":Navigator_List", "grnd1")
    clickItem(":Navigator_List", "grnd1", 32, 3, 0, Qt.LeftButton)
    waitForObjectItem(":Navigator_List", "grnd1")
    doubleClickItem(":Navigator_List", "grnd1", 32, 7, 0, Qt.LeftButton)

    # Verify apriori values are correct
    waitFor("object.exists(':Control Point.Apriori Latitude QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Latitude QLabel").text, "Apriori Latitude:  14.9882")
    waitFor("object.exists(':Control Point.Apriori Longitude QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Longitude QLabel").text, "Apriori Longitude:  300.317")
    waitFor("object.exists(':Control Point.Apriori Radius QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Radius QLabel").text, "Apriori Radius:  3393365.40 <meters>")
    waitFor("object.exists(':Control Point.Apriori Latitude Sigma QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Latitude Sigma QLabel").text, "Apriori Latitude Sigma:  100 <meters>")
    waitFor("object.exists(':Control Point.Apriori Longitude Sigma QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Longitude Sigma QLabel").text, "Apriori Longitude Sigma:  200 <meters>")
    waitFor("object.exists(':Control Point.Apriori Radius Sigma QLabel')", 20000)
    test.compare(findObject(":Control Point.Apriori Radius Sigma QLabel").text, "Apriori Radius Sigma:  300 <meters>")

    # Re-open "grnd2"
    waitForObjectItem(":Navigator_List", "grnd2")
    doubleClickItem(":Navigator_List", "grnd2", 35, 7, 0, Qt.LeftButton)

    # Edit lock "grnd2" and save
    clickButton(waitForObject(":Control Point.Edit Lock Point_QCheckBox"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))
    waitForObjectItem(":Navigator_List", "grnd1")

    # Highlight "grnd1" in navigator and ignore, expect edit locked error
    clickItem(":Navigator_List", "grnd1", 29, 1, 67108864, Qt.LeftButton)
    clickButton(waitForObject(":Control Network Navigator.Ignore Points_QPushButton"))
    clickButton(waitForObject(":Control Network Navigator - Ignore Points.Yes_QPushButton"))
    clickButton(waitForObject(":EditLocked Points.OK_QPushButton"))

    # Highlight "grnd2" in navigator and open
    waitForObjectItem(":Navigator_List", "grnd2")
    clickItem(":Navigator_List", "grnd2", 25, 7, 0, Qt.LeftButton)
    waitForObjectItem(":Navigator_List", "grnd2")
    doubleClickItem(":Navigator_List", "grnd2", 25, 7, 0, Qt.LeftButton)

    # Ignore point, expect error due to being edit locked
    clickButton(waitForObject(":Control Point.Ignore Point_QCheckBox"))
    clickButton(waitForObject(":Error.OK_QPushButton"))

    # Undo edit lock, save, ignore, save
    clickButton(waitForObject(":Control Point.Edit Lock Point_QCheckBox"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))
    clickButton(waitForObject(":Control Point.Ignore Point_QCheckBox"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))
    clickButton(waitForObject(":Control Network Navigator_QToolButton"))
    clickButton(waitForObject(":Control Network Navigator_QToolButton"))

    # In navigator, go to point properties filter
    clickTab(waitForObject(":Control Network Navigator.qt_tabwidget_tabbar_QTabBar"), "Point Properties")

    # Filter by ignored
    mouseClick(waitForObject(":Filter by Point Type(s)_QGroupBox"), 11, 5, 0, Qt.LeftButton)
    mouseClick(waitForObject(":Filter by Ignore Status_QGroupBox"), 18, 2, 0, Qt.LeftButton)
    clickButton(waitForObject(":Control Network Navigator.Filter_QPushButton"))

    # Verify both grnd1 and grnd2 still listed
    waitFor("object.exists(':grnd1_QModelIndex')", 20000)
    test.compare(findObject(":grnd1_QModelIndex").text, "grnd1")
    waitFor("object.exists(':grnd2_QModelIndex')", 20000)
    test.compare(findObject(":grnd2_QModelIndex").text, "grnd2")
    waitForObjectItem(":Navigator_List", "grnd1")

    # Open "grnd1" and edit lock/save
    doubleClickItem(":Navigator_List", "grnd1", 9, 6, 0, Qt.LeftButton)
    clickButton(waitForObject(":Control Point.Edit Lock Point_QCheckBox"))
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))

    # Filter by edit locked in navigator, verify only grnd1 listed
    mouseClick(waitForObject(":Filter by Ignore Status_QGroupBox"), 14, 5, 0, Qt.LeftButton)
    mouseClick(waitForObject(":Filter by Edit Lock Status_QGroupBox"), 14, 5, 0, Qt.LeftButton)
    clickButton(waitForObject(":Control Network Navigator.Filter_QPushButton"))
    waitFor("object.exists(':grnd1_QModelIndex')", 20000)
    test.compare(findObject(":grnd1_QModelIndex").text, "grnd1")
    waitFor("object.exists(':Navigator_List')", 20000)
    test.compare(findObject(":Navigator_List").count, 1)
    
    # Save resulting network
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Save Control Network As..."))
    clickButton(waitForObject(":Choose filename to save under.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_3"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), "src/qisis/tsts/SquishTests/output/GroundNetworkFromScratch.net")
    snooze(0.5)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), "<Return>")
    
    # Exit 
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Exit"))
    snooze(1)
