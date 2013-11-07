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
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/molaMarsPlanetaryRadius0005.crop.cub")
    clickButton(waitForObject(":Open.Open_QPushButton"))
    clickButton(waitForObject(":qview.Open_QToolButton"))
    snooze(0.5)
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/ulcn2005_clean_0001.crop.cub")
    clickButton(waitForObject(":Open.Open_QPushButton"))
    clickButton(waitForObject(":qview.Open_QToolButton"))
    snooze(0.5)
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/approvalStatusTest.cub")
    clickButton(waitForObject(":Open.Open_QPushButton"))

    # Go to the nomenclature tool
    clickButton(waitForObject(":nomenclatureToolButton"))

    # OK the disclaimer
    waitFor("object.exists(':Nomenclature Disclaimer_QMessageBox')", 20000)
    test.compare(findObject(":Nomenclature Disclaimer_QMessageBox").visible, True)
    test.compare(findObject(":Nomenclature Disclaimer_QMessageBox").text, "The nomenclature qview tool will label named features in your opened cube files. This tool <strong>requires</strong> an active internet connection, projection or camera information, and a calculatable ground range to function. The larger the ground range (covered area on a planet), the longer it will take to populate the nomenclature for a particular cube.<br/><br/><font color='red'>**WARNING**</font> The accuracy of this tool is not perfect, features <strong>can and will be mislabeled</strong> if you have not properly controlled your images to the control network that identifies the latitude/longitude values of a feature. Please use the nomenclature website to verify a label is correct for a feature. <br/><br/>See the IAU Gazetteer of Planetary Nomenclature website for more information.<br/><a href='http://planetarynames.wr.usgs.gov/'>http://planetarynames.wr.usgs.gov/</a>")
    clickButton(waitForObject(":Nomenclature Disclaimer.OK_QPushButton"))


    # Re-visit the disclaimer
    clickButton(waitForObject(":qview.Disclaimer_QPushButton"))
    test.compare(findObject(":Nomenclature Disclaimer_QMessageBox").visible, True)
    clickButton(waitForObject(":Nomenclature Disclaimer.OK_QPushButton"))

    # Now wait for viewports to finish loading and nomenclature query to finish
    waitFor("object.exists(':viewport1_progress')")
    waitFor('findObject(":viewport1_progress").text == \"100%\"')
    waitFor("object.exists(':viewport2_progress')")
    waitFor('findObject(":viewport2_progress").text == \"100%\"')
    waitFor("object.exists(':qview.nomenclatureQueryProgress')")
    waitFor('findObject(":qview.nomenclatureQueryProgress").visible == 0')
    waitFor("object.exists(':ProgressDialog') == 0")
    
    # Verify initial nomenclature is correct
    test.vp("viewport1-mola")
    test.vp("viewport2-ulcn")
    
    # Try out various configurations
    clickButton(waitForObject(":qview.Tool Options_QPushButton"))
    
    # New font size
    mouseClick(waitForObject(":Active Tool.Font Size_QComboBox"), 45, 14, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Active Tool.Font Size_QComboBox", "8"), 23, 11, 0, Qt.LeftButton)
    clickButton(waitForObject(":Active Tool.Apply_QPushButton"))
    test.vp("viewport1_changedfontsize")
    test.vp("viewport2_changedfontsize")
    
    # New font color
    clickButton(waitForObject(":Active Tool.Font Color_QPushButton"))

    mouseDrag(waitForObject(":Select Color.Basic colors_QColorPicker"), 79, 132, 93, -89, 1, Qt.LeftButton)
    mouseDrag(waitForObject(":Select Color_QColorDialog"), 395, 318, 0, 4, 1, Qt.LeftButton)
    clickButton(waitForObject(":Select Color.OK_QPushButton"))
    clickButton(waitForObject(":Active Tool.Apply_QPushButton"))
    test.vp("viewport1_fontcolor")
    test.vp("viewport2_fontcolor")
    
    # Go through extent types
    mouseClick(waitForObject(":Active Tool.Show feature extents_QComboBox"), 50, 1, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Active Tool.Show feature extents_QComboBox", "4 Arrows"), 33, 4, 0, Qt.LeftButton)
    clickButton(waitForObject(":Active Tool.Apply_QPushButton"))
    test.vp("viewport1_4arrows")
    test.vp("viewport2_4arrows")
    
    mouseClick(waitForObject(":Active Tool.Show feature extents_QComboBox"), 44, 10, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Active Tool.Show feature extents_QComboBox", "8 Arrows"), 31, 5, 0, Qt.LeftButton)
    clickButton(waitForObject(":Active Tool.Apply_QPushButton"))
    test.vp("viewport1_8arrows")
    test.vp("viewport2_8arrows")
    
    mouseClick(waitForObject(":Active Tool.Show feature extents_QComboBox"), 48, 11, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Active Tool.Show feature extents_QComboBox", "Box"), 28, 8, 0, Qt.LeftButton)
    clickButton(waitForObject(":Active Tool.Apply_QPushButton"))
    test.vp("viewport1_box")
    test.vp("viewport2_box")
    test.vp("viewport3_approvedonly")


    # Try IAU toggle
    clickButton(waitForObject(":Active Tool.Show IAU approved only_QCheckBox"))
    clickButton(waitForObject(":Active Tool.Apply_QPushButton"))
    waitFor("object.exists(':ProgressDialog') == 0")
    test.vp("viewport1_unapproved")
    test.vp("viewport2_unapproved")
    test.vp("viewport3_unapproved")
    
    clickButton(waitForObject(":Active Tool.Ok_QPushButton"))
    
    os.system("qview ../../../input/I26649003RDR.crop.tr.lev2.cub")
    
    waitFor("object.exists(':viewport4_progress')")
    waitFor('findObject(":viewport4_progress").text == \"100%\"')
    snooze(1.0)
    waitFor("object.exists(':qview.nomenclatureQueryProgress')")
    waitFor('findObject(":qview.nomenclatureQueryProgress").visible == 0')
    waitFor("object.exists(':ProgressDialog') == 0")
    snooze(1.5)
    test.vp("viewport4_autosearch")
    
    # Finished with initial run
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


