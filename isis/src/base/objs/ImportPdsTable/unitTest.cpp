#include "Preference.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "ImportPdsTable.h"

//  Convenience list
typedef std::vector<std::string>  StrList;

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  string inputFile = "data/VIR_IR_1A_1_332974737_1_HK.LBL";
  if (--argc == 1) { inputFile = argv[1]; }

  cout << "\n\nTesting ImportPdsTable class using file " << inputFile << "\n";

  ImportPdsTable myTable(inputFile);
  cout << "\n\nList of Columns found - Total: " << myTable.columns() << "\n";
  StrList kfiles = myTable.getColumnNames();
  copy(kfiles.begin(), kfiles.end(), ostream_iterator<std::string>(cout, "\n"));

  cout << "\n\nNow without Name Conversion: \n";
  kfiles = myTable.getColumnNames(false);
  copy(kfiles.begin(), kfiles.end(), ostream_iterator<std::string>(cout, "\n"));

  // Update/correct column types
  myTable.setType("ShutterStatus", "CHARACTER");
  myTable.setType("ChannelId", "CHARACTER");
  myTable.setType("CompressionMode", "CHARACTER");
  myTable.setType("SpectralRange", "CHARACTER");
  myTable.setType("CurrentMode", "CHARACTER");
  myTable.setType("SubCurrentMode", "CHARACTER");
  myTable.setType("IrExpo", "DOUBLE");
  myTable.setType("IrTemp", "DOUBLE");
  myTable.setType("CcdExpo", "DOUBLE");
  myTable.setType("CcdTemp", "DOUBLE");
  myTable.setType("MirrorSin", "DOUBLE");
  myTable.setType("Mirror", "DOUBLE");
  myTable.setType("SpectTemp", "DOUBLE");
  myTable.setType("TeleTemp", "DOUBLE");
  myTable.setType("COLD TIP TEMP", "DOUBLE");
  myTable.setType("RADIATOR TEMP", "DOUBLE");
  myTable.setType("SU MOTOR CURR", "DOUBLE");
  myTable.setType("LEDGE TEMP", "DOUBLE");
  myTable.setType("FRAME COUNT", "CHARACTER");

  // Create a table from the input
  cout << "Getting ISIS Table...\n";
  Table newTable = myTable.exportAsTable("VIR_DATA");

//  newTable.Write("vir_data.tbl");
  return (0);
}
