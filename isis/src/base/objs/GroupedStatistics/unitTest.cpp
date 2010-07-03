#include <iostream>

#include <QVector>
#include <QString>

#include "GroupedStatistics.h"
#include "Statistics.h"
#include "Preference.h"


using namespace std;
using namespace Isis;


int main ()
{
  Isis::Preference::Preferences(true);
  cerr << "GroupedStatistics unitTest!!!\n\n";

  // test constructor
  cerr << "testing constructor...\n\n";
  GroupedStatistics * groupedStats = new GroupedStatistics();
  
  // test AddStatistic
  cerr << "testing AddStatistic...\n\n";
  groupedStats->AddStatistic("Height", 71.5);
  
  // test copy constructor
  cerr << "testing copy constructor...\n\n";
  GroupedStatistics * groupedStats2 = new GroupedStatistics(*groupedStats);
  
  // test GetStatistics
  cerr << "testing GetStatistics...\n";
  Statistics stats = groupedStats2->GetStatistics("Height");
  cerr << "    " << stats.Average() << "\n\n";
  
  // test GetStatisticTypes
  cerr << "testing GetStatisticTypes...\n";
  QVector< QString > statTypes = groupedStats->GetStatisticTypes();
  for (int i = 0; i < statTypes.size(); i++)
    cerr << "    " << statTypes[i].toStdString() << "\n";
  cerr << "\n";

  // test destructor
  delete groupedStats;
  delete groupedStats2;
  
  return 0;
}
