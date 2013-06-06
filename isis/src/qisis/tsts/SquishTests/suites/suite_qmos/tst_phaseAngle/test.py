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
    activateItem(waitForObjectItem(":qmos_QMenuBar", "View"))
    activateItem(waitForObjectItem(":qmos.View_QMenu", "Show Phase Angle Column"))
    setWindowState(waitForObject(":qmos_Isis::MosaicMainWindow"), WindowState.Normal)
    test.vp("Phase Angle Value")
    
    # Verify load project->cancel doesn't affect anything
    activateItem(waitForObjectItem(":qmos_QMenuBar", "File"))
    activateItem(waitForObjectItem(":_QMenu", "Load Project..."))
    clickButton(waitForObject(":Load Project.Cancel_QPushButton"))
    test.vp("Phase Angle Value")
    
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
             
