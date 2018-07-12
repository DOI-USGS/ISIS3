#include "Isis.h"

#include <QList>
#include <QPair>
#include <QString>

#include "CSVReader.h"
#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "ProgramLauncher.h"
#include "Pvl.h"
#include "PvlContainer.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "Table.h"
#include "TableField.h"
#include "TableRecord.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;


PvlKeyword makeKeyword(QString columnName, CSVReader::CSVAxis row, CSVReader reader);
PvlKeyword makeMultipleColumnKeyword(QString baseColumnName, QString keywordName, int max, 
                                     CSVReader::CSVAxis row, CSVReader reader);

/**
 * This program imports Apollo 15 Pan images.
 */
void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  
  Cube *icube = NULL;
  icube = new Cube();

  Pvl *label = new Pvl(ui.GetFileName("FROM"));
  
  PvlObject *pvl = label;

  if (ui.WasEntered("TOPVL")) {
    icube->open(ui.GetFileName("FROM"), "r");
  }
  else {
    icube->open(ui.GetFileName("FROM"), "rw");
    pvl = &(icube->label()->findObject("IsisCube"));
  }
  
  //QString detachedLabelFile = FileName(ui.GetFileName("TOPVL")).name();
  
  const QString fileName = "/archive/missions/apollo_pan/AS15/CriticalData/PanData/MetadataCSVVersions/Apollo15PanMetadata.csv";
  CSVReader reader(fileName, true, 0, ',', true, true);

  //Find the correct row for the image, this needs to be parsed from the input file name
  FileName from(ui.GetFileName("FROM"));
  QString cubeName = from.baseName();
  QStringList cubeNameList = cubeName.split("_");
  
  QString imageHeader = "Image#";
  QString imageValue = cubeNameList[0];
  QPair<QString,QString> imageSearchPair(imageHeader, imageValue);
  
  QString tileHeader = "Tile#";
  QString tileValue = cubeNameList[1].right(1);
  QPair<QString,QString> tileSearchPair(tileHeader, tileValue);
  
  
  //TODO give the users the option to search to see if the image/tile found will match with 
  //another column and value as well
  QList< QPair<QString,QString> > searchList;
  searchList.append(imageSearchPair);
  searchList.append(tileSearchPair);
  
  CSVReader::CSVAxis row = reader.getRow(searchList);
 
  
  //Delete any existing groups in the label
  if (pvl->hasGroup("Instrument")) {
    pvl->deleteGroup("Instrument");
  }
  if (pvl->hasGroup("Kernels")) {
    pvl->deleteGroup("Kernels");
  }
  if (pvl->hasGroup("Fiducials")) {
    pvl->deleteGroup("Fiducials");
  }
  if (pvl->hasGroup("TimingMarks")) {
    pvl->deleteGroup("TimingMarks");
  }
  
  //Create all new group labels
  pvl->addGroup(QString("Instrument"));
  pvl->addGroup(QString("Kernels"));
  pvl->addGroup(QString("Fiducials"));
  pvl->addGroup(QString("TimingMarks"));
  
  
  //Grab all of the data for the instrument group and write it to the cube
  PvlGroup &instrumentGrp = pvl->findGroup("Instrument", PvlObject::Traverse);
  
  instrumentGrp.addKeyword(makeKeyword("SpacecraftName", row, reader));
  instrumentGrp.addKeyword(makeKeyword("InstrumentID", row, reader));
  instrumentGrp.addKeyword(makeKeyword("TargetName", row, reader));
  instrumentGrp.addKeyword(makeKeyword("StartTime", row, reader));
  instrumentGrp.addKeyword(makeKeyword("StopTime", row, reader));
  instrumentGrp.addKeyword(makeKeyword("SpacecraftClockStartCount", row, reader));
  instrumentGrp.addKeyword(makeKeyword("SpacecraftClockStopCount", row, reader));
  instrumentGrp.addKeyword(makeKeyword("Tile#", row, reader));
  
  PvlGroup &kernelsGrp = pvl->findGroup("Kernels", PvlObject::Traverse);
  kernelsGrp.addKeyword(makeKeyword("NAIFIkCode", row, reader));
  
  PvlGroup &fiducialsGrp = pvl->findGroup("Fiducials", PvlObject::Traverse);

  fiducialsGrp.addKeyword(makeMultipleColumnKeyword("FiducialNum", "Number", 20, row, reader));
  fiducialsGrp.addKeyword(makeMultipleColumnKeyword("FiducialLine", "Line", 20, row, reader));
  fiducialsGrp.addKeyword(makeMultipleColumnKeyword("FiducialSamp", "Sample", 20, row, reader));
  fiducialsGrp.addKeyword(makeMultipleColumnKeyword("FiducialXCoord", "XCoordinates", 20, row, reader));
  fiducialsGrp.addKeyword(makeMultipleColumnKeyword("FiducialYCoord", "YCoordinates", 20, row, reader));
  fiducialsGrp.addKeyword(makeMultipleColumnKeyword("FiducialValid", "Valid", 20, row, reader));
  
  PvlGroup &timingMarksGrp = pvl->findGroup("TimingMarks", PvlObject::Traverse);
  timingMarksGrp.addKeyword(makeKeyword("TimingOffset", row, reader));
  timingMarksGrp.addKeyword(makeMultipleColumnKeyword("TimingLine", "Line", 70, row, reader));
  timingMarksGrp.addKeyword(makeMultipleColumnKeyword("TimingSamp", "Sample", 70, row, reader));
  timingMarksGrp.addKeyword(makeMultipleColumnKeyword("TimingLength", "Length", 70, row, reader));
  timingMarksGrp.addKeyword(makeMultipleColumnKeyword("TimingValid", "Valid", 70, row, reader));
  
  //For these keywords the actual name of the keyword doesn't matter because all we want is the 
  //value of the keyword
  PvlKeyword ephemerisTimeKeyword = makeMultipleColumnKeyword("EphemerisTime", "EphemerisTime", 70, 
                                                              row, reader);
  PvlKeyword exposureTimeKeyword = makeMultipleColumnKeyword("ExposureTime", "ExposureTime", 70, 
                                                              row, reader);
  PvlKeyword exposureSampleKeyword = makeMultipleColumnKeyword("ExposureSample", "ExposureSample", 
                                                              70, row, reader);
  
  
  //If there is no detached label specified, create the table and write it to the input cube
  if (!ui.WasEntered("TOPVL")) {
    QString eph = ephemerisTimeKeyword[0];
    QString exp = exposureTimeKeyword[0];
    QString samp = exposureSampleKeyword[0];
  
    QStringList ephList = eph.split(",");
    QStringList expList = exp.split(",");
    QStringList sampList = samp.split(",");
    
    ephList.replaceInStrings("(", " ");
    expList.replaceInStrings("(", " ");
    sampList.replaceInStrings("(", " ");
    
    ephList.replaceInStrings(")", " ");
    expList.replaceInStrings(")", " ");
    sampList.replaceInStrings(")", " ");
    
    TableField ephTimeField("EphemerisTime", TableField::Double);
    TableField expTimeField("ExposureTime", TableField::Double);
    TableField sampStartField("SampleStart", TableField::Integer);

    TableRecord timesRecord;
    timesRecord += ephTimeField;
    timesRecord += expTimeField;
    timesRecord += sampStartField;

    Table timesTable("SampleScanTimes", timesRecord);

    std::vector<double> ephemerisTimes;
    std::vector<double> exposureTimes;
    std::vector<int> sampleStartTimes;
    
    for (int i = 0; i < ephList.size(); i++) {
      ephemerisTimes.push_back(ephList[i].toDouble());
      exposureTimes.push_back(expList[i].toDouble());
      sampleStartTimes.push_back(sampList[i].toInt());
    }
    
    for(unsigned int i = 0; i < ephemerisTimes.size(); i++) {
      timesRecord[0] = ephemerisTimes[i];
      timesRecord[1] = exposureTimes[i];
      timesRecord[2] = sampleStartTimes[i];
      timesTable += timesRecord;
    }
    
    icube->deleteBlob("Table", "SampleScanTimes");
    icube->write(timesTable);
  }
  else {
    label->write(ui.GetFileName("TOPVL"));
  }
  
  icube->close();
  delete label;
  label = NULL;
}

