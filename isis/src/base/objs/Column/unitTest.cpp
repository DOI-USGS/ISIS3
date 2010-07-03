#include "Column.h"

#include <iostream>

#include "Preference.h"



using namespace std;
using namespace Isis;

int main()
{
  cerr << "\nUnit Test for Column!!!\n\n";
  Preference::Preferences(true);

  Column * testColumn = new Column("test column", 15, Column::Integer);

  cerr << "Name() returns: " << testColumn->Name() << "\n\n";
  cerr << "Width() returns: " << testColumn->Width() << "\n\n";
  cerr << "DataType() returns: " << testColumn->DataType() << "\n\n";
  cerr << "Alignment() returns: " << testColumn->Alignment() << "\n\n";
  cerr << "Precision() returns: " << testColumn->Precision() << "\n\n";

  return 0;
}
