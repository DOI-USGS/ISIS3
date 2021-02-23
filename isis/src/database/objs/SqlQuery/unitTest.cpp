/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "SqlQuery.h"
#include "SqlRecord.h"
#include "FileName.h"
#include "Database.h"
#include "IException.h"
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
    throw IException(IException::User, "Connection failed", _FILEINFO_);
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
  catch(IException &e) {
    e.print();
  }

  cout << "Some versoin of Sqlite treat reals as strings. " <<
       "Until the sqlite honors reals the main truth file will " <<
       "appear incorrect. Systems with sqlite version that correctly" <<
       "report real fields as double will need os specific truth data" << endl;

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
