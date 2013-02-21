import os                                                                                               
import shutil                                                                                           
      
def main():
    # Backup current qmos settings                                                                     
    try:                                                                                                
        shutil.rmtree(os.path.expandvars('$HOME/.Isis/qmos.squishbackup'))                             
    except Exception:                                                                                   
        pass                                                                                            
                                                                                                        
    try:                                                                                                
        os.rename(os.path.expandvars('$HOME/.Isis/qmos'), os.path.expandvars('$HOME/.Isis/qmos.squishbackup'))
    except Exception:                                                                                   
        pass
    
    startApplication("qmos")
    activateItem(waitForObjectItem(":qmos_QMenuBar", "File"))
    activateItem(waitForObjectItem(":qmos.File_QMenu", "Open Cube..."))
    snooze(0.5)
    type(waitForObject(":fileNameEdit_QLineEdit"), "../src/qisis/tsts/SquishTests/input/lub2675j.342.lev1.cub")
    type(waitForObject(":_QListView"), "<Return>")

    snooze(3)
    
    clickButton(waitForObject(":qmos_QToolButton_2"))
    
    
    clickButton(waitForObject(":qmos.Grid Options_QPushButton"))
    clickButton(waitForObject(":Auto Grid_QCheckBox"))
    mouseClick(waitForObject(":Grid Options.Latitude Range_QComboBox"), 127, 3, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Grid Options.Latitude Range_QComboBox", "Manual"), 53, 3, 0, Qt.LeftButton)
    mouseDrag(waitForObject(":Grid Options_QLineEdit"), 77, 10, -92, -5, 1, Qt.LeftButton)
    type(waitForObject(":Grid Options_QLineEdit"), "0")
    mouseDrag(waitForObject(":Grid Options_QLineEdit_2"), 73, 1, -101, 2, 1, Qt.LeftButton)
    type(waitForObject(":Grid Options_QLineEdit_2"), "2")
    mouseClick(waitForObject(":Grid Options.Longitude Range_QComboBox"), 126, 7, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Grid Options.Longitude Range_QComboBox", "Manual"), 67, 1, 0, Qt.LeftButton)
    mouseDrag(waitForObject(":Grid Options_QLineEdit_3"), 76, 10, -90, -7, 1, Qt.LeftButton)
    mouseDrag(waitForObject(":Grid Options_QLineEdit_3"), 74, 6, -74, 2, 1, Qt.LeftButton)
    type(waitForObject(":Grid Options_QLineEdit_3"), "-113")
    mouseDrag(waitForObject(":Grid Options_QLineEdit_4"), 65, 7, -67, -1, 1, Qt.LeftButton)
    type(waitForObject(":Grid Options_QLineEdit_4"), "-110")
    
    # This froze for mantis ticket #1060 - we're just checking that qmos doesn't lock up.
    clickButton(waitForObject(":Grid Options.Ok_QPushButton"))                                                           
    snooze(1)   

    sendEvent("QCloseEvent", waitForObject(":qmos_Isis::MosaicMainWindow"))
                                                                           
    snooze(1)                             
    # Restore original qmos settings                                                                   
    try:                                                                                                
        shutil.rmtree(os.path.expandvars('$HOME/.Isis/qmos'))                                          
    except Exception:                                                                                   
        pass                                                                                            
                                                                                                        
    try:                                                                                                
        os.rename(os.path.expandvars('$HOME/.Isis/qmos.squishbackup'), os.path.expandvars('$HOME/.Isis/qmos'))
    except Exception:                                                                                   
        pass                                                                                            
             