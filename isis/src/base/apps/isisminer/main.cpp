#include "Isis.h"

// std library
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Qt library
#include <QList>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QTime>

// boost library
#include <boost/foreach.hpp>

// ISIS
#include "Application.h"
#include "Database.h"
#include "FileName.h"
#include "GisGeometry.h"
#include "IString.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlFlatMap.h"
#include "Resource.h"
#include "SqlQuery.h"
#include "SqlRecord.h"
#include "Strategy.h"
#include "StrategyFactory.h"

using namespace std; 
using namespace Isis;

void IsisMain() {

  //  Program constants
  const QString isisminer_program = "isisminer";
  const QString isisminer_version = "1.0";
  const QString isisminer_revision = "$Revision: 6513 $";
  const QString isisminer_runtime = Application::DateTime();

  UserInterface &ui = Application::GetUserInterface();
  StrategyFactory *factory = StrategyFactory::instance();

  SharedResource globals(new Resource("Globals"));
  globals->add("Program", isisminer_program);
  globals->add("Version", isisminer_version);
  globals->add("Revision", isisminer_revision);
  globals->add("RunTime", isisminer_runtime);

  
  // File containing isisminer configuration run
  QString config = ui.GetFileName("CONFIG");
  globals->add("CONFIG", config);

  // Add parameters provided by user to global resources
  if ( ui.WasEntered("PARAMETERS") ) {
    QString parameters = ui.GetString("PARAMETERS");
    globals->add("PARAMETERS", parameters);
    
    // Split by separate parameters
    QStringList parmlist = parameters.split("@", QString::SkipEmptyParts);
    BOOST_FOREACH (QString parm, parmlist) {
      // Split values from keyword name
      QStringList keyval = parm.split(":", QString::SkipEmptyParts);
      if ( keyval.size() != 2 ) {
        QString mess = "Ill-formed PARAMETERS (" + parm + ") - use form @key:val";
        throw IException(IException::User, mess, _FILEINFO_);
      }

      // Now split multi string values and construct the Pvl keyword
      QString keyname = keyval[0];
      QStringList values =  keyval[1].split(",", QString::SkipEmptyParts);
      PvlKeyword keyword(keyname.toStdString());
      BOOST_FOREACH ( QString val, values) {
        keyword.addValue(val.toStdString());
      }

      // Add the parameter to global parameters
      globals->add(keyword);
    }
  }

  // Add to factory
  factory->addGlobal(globals);

  // Load global parameter file for use in global variable pool
  if ( ui.WasEntered("GLOBALS") ) {
    QString globalfile = ui.GetFileName("GLOBALS");
    Pvl pvl_globals(globalfile.toStdString());
    SharedResource gfile(new Resource("GlobalFileResources", PvlFlatMap(pvl_globals)));
    factory->addGlobal(gfile);
    globals->add("GLOBALS", globalfile);
  }

  // Create strategies (computations, constraints, ranks, sorts, etc...)
  cout << "\nCreating strategies...\n";
  StrategyList strategies = factory->buildRun(config);
  cout << "Finished creating " << factory->manufactured() << " strategies...\n";

  // Input resource list preserved for subsequent processing
  ResourceList resources;
  QTime runTime = QTime::currentTime();
  BOOST_FOREACH ( SharedStrategy strategy, strategies ) {
    QTime stime = QTime::currentTime();
    cout << "\nRunning " << strategy->type() << "::" << strategy->name() 
         << " (TimeIn:  " << stime.toString("hh:mm:ss.zzz") 
         << ")\n"
         << "Description: " << strategy->description() << "\n";
    stime.start();
    int n = strategy->apply(resources);
    unsigned int ntotal = strategy->totalProcessed();
    cout << n << " of " << ntotal << " processed in " 
         << strategy->type() << "::" << strategy->name() 
         << " (TimeOut: " << QTime::currentTime().toString("hh:mm:ss.zzz") << ")\n";
    cout << "ElapsedTime(s): " << stime.elapsed() / 1000  << "\n";
  }

  // Get total elapded time
  QTime totalT(0,0);
  totalT = totalT.addMSecs(runTime.msecsTo(QTime::currentTime()));
  cout << "\nSession complete in " << totalT.toString("hh:mm:ss.zzz") 
       << " of elapsed time\n";

  return;
}

