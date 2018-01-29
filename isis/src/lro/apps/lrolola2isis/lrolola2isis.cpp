#include "Isis.h"

//ISIS libraries
#include "FileName.h"
#include "IException.h"
#include "UserInterface.h"

//STD libraries


//QT libraries
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextStream>


using namespace std;
using namespace Isis;


void fetchCSVData(FileName &csvFile );


void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  FileName dataFile = ui.GetFileName("FROM");
  FileName cubeList = ui.GetFileName("CUBES");
  FileName output = ui.GetFileName("TO");


  return;
}


void fetchCSVData(FileName &csvFile) {

  QFile data(csvFile.expanded());

  if (data.open(QIODevice::ReadOnly)) {
    qDebug() << "Could not open:  " << csvFile.expanded();
  }


  return;
}


