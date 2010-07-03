#include "DbProfile.h"
#include "Database.h"
#include "DatabaseFactory.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  DatabaseFactory *df = DatabaseFactory::getInstance();
  DbProfile p0("default profile");
  DbProfile p1("test profile 1");
  DbProfile p2("test profile 2");
  df->setDefault("default profile");
  cout << "Default name: " << df->getDefault() << endl;
  cout << "Add access profile: " << df->addAccessProfile("profile") << endl; 
  cout << "Adding a couple profiles..." << endl;
  df->addProfile(p1);
  df->addProfile(p2);
  vector<string> strings = df->getProfileList();
/*  Removed to keep from having to change truth data every install.
 *  Added specific checks below for databases we care about.
  for(int i = 0; i < (int)strings.size(); i++) {
    cout << "Profile list: " << strings[i] << endl;
  }*/
  
  DbProfile dup = df->getProfile("test profile 1");

  cout << "Default profile name: " << df->getDefaultProfileName() << endl;
  cout << "Setting a default name... ";
  cout << df->setDefaultProfileName("default name") << endl;
  cout << "Default profile name: " << df->getDefaultProfileName() << endl;
  
  vector<string> available = df->available();
  cout << "Driver available [doesntexist]: " << df->isDriverAvailable("doesntexist") << endl;
  cout << "Driver available [mysql]: " << df->isDriverAvailable("mysql") << endl;
  cout << "Driver available [postgresql]: " << df->isDriverAvailable("postgresql") << endl;
  cout << "Driver available [sqlite]: " << df->isDriverAvailable("sqlite") << endl;
  cout << "Is Available: " << df->isAvailable("doesntexist") << endl;
  cout << "Is connected: " << df->isConnected("doesntexist") << endl;
  cout << "Is persistant: " << df->isPersistant("doesntexist") << endl;
  
  try { 
    cout << "Attempting to create a database connection" << endl;
    df->create("doesntexist");
  }
  catch(iException &e) {
    e.Report(false);
  }

  Database d;
  cout << "Adding a database... ";
  df->add(d, "test db", true);
  cout << "Done." << endl;
  
  try { 
    cout << "Attempting to create a database connection... ";
    df->create("test db");
    cout << "Done." << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }
  
  cout << "Is Available: " << df->isAvailable("test db") << endl;
  cout << "Is connected: " << df->isConnected("test db") << endl;
  cout << "Is persistant: " << df->isPersistant("test db") << endl;

  cout << "Removing database... ";
  df->remove("test db");
  cout << "Done." << endl;
  cout << "Destroying database... ";
  df->destroy("test db");
  cout << "Done." << endl;
  

  return 0;
}
