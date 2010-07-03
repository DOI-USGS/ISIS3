#include "Database.h"
#include "Filename.h"
#include "SqlQuery.h"
#include "SqlRecord.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  // SQLite
  Filename dbfile("/tmp/test.db");
  Database testdb("testdb", "SQLite");
  string dbfileName(dbfile.Expanded());
  testdb.setDatabaseName(dbfileName.c_str());
  if(!testdb.open()) {
    iException::Message(iException::User, "Connection failed", _FILEINFO_);
  }
  
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

    while(create.next()) {
      SqlRecord r = create.getRecord();
      cout << "v1: " << iString(r.getValue(0)) << endl;
      cout << "v2: " << iString(r.getValue(1)) << endl;
      cout << "v3: " << iString(r.getValue(2)) << endl;
      cout << "Is null (v1): " << r.isNull("v1") << endl;
      cout << "Is null (blank): " << r.isNull("") << endl;
    }
  }
  catch(iException &e) {
    e.Report(false);
  }
 
  
  SqlRecord record(create);
  cout << "Size: " << record.size() << endl;
  cout << "Has field(v2): " << record.hasField("v2") << endl;
  cout << "Field index(v3): " << record.getFieldIndex("v3") << endl;

  cout << "The mac systems report the double fields as string fields. " <<
          "Until the problematic 3rd party software is fixed, " <<
          "the mac systems will need OS truth data." << endl;

  for(int i = 0; i < record.size(); i++) {
    iString value = record.getValue(i);
    cout << "Col " << i << ") " << "Name: " << record.getFieldName(i) <<
            ", Type: " << record.getType(i) << endl;
            
  }
 
  remove("/tmp/test.db");
  return 0;
}
