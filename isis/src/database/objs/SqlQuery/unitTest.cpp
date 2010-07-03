#include "SqlQuery.h"
#include "SqlRecord.h"
#include "Filename.h"
#include "Database.h"
#include "iException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  Database testdb("testdb", "SQLite");
  testdb.setDatabaseName(":memory:");
  string table = "CREATE TABLE testTable ("
                 " v1 TEXT,"
                 " v2 INTEGER,"
                 " v3 REAL );";
  string  insert = "INSERT INTO testTable (v1, v2, v3) "
                    " VALUES ('test txt', 7, 123.4);";
  string  query = "SELECT * FROM testTable;";

  if(!testdb.open()) {
    iException::Message(iException::User, "Connection failed", _FILEINFO_);
  }

  SqlQuery q(testdb);
  q.setThrowOnFailure();
  cout << "Is throwing: " << q.isThrowing() << endl;

  try {
    cout << "Executing CREATE TABLE command: " << q.exec(table) << endl;
    cout << "Query: " << q.getQuery() << endl;
    cout << "Executing INSERT command: " << q.exec(insert) << endl;
    cout << "Query: " << q.getQuery() << endl;
    cout << "Executing SELECT command: " << q.exec(query) << endl;
    cout << "Query: " << q.getQuery() << endl;

  }  
  catch(iException &e) {
    e.Report(false);
  }

  cout << "The mac systems report the double fields as string fields. " <<
          "Until the problematic 3rd party software is fixed, " <<
          "the mac systems will need OS truth data." << endl;

  vector<string> nameList = q.fieldNameList();
  vector<string> typeList = q.fieldTypeList();
  for(int i = 0; i < (int)nameList.size(); i++) {
    cout << i << ") Name: " << nameList[i] << ", Type: " << typeList[i] << endl;
  }

  cout << "Field index (v2): " << q.fieldIndex("v2") << endl;
  cout << "nFields: " << q.nFields() << endl;
  cout << "nRows: " << q.nRows() << endl;
  cout << "Field name[0]: " << q.fieldName(0) << endl;
  SqlRecord record = q.getRecord();
  cout << "SqlRecord size: " << record.size() << endl;

  return 0;
}
