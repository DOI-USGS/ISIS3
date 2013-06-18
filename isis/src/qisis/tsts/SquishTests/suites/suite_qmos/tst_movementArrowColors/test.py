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
    activateItem(waitForObjectItem(":qmos.File_QMenu", "Load Project..."))
    snooze(0.5)
    mouseClick(waitForObject(":fileNameEdit_QLineEdit_2"), 149, 11, 0, Qt.LeftButton)
    type(waitForObject(":fileNameEdit_QLineEdit_2"), "../src/qisis/tsts/SquishTests/input/small_project.mos")

    type(waitForObject(":_QListView"), "<Return>")
    snooze(3)
    test.vp("MosaicScene_ColoredArrows")
    
    waitForObject(":qmos_MosaicSceneWidget")
    
    waitForObject(":THM_AEOLIS_auto_035068")

    #openContextMenu(":qmos_MosaicSceneWidget", 225, 200, 0)
    openContextMenu(":THM_AEOLIS_auto_035068", 50, 25, 0)

    activateItem(waitForObjectItem(":_QMenu", "Show Point Info"))
    

    waitFor("object.exists(':Control Point Information.<div>Point ID: THM_AEOLIS_auto_035068<br />Point Type: Free<br />Number of Measures: 17<br />Ignored: No<br />Edit Locked: No<br />Odyssey/THEMIS_IR/763079637.076 [residual: <font color=\\'red\\'>0.064050352600319</font>]<br />Odyssey/THEMIS_IR/760860152.204 [residual: <font color=\\'red\\'>0.19328122482455</font>]<br />Odyssey/THEMIS_IR/795837905.025 [residual: <font color=\\'red\\'>0.06646671233399</font>]<br />Odyssey/THEMIS_IR/743903678.230 [residual: <font color=\\'red\\'>0.4287836943445</font>]<br />Odyssey/THEMIS_IR/859135323.128 [residual: <font color=\\'red\\'>0.24759365861102</font>]<br />Odyssey/THEMIS_IR/728100510.128 [residual: <font color=\\'red\\'>0.22138667256272</font>]<br />Odyssey/THEMIS_IR/725348214.153 [residual: <font color=\\'red\\'>0.15371356223479</font>]<br />Odyssey/THEMIS_IR/706792855.230 [residual: <font color=\\'red\\'>0.27622761966836</font>]<br />Odyssey/THEMIS_IR/711942122.000 [residual: <font color=\\'red\\'>0.24848443615479</font>]<br />Odyssey/THEMIS_IR/798057327.153 [residual: <font color=\\'red\\'>0.068010234920038</font>]<br />Odyssey/THEMIS_IR/704218148.204 [residual: <font color=\\'red\\'>0.054820451973017</font>]<br />Odyssey/THEMIS_IR/776218444.204 [residual: <font color=\\'red\\'>0.38205196382913</font>]<br />Odyssey/THEMIS_IR/828596080.230 [residual: <font color=\\'red\\'>0.20693319566613</font>]<br />Odyssey/THEMIS_IR/765298995.000 [residual: <font color=\\'red\\'>0.092469166851159</font>]<br />Odyssey/THEMIS_IR/817676676.076 [residual: <font color=\\'red\\'>0.087222014470049</font>]<br />Odyssey/THEMIS_IR/746478164.204 [residual: <font color=\\'red\\'>0.33879874333558</font>]<br />Odyssey/THEMIS_IR/819896153.230 [residual: <font color=\\'red\\'>0.24197426398359</font>]</div>_QLabel')", 20000)
    test.compare(findObject(":Control Point Information.<div>Point ID: THM_AEOLIS_auto_035068<br />Point Type: Free<br />Number of Measures: 17<br />Ignored: No<br />Edit Locked: No<br />Odyssey/THEMIS_IR/763079637.076 [residual: <font color='red'>0.064050352600319</font>]<br />Odyssey/THEMIS_IR/760860152.204 [residual: <font color='red'>0.19328122482455</font>]<br />Odyssey/THEMIS_IR/795837905.025 [residual: <font color='red'>0.06646671233399</font>]<br />Odyssey/THEMIS_IR/743903678.230 [residual: <font color='red'>0.4287836943445</font>]<br />Odyssey/THEMIS_IR/859135323.128 [residual: <font color='red'>0.24759365861102</font>]<br />Odyssey/THEMIS_IR/728100510.128 [residual: <font color='red'>0.22138667256272</font>]<br />Odyssey/THEMIS_IR/725348214.153 [residual: <font color='red'>0.15371356223479</font>]<br />Odyssey/THEMIS_IR/706792855.230 [residual: <font color='red'>0.27622761966836</font>]<br />Odyssey/THEMIS_IR/711942122.000 [residual: <font color='red'>0.24848443615479</font>]<br />Odyssey/THEMIS_IR/798057327.153 [residual: <font color='red'>0.068010234920038</font>]<br />Odyssey/THEMIS_IR/704218148.204 [residual: <font color='red'>0.054820451973017</font>]<br />Odyssey/THEMIS_IR/776218444.204 [residual: <font color='red'>0.38205196382913</font>]<br />Odyssey/THEMIS_IR/828596080.230 [residual: <font color='red'>0.20693319566613</font>]<br />Odyssey/THEMIS_IR/765298995.000 [residual: <font color='red'>0.092469166851159</font>]<br />Odyssey/THEMIS_IR/817676676.076 [residual: <font color='red'>0.087222014470049</font>]<br />Odyssey/THEMIS_IR/746478164.204 [residual: <font color='red'>0.33879874333558</font>]<br />Odyssey/THEMIS_IR/819896153.230 [residual: <font color='red'>0.24197426398359</font>]</div>_QLabel").text, "<div>Point ID: THM_AEOLIS_auto_035068<br />Point Type: Free<br />Number of Measures: 17<br />Ignored: No<br />Edit Locked: No<br />Odyssey/THEMIS_IR/763079637.076 [residual: <font color='red'>0.064050352600319</font>]<br />Odyssey/THEMIS_IR/760860152.204 [residual: <font color='red'>0.19328122482455</font>]<br />Odyssey/THEMIS_IR/795837905.025 [residual: <font color='red'>0.06646671233399</font>]<br />Odyssey/THEMIS_IR/743903678.230 [residual: <font color='red'>0.4287836943445</font>]<br />Odyssey/THEMIS_IR/859135323.128 [residual: <font color='red'>0.24759365861102</font>]<br />Odyssey/THEMIS_IR/728100510.128 [residual: <font color='red'>0.22138667256272</font>]<br />Odyssey/THEMIS_IR/725348214.153 [residual: <font color='red'>0.15371356223479</font>]<br />Odyssey/THEMIS_IR/706792855.230 [residual: <font color='red'>0.27622761966836</font>]<br />Odyssey/THEMIS_IR/711942122.000 [residual: <font color='red'>0.24848443615479</font>]<br />Odyssey/THEMIS_IR/798057327.153 [residual: <font color='red'>0.068010234920038</font>]<br />Odyssey/THEMIS_IR/704218148.204 [residual: <font color='red'>0.054820451973017</font>]<br />Odyssey/THEMIS_IR/776218444.204 [residual: <font color='red'>0.38205196382913</font>]<br />Odyssey/THEMIS_IR/828596080.230 [residual: <font color='red'>0.20693319566613</font>]<br />Odyssey/THEMIS_IR/765298995.000 [residual: <font color='red'>0.092469166851159</font>]<br />Odyssey/THEMIS_IR/817676676.076 [residual: <font color='red'>0.087222014470049</font>]<br />Odyssey/THEMIS_IR/746478164.204 [residual: <font color='red'>0.33879874333558</font>]<br />Odyssey/THEMIS_IR/819896153.230 [residual: <font color='red'>0.24197426398359</font>]</div>")

    clickButton(waitForObject(":Control Point Information.OK_QPushButton"))

    clickButton(waitForObject(":qmos_ControlNetToolButton"))
    clickButton(waitForObject(":qmos.Configure Movement Display_QPushButton"))
    mouseClick(waitForObject(":Color Criteria_QComboBox"), 67, 15, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Color Criteria_QComboBox", "Measure Count"), 53, 2, 0, Qt.LeftButton)
    mouseDrag(waitForObject(":Min measure count to color_QLineEdit"), 28, 12, -16, 0, 1, Qt.LeftButton)
    type(waitForObject(":Min measure count to color_QLineEdit"), "5")
    clickButton(waitForObject(":Movement Options.Apply_QPushButton"))
    test.vp("MosaicScene_MeasureCountColoredArrows")
    mouseClick(waitForObject(":Color Criteria_QComboBox"), 81, 13, 0, Qt.LeftButton)
    mouseClick(waitForObjectItem(":Color Criteria_QComboBox", "No Color"), 63, 5, 0, Qt.LeftButton)
    clickButton(waitForObject(":Movement Options.Apply_QPushButton"))
    test.vp("MosaicScene_NoColorArrows")
    mouseClick(waitForObject(":Color Criteria_QComboBox"), 58, 11, 0, Qt.LeftButton)
    clickButton(waitForObject(":Show Movement_QCheckBox"))
    clickButton(waitForObject(":Movement Options.Apply_QPushButton"))
    test.vp("MosaicScene_NoArrows")
    
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
             
