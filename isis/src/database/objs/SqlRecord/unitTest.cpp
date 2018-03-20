#include "Database.h"

#include <QFile>
#include <QSqlDatabase>
#include <QDebug>

#include "FileName.h"
#include "SqlQuery.h"
#include "SqlRecord.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  // SQLite
  FileName dbfile("$TEMPORARY/test.db");
  Database testdb("testdb", "SQLite");
  QString dbfileName(dbfile.expanded());
  testdb.setDatabaseName(dbfileName.toLatin1().data());
  if(!testdb.open()) {
    throw IException(IException::User, "Connection failed", _FILEINFO_);
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
      cout << "v1: " << IString(r.getValue(0)) << endl;
      cout << "v2: " << IString(r.getValue(1)) << endl;
      cout << "v3: " << IString(r.getValue(2)) << endl;
      cout << "Is null (v1): " << r.isNull("v1") << endl;
      cout << "Is null (blank): " << r.isNull("") << endl;
    }
  }
  catch(IException &e) {
    e.print();
  }


  SqlRecord record(create);
  cout << "Size: " << record.size() << endl;
  cout << "Has field(v2): " << record.hasField("v2") << endl;
  cout << "Field index(v3): " << record.getFieldIndex("v3") << endl;

  cout << "Older sqlite versions report the double fields as string fields. " <<
       "Until the problematic 3rd party software is fixed, " <<
       "systems that report double will need OS truth data." << endl;

  for(int i = 0; i < record.size(); i++) {
    IString value = record.getValue(i);
    cout << "Col " << i << ") " << "Name: " << record.getFieldName(i) <<
         ", Type: " << record.getType(i) << endl;

  }

  QFile::remove(FileName("$TEMPORARY/test.db").expanded());
  return 0;
}
