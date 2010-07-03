#include <string>
#include <vector>
#include "Database.h"
#include "SqlQuery.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

void print(vector<string> in);

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  // SQLite database is in memory
  std::string dbname(":memory:");

  Database testdb("unittestdb", "SQLite");
  testdb.setDatabaseName(dbname.c_str());
  if(!testdb.open()) {
    iException::Message(iException::User, "Connection failed", _FILEINFO_);
  }

  //  Construct queries
  string table = "CREATE TABLE testTable ("
                 " v1 TEXT,"
                 " v2 INTEGER,"
                 " v3 REAL );";
  string insert = "INSERT INTO testTable (v1, v2, v3) "
                  " VALUES ('test text', 7, 123.4);";
  string query = "SELECT * FROM testTable;";
  
  SqlQuery create(testdb);
  create.setThrowOnFailure();

  try {
    create.exec(table);
    create.exec(insert);
    create.exec(query);
    vector<string> fields = create.fieldNameList();
    for(int i = 0; i < (int)fields.size(); i++) {
      cout << "Field " << i << ": " << fields[i] << endl;
    }
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << "Database Name: " << testdb.Name() << endl;
  cout << "Persistant: " << testdb.isPersistant() << endl;
  cout << "Calling makePersistant()..." << endl;
  testdb.makePersistant();
  cout << "Persistant: " << testdb.isPersistant() << endl;
  
  Database c = testdb.clone("unittestdb clone"); 
  cout << "Clone name: " << c.Name() << endl;

  cout << "Tables: ";
  print(testdb.getTables());

  cout << "Views: ";
  print(testdb.getViews());

  cout << "SystemTables: ";
  print(testdb.getSystemTables());

  cout << "Removing clone... ";
  Database::remove("c");
  cout << "Done." << endl;

  // Closing and removing persistant DB
  testdb.close();
  Database::remove(dbname);
  return 0;
}

// Print contents of a vector<string> to console
void print(vector<string> in) {
  cout << "[";
  for(int i = 0; i < (int)in.size(); i++) {
    cout << " " << in[i] << " ";
  }
  cout << "]" << endl;
}
