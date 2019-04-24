#define GUIHELPERS

#include "Isis.h"

#include <sstream>

#include "FileName.h"
#include "IString.h"
#include "Table.h"

using namespace Isis;
using namespace std;

int g_pos = 0;
QString g_previousFile = "";

void helperButtonGetTableList();

map <QString, void *> GuiHelpers() {
  map <QString, void *> helper;
  helper ["helperButtonGetTableList"] = (void *) helperButtonGetTableList;
  return helper;
}

void IsisMain() {
  // Gather parameters from the UserInterface
  UserInterface &ui = Application::GetUserInterface();
  FileName file = ui.GetFileName("FROM");
  QString tableName = ui.GetString("NAME");
  Table table(tableName, file.expanded());

  // Set the character to separate the entries
  QString delimit;
  if (ui.GetString("DELIMIT") == "COMMA") {
    delimit = ",";
  }
  else if (ui.GetString("DELIMIT") == "SPACE") {
    delimit = " ";
  }
  else {
    delimit = ui.GetString("CUSTOM");
  }

  // Open the file and output the column headings
  stringstream ss(stringstream::in | stringstream::out);

  for (int i = 0; i < table[0].Fields(); i++) {
    for (int j = 0; j < table[0][i].size(); j++) {
      QString title = table[0][i].name();
      if (table[0][i].isText()) {
        j += table[0][i].bytes();
      }
      else if (table[0][i].size() > 1) {
        title += "(" + toString(j) + ")";
      }
      if (i == table[0].Fields() - 1 && j == table[0][i].size() - 1) {
        // We've reached the last field, omit the delimiter
        ss << title;
      }
      else {
        ss << title + delimit;
      }
    }
  }

  // Loop through for each record
  for (int i = 0; i < table.Records(); i++) {
    ss.put('\n');

    // Loop through each Field in the record
    for (int j = 0; j < table[i].Fields(); j++) {
      // if there is only one entry in this field,
      // cast and output accordingly
      if (table[i][j].size() == 1) {
        if (table[i][j].isInteger()) {
          ss << IString((int)table[i][j]);
        }
        else if (table[i][j].isDouble()) {
          ss << IString((double)table[i][j]);
        }
        else if (table[i][j].isText()) {
          ss << (QString)table[i][j];
        }
        if (j < table[i].Fields() - 1) {
          ss << delimit;
        }
      }
      // Otherwise, build a vector to contain the entries,
      // and output them with the delimiter character between
      else {
        if (table[i][j].isText()) {
          ss << (QString)table[i][j] << delimit;
        }
        else if (table[i][j].isInteger()) {
          vector<int> currField = table[i][j];
          for (int k = 0; k < (int)currField.size(); k++) {
            // check to see that we aren't on either the last field, or
            // (if we are), we aren't on the last element of the field
            if (j < table[i].Fields() - 1 ||
                k < (int)currField.size() - 1) {
              ss << currField[k] << delimit;
            }
            else {
              ss << currField[k];
            }
          }
        }
        else if (table[i][j].isDouble()) {
          vector<double> currField = table[i][j];
          for (int k = 0; k < (int)currField.size(); k++) {
            // check to see that we aren't on either the last field, or
            // (if we are), we aren't on the last element of the field
            if (j < table[i].Fields() - 1 ||
                k < (int)currField.size() - 1) {
              ss << currField[k] << delimit;
            }
            else {
              ss << currField[k];
            }
          }
        }
      }
    } // End Field loop
  } // End Record loop
  ss.put('\n');


  if (ui.WasEntered("TO")) {
    QString outfile(FileName(ui.GetFileName("TO")).expanded());
    ofstream outFile(outfile.toLatin1().data());
    outFile << ss.str();
    outFile.close();
  }
  else if (ui.IsInteractive()) {
    QString log = ss.str().c_str();
    Application::GuiLog(log);
  }
  else {
    cout << ss.str();
  }
}

// Function to find the available table names and put them into the GUI
void helperButtonGetTableList() {
  QString list;
  bool match = false;

  UserInterface &ui = Application::GetUserInterface();
  QString currentFile = ui.GetFileName("FROM");
  const Pvl label(FileName(currentFile).expanded());

  // Check to see if the "FILE" parameter has changed since last press
  if (currentFile != g_previousFile) {
    ui.Clear("NAME");
    g_pos = 0;
    g_previousFile = currentFile;
  }

  // Look for tables
  int cnt = 0;
  while (!match) {
    // If we've gone through all objects and found nothing, throw an exception
    if (cnt >= label.objects()) {
      g_pos = 0;
      QString msg = "Parameter [FROM] has no tables.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    // When the end of the objects is hit, display "NAME" parameter as blank
    if (g_pos >= label.objects()) {
      list = "";
      match = true;
      g_pos = 0;  // Prepare to start over again
    }
    // When we find a table, fetch its name to stick in the "NAME" parameter
    else if (label.object(g_pos).name() == "Table") {
      list = label.object(g_pos)["Name"][0];
      match = true;
      g_pos++;
    }
    // If all else fails, keep looking for tables
    else {
      g_pos++;
      cnt++;
    }
  }

  ui.Clear("NAME");
  ui.PutString("NAME", list);
}
