import os

def main():
    try:
        os.mkdir(os.path.expandvars('$ISISROOT/src/qisis/tsts/SquishTests/output'))
    except Exception:
        pass
    try:
        os.unlink(os.path.expandvars('$ISISROOT/src/qisis/tsts/SquishTests/output/') + 'MiniUpdate2.net')
    except Exception:
        pass
    try:
        os.unlink(os.path.expandvars('$ISISROOT/src/qisis/tsts/SquishTests/output/') + 'reg.def.txt')
    except Exception:
        pass

    
    startApplication("qnet")
    
    # Open cube list
    clickButton(waitForObject(":qnet.Open control network  cube list_QToolButton"))
    clickButton(waitForObject(":Select a list of cubes.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit"), "src/qisis/tsts/SquishTests/input/Ground/Extracted_AllOverlaps.lis")
    type(waitForObject(":_QListView"), "<Return>")

    # Open control net
    snooze(0.5)
    mouseClick(waitForObject(":stackedWidget.treeView_QTreeView_2"), 95, 13, 0, Qt.LeftButton)
    waitForObjectItem(":stackedWidget.treeView_QTreeView_2", "Mini\\.net")
    doubleClickItem(":stackedWidget.treeView_QTreeView_2", "Mini\\.net", 58, 5, 0, Qt.LeftButton)
    
    # Open ground source file
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open Ground Source"))
    clickButton(waitForObject(":Open ground source.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_2"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "src/qisis/tsts/SquishTests/input/Ground/Shade_Mola_Radius.cub")
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "<Return>")
    
    # Open radius source file
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Open Radius Source"))
    clickButton(waitForObject(":Open DEM.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_4"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_4"), "src/qisis/tsts/SquishTests/input/Ground/Mola_Radius.cub")
    type(waitForObject(":fileNameEdit_QLineEdit_4"), "<Return>")
    
    # Create measure from ground source file
    mouseClick(waitForObject(":qnet.viewport_QWidget"), 235, 337, Qt.NoModifier, Qt.RightButton)
    type(waitForObject(":Point ID:_QLineEdit"), "newpoint001")
    type(waitForObject(":Point ID:_QLineEdit"), "<Return>")
    
    # Edit Measure - Update using registration templates then register measures
    mouseClick(waitForObject(":Right Measure_QComboBox"), 83, 17, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "I08528020RDR\\.cub"), 83, 8, 0, Qt.LeftButton)
    mouseClick(waitForObject(":VP2"), 41, 159, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    mouseClick(waitForObject(":Right Measure_QComboBox"), 77, 7, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "I27759024RDR\\.cub"), 80, 8, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Register_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.OK_QPushButton"))
    clickButton(waitForObject(":QnetTool.View/edit registration template_QToolButton"))
    clickButton(waitForObject(":QnetToolScroll.Open registration template_QToolButton"))
    
    # Open default registration template
    type(waitForObject(":fileNameEdit_QLineEdit_5"), "/usgs/cpkgs/isis3/data/base/templates/autoreg/qnetReg.def")
    type(waitForObject(":fileNameEdit_QLineEdit_5"), "<Return>")

    mouseClick(waitForObject(":QnetToolScroll.Template Editor ToolBar_QTextEdit"), 82, 98, 0, Qt.LeftButton)
    mouseClick(waitForObject(":QnetToolScroll.Template Editor ToolBar_QTextEdit"), 97, 217, 0, Qt.LeftButton)
    type(waitForObject(":QnetToolScroll.Template Editor ToolBar_QTextEdit"), "1")
    mouseClick(waitForObject(":QnetToolScroll.Template Editor ToolBar_QTextEdit"), 84, 236, 0, Qt.LeftButton)
    type(waitForObject(":QnetToolScroll.Template Editor ToolBar_QTextEdit"), "1")
    
    # Try to save to file we can't write to
    clickButton(waitForObject(":QnetToolScroll.Save template file_QToolButton"))
    clickButton(waitForObject(":IO Error.OK_QPushButton"))
    
    # Save as - this will be an output file
    clickButton(waitForObject(":QnetToolScroll.Save template as_QToolButton"))
    
    clickButton(waitForObject(":Save registration template.toParentButton_QToolButton"))
    type(waitForObject(":fileNameEdit_QLineEdit_6"), "src/qisis/tsts/SquishTests/output/reg.def.txt")
    type(waitForObject(":fileNameEdit_QLineEdit_6"), "<Return>")
    
    mouseClick(waitForObject(":QnetToolScroll.Template Editor ToolBar_QTextEdit"), 77, 158, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Register_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.OK_QPushButton"))
    mouseDrag(waitForObject(":QnetToolScroll.Template Editor ToolBar_QTextEdit"), 127, 52, 16, 3, 1, Qt.LeftButton)
    type(waitForObject(":QnetToolScroll.Template Editor ToolBar_QTextEdit"), "64")
    clickButton(waitForObject(":QnetToolScroll.Save template file_QToolButton"))
    clickButton(waitForObject(":QnetToolScroll.Register_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    mouseClick(waitForObject(":Right Measure_QComboBox"), 152, 14, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "Shade\\_Mola\\_Radius\\.cub"), 108, 5, 0, Qt.LeftButton)
    clickButton(waitForObject(":Qnet Tool.Geom_QRadioButton"))
    mouseClick(waitForObject(":Right Measure_QComboBox"), 171, 9, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "Shade\\_Mola\\_Radius\\.cub"), 112, 3, 0, Qt.LeftButton)
    mouseClick(waitForObject(":Right Measure_QComboBox"), 133, 13, 0, Qt.LeftButton)
    mouseClick(waitForObject(":QnetToolScroll_QWidget"), 920, 689, 0, Qt.LeftButton)
    mouseClick(waitForObject(":VP2"), 87, 118, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Register_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    mouseClick(waitForObject(":Right Measure_QComboBox"), 148, 1, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Right Measure_QComboBox", "Shade\\_Mola\\_Radius\\.cub"), 87, 4, 0, Qt.LeftButton)
    mouseClick(waitForObject(":QnetToolScroll_Isis::ControlPointEdit"), 357, 455, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Register_QPushButton"))
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    sendEvent("QWheelEvent", waitForObject(":VP2"), 166, 165, -120, 0, 2)
    clickButton(waitForObject(":QnetToolScroll.No geom/rotate_QRadioButton"))
    doubleClick(waitForObject(":QnetToolScroll_QToolButton"), 14, 17, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll_QToolButton"))
    clickButton(waitForObject(":QnetToolScroll_QToolButton_2"))
    clickButton(waitForObject(":QnetToolScroll_QToolButton_2"))
    clickButton(waitForObject(":QnetToolScroll_QToolButton_2"))
    clickButton(waitForObject(":QnetToolScroll_QToolButton_2"))
    mouseClick(waitForObject(":QnetToolScroll_QWidget"), 749, 1122, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))
    clickButton(waitForObject(":Control Network Navigator.View Cube(s)_QPushButton"))
    
    # Viewport must be active before doing the mouse click...
    nativeMouseClick(waitForObject(":Isis::ViewportMdiSubWindow - I27759024RDR.cub"), 150, 25, Qt.NoModifier, Qt.LeftButton)
    mouseClick(waitForObject(":qnet.viewport_QWidget_2"), 252, 232, Qt.NoModifier, Qt.RightButton)
    
    type(waitForObject(":Point ID:_QLineEdit_2"), "newpomt-")
    type(waitForObject(":Point ID:_QLineEdit_2"), "<Backspace>")
    type(waitForObject(":Point ID:_QLineEdit_2"), "<Backspace>")
    type(waitForObject(":Point ID:_QLineEdit_2"), "<Backspace>")
    type(waitForObject(":Point ID:_QLineEdit_2"), "int002")
    type(waitForObject(":Point ID:_QLineEdit_2"), "<Return>")
    mouseClick(waitForObject(":VP2"), 149, 83, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Save Measure_QPushButton"))
    mouseClick(waitForObject(":Right Measure_QComboBox"), 180, 5, 0, Qt.LeftButton)
    clickButton(waitForObject(":QnetToolScroll.Save Point_QPushButton"))
    waitForObjectItem(":Navigator_List", "newpoint001")
    clickItem(":Navigator_List", "newpoint001", 89, 8, 0, Qt.LeftButton)
    clickButton(waitForObject(":Control Network Navigator.Modify Point_QPushButton"))
    clickButton(waitForObject(":Control Network Navigator.Set Apriori/Sigmas_QPushButton"))
    mouseClick(waitForObject(":latSigmaEdit_QLineEdit"), 43, 6, 0, Qt.LeftButton)
    type(waitForObject(":latSigmaEdit_QLineEdit"), "100")
    type(waitForObject(":latSigmaEdit_QLineEdit"), "<Tab>")
    type(waitForObject(":Set Apriori Point and Constraints.Longitude Constraints_QGroupBox"), "<Tab>")
    type(waitForObject(":lonSigmaEdit_QLineEdit"), "100")
    type(waitForObject(":lonSigmaEdit_QLineEdit"), "<Tab>")
    type(waitForObject(":Set Apriori Point and Constraints.Radius Constraints_QGroupBox"), "<Tab>")
    type(waitForObject(":radiusSigmaEdit_QLineEdit"), "500")
    clickButton(waitForObject(":Set Apriori Point and Constraints.Set Apriori_QPushButton"))
    sendEvent("QMoveEvent", waitForObject(":Set Apriori Point and Constraints_QnetSetAprioriDialog"), 4065, 790, 4171, 796)
    
    # Viewport must be active before clicking on it
    nativeMouseClick(waitForObject(":Isis::ViewportMdiSubWindow - I08528020RDR.cub"), 150, 25, Qt.NoModifier, Qt.LeftButton)
    mouseClick(waitForObject(":qnet.viewport_QWidget_3"), 245, 325, Qt.NoModifier, Qt.RightButton)
    
    mouseClick(waitForObject(":Point ID:_QLineEdit_2"), 87, 15, 0, Qt.LeftButton)
    type(waitForObject(":Point ID:_QLineEdit_2"), "<Backspace>")
    type(waitForObject(":Point ID:_QLineEdit_2"), "3")
    clickButton(waitForObject(":Create New ControlPoint.OK_QPushButton"))
    
    # Save resulting network
    activateItem(waitForObjectItem(":Qnet Tool_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Save Control Network As..."))
    clickButton(waitForObject(":Choose filename to save under.toParentButton_QToolButton"))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_3"), 95, 13, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_3"), "src/qisis/tsts/SquishTests/output/MiniUpdate2.net")
    type(waitForObject(":fileNameEdit_QLineEdit_3"), "<Return>")
    
    activateItem(waitForObjectItem(":qnet_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qnet.File_QMenu", "Exit"))
    snooze(1)