/**
 * This finds the column given the column name, creates a keyword from the value of that
 * column, and returns the keyword.
 * 
 * @param columnName QString of the column name
 * @param row CSVReader::CSVAxis that we are searching
 * @param reader The reader we are using
 * 
 * @return @b PvlKeyword The keyword made from the value of the column
 */
PvlKeyword makeKeyword(QString columnName, CSVReader::CSVAxis row, CSVReader reader) {
  int columnNumber = reader.getHeaderColumn(columnName);
  QString columnValue = QString(row[columnNumber]).trimmed();
  PvlKeyword columnKeyword(columnName, columnValue);
  return columnKeyword;
}

/**
 * This loops through multiple columns using a column base name (ie the column base name is 
 * FiducialNum and the columns are actually called FiducialNum1, FiducialNum2, etc) creates a 
 * QString of the values of the columns and then creates a keyword using those values and the 
 * actual name of the keyword that was specified.
 * 
 * The value of the keywords will be in parenthesis.
 * 
 * @param baseColumnName QString of the base column name
 * @param keywordName the name of the keyword
 * @param max the maximum number of columns that use the base column name
 * @param row CSVReader::CSVAxis that we are searching
 * @param reader The reader we are using
 * 
 * @return @b PvlKeyword The keyword made from the value of the column
 */
PvlKeyword makeMultipleColumnKeyword(QString baseColumnName, QString keywordName, int max, 
                                     CSVReader::CSVAxis row, CSVReader reader) {
  
  if(max < 1) {
    QString mess = "Max cannot be less than 1";
    throw IException(IException::Programmer, mess, _FILEINFO_);
  }
    
  QString keywordValue = "(";
  for (int i = 1; i <= max; i++) {
    QString headerName = QString(baseColumnName + "%1").arg(i);
    int columnNum = reader.getHeaderColumn(headerName);
    QString columnValue = QString(row[columnNum]).trimmed();
    if (columnValue == "") {
      keywordValue = keywordValue + ")";
      PvlKeyword columnKeyword(keywordName, keywordValue);
      return columnKeyword;
    }
    if (i == 1) { 
      keywordValue = keywordValue + columnValue;
    }
    else {
      keywordValue = keywordValue + ", " + columnValue;
    }
  }
  PvlKeyword emptyKeyword;
  return emptyKeyword;
}