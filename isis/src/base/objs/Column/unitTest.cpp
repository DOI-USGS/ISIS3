#include "Column.h"

#include <iostream>

#include "IException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

void printColumn(const Column &column);

int main() {
  cerr << "\nUnit Test for Column!!!\n\n";
  Preference::Preferences(true);
  
  try {
    Column testColumn;
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetWidth(1);
    testColumn.SetName("test column");
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetName("test column");
    testColumn.SetWidth(100);
    testColumn.SetPrecision(100);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetName("test column");
    testColumn.SetWidth(1);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetType(Column::Pixel);
    testColumn.SetPrecision(100);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetType(Column::Real);
    testColumn.SetAlignment(Column::Decimal);
    testColumn.SetPrecision(100);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetType(Column::Integer);
    testColumn.SetAlignment(Column::Decimal);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetType(Column::String);
    testColumn.SetAlignment(Column::Decimal);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetAlignment(Column::Decimal);
    testColumn.SetType(Column::Real);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetAlignment(Column::Decimal);
    testColumn.SetType(Column::Integer);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn;
    testColumn.SetAlignment(Column::Decimal);
    testColumn.SetType(Column::String);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }

  try {
    Column testColumn("test column", 15, Column::Integer);
    printColumn(testColumn);
  }
  catch (IException &e) {
    e.print();
  }


  return 0;
}

void printColumn(const Column &column) {
  cerr << "Column \"" << column.Name() << "\"\n";
  try {
    cerr << "\tWidth() = " << column.Width() << "\n";
    cerr << "\tDataType() = " << column.DataType() << "\n";
    cerr << "\tAlignment() = " << column.Alignment() << "\n";
    cerr << "\tPrecision() = " << column.Precision() << "\n\n";
  }
  catch (IException &e) {
    cerr << "\t" << e.toString() << "\n\n";
  }
}
