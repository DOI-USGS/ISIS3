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

    openContextMenu(":qmos_MosaicSceneWidget", 225, 200, 0)

    activateItem(waitForObjectItem(":_QMenu", "Show Point Info"))
    
    waitFor("object.exists(':Control Point Information.<div>Point ID: THM_AEOLIS_auto_035066<br />Point Type: Free<br />Number of Measures: 17<br />Ignored: No<br />Edit Locked: No<br />I03415002RDR.cub (Odyssey/THEMIS_IR/717091315.204) [residual: <font color=\\'red\\'>0.34334690243932</font>]<br />Odyssey/THEMIS_IR/760860152.204 [residual: <font color=\\'red\\'>0.26169018459583</font>]<br />I11104002RDR.cub (Odyssey/THEMIS_IR/771779441.102) [residual: <font color=\\'red\\'>0.41863100293464</font>]<br />I16021002RDR.cub (Odyssey/THEMIS_IR/806757253.179) [residual: <font color=\\'red\\'>0.46134858380318</font>]<br />Odyssey/THEMIS_IR/701643163.076 [residual: <font color=\\'red\\'>0.43597701233739</font>]<br />I04576001RDR.cub (Odyssey/THEMIS_IR/725348214.153) [residual: <font color=\\'red\\'>0.30914060868133</font>]<br />I09257002RDR.cub (Odyssey/THEMIS_IR/758640718.179) [residual: <font color=\\'red\\'>0.46836807082174</font>]<br />I11416002RDR.cub (Odyssey/THEMIS_IR/773998971.153) [residual: <font color=\\'red\\'>0.91150500491869</font>]<br />I12639002RDR.cub (Odyssey/THEMIS_IR/782699009.153) [residual: <font color=\\'red\\'>1.1778289552804</font>]<br />I02691002RDR.cub (Odyssey/THEMIS_IR/711942122.000) [residual: <font color=\\'red\\'>0.34370476346785</font>]<br />Odyssey/THEMIS_IR/824157276.000 [residual: <font color=\\'red\\'>0.18355145849252</font>]<br />Odyssey/THEMIS_IR/751627267.051 [residual: <font color=\\'red\\'>0.36767426891234</font>]<br />I11728002RDR.cub (Odyssey/THEMIS_IR/776218444.204) [residual: <font color=\\'red\\'>0.45722231944607</font>]<br />I19091002RDR.cub (Odyssey/THEMIS_IR/828596080.230) [residual: <font color=\\'red\\'>0.27598739181935</font>]<br />Odyssey/THEMIS_IR/817676676.076 [residual: <font color=\\'red\\'>0.27221605600613</font>]<br />Odyssey/THEMIS_IR/830815439.153 [residual: <font color=\\'red\\'>0.74485039589228</font>]<br />Odyssey/THEMIS_IR/746478164.204 [residual: <font color=\\'red\\'>0.3135765348542</font>]</div>_QLabel')", 20000)
    test.compare(findObject(":Control Point Information.<div>Point ID: THM_AEOLIS_auto_035066<br />Point Type: Free<br />Number of Measures: 17<br />Ignored: No<br />Edit Locked: No<br />I03415002RDR.cub (Odyssey/THEMIS_IR/717091315.204) [residual: <font color='red'>0.34334690243932</font>]<br />Odyssey/THEMIS_IR/760860152.204 [residual: <font color='red'>0.26169018459583</font>]<br />I11104002RDR.cub (Odyssey/THEMIS_IR/771779441.102) [residual: <font color='red'>0.41863100293464</font>]<br />I16021002RDR.cub (Odyssey/THEMIS_IR/806757253.179) [residual: <font color='red'>0.46134858380318</font>]<br />Odyssey/THEMIS_IR/701643163.076 [residual: <font color='red'>0.43597701233739</font>]<br />I04576001RDR.cub (Odyssey/THEMIS_IR/725348214.153) [residual: <font color='red'>0.30914060868133</font>]<br />I09257002RDR.cub (Odyssey/THEMIS_IR/758640718.179) [residual: <font color='red'>0.46836807082174</font>]<br />I11416002RDR.cub (Odyssey/THEMIS_IR/773998971.153) [residual: <font color='red'>0.91150500491869</font>]<br />I12639002RDR.cub (Odyssey/THEMIS_IR/782699009.153) [residual: <font color='red'>1.1778289552804</font>]<br />I02691002RDR.cub (Odyssey/THEMIS_IR/711942122.000) [residual: <font color='red'>0.34370476346785</font>]<br />Odyssey/THEMIS_IR/824157276.000 [residual: <font color='red'>0.18355145849252</font>]<br />Odyssey/THEMIS_IR/751627267.051 [residual: <font color='red'>0.36767426891234</font>]<br />I11728002RDR.cub (Odyssey/THEMIS_IR/776218444.204) [residual: <font color='red'>0.45722231944607</font>]<br />I19091002RDR.cub (Odyssey/THEMIS_IR/828596080.230) [residual: <font color='red'>0.27598739181935</font>]<br />Odyssey/THEMIS_IR/817676676.076 [residual: <font color='red'>0.27221605600613</font>]<br />Odyssey/THEMIS_IR/830815439.153 [residual: <font color='red'>0.74485039589228</font>]<br />Odyssey/THEMIS_IR/746478164.204 [residual: <font color='red'>0.3135765348542</font>]</div>_QLabel").text, "<div>Point ID: THM_AEOLIS_auto_035066<br />Point Type: Free<br />Number of Measures: 17<br />Ignored: No<br />Edit Locked: No<br />I03415002RDR.cub (Odyssey/THEMIS_IR/717091315.204) [residual: <font color='red'>0.34334690243932</font>]<br />Odyssey/THEMIS_IR/760860152.204 [residual: <font color='red'>0.26169018459583</font>]<br />I11104002RDR.cub (Odyssey/THEMIS_IR/771779441.102) [residual: <font color='red'>0.41863100293464</font>]<br />I16021002RDR.cub (Odyssey/THEMIS_IR/806757253.179) [residual: <font color='red'>0.46134858380318</font>]<br />Odyssey/THEMIS_IR/701643163.076 [residual: <font color='red'>0.43597701233739</font>]<br />I04576001RDR.cub (Odyssey/THEMIS_IR/725348214.153) [residual: <font color='red'>0.30914060868133</font>]<br />I09257002RDR.cub (Odyssey/THEMIS_IR/758640718.179) [residual: <font color='red'>0.46836807082174</font>]<br />I11416002RDR.cub (Odyssey/THEMIS_IR/773998971.153) [residual: <font color='red'>0.91150500491869</font>]<br />I12639002RDR.cub (Odyssey/THEMIS_IR/782699009.153) [residual: <font color='red'>1.1778289552804</font>]<br />I02691002RDR.cub (Odyssey/THEMIS_IR/711942122.000) [residual: <font color='red'>0.34370476346785</font>]<br />Odyssey/THEMIS_IR/824157276.000 [residual: <font color='red'>0.18355145849252</font>]<br />Odyssey/THEMIS_IR/751627267.051 [residual: <font color='red'>0.36767426891234</font>]<br />I11728002RDR.cub (Odyssey/THEMIS_IR/776218444.204) [residual: <font color='red'>0.45722231944607</font>]<br />I19091002RDR.cub (Odyssey/THEMIS_IR/828596080.230) [residual: <font color='red'>0.27598739181935</font>]<br />Odyssey/THEMIS_IR/817676676.076 [residual: <font color='red'>0.27221605600613</font>]<br />Odyssey/THEMIS_IR/830815439.153 [residual: <font color='red'>0.74485039589228</font>]<br />Odyssey/THEMIS_IR/746478164.204 [residual: <font color='red'>0.3135765348542</font>]</div>")

    clickButton(waitForObject(":Control Point Information.OK_QPushButton"))

    clickButton(waitForObject(":qmos_QToolButton"))
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
             
