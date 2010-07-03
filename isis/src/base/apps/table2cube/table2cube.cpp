#include "Isis.h"
#include "Table.h"
#include "ProcessByLine.h"
#include "iString.h"
#include <Table.h>
#include <cmath>

using namespace std; 
using namespace Isis;

Table *table;
iString field;
int startRecord;
int numRecords;
int startElement;
int numElements;

void readTable(Isis::Buffer &outcube);

void IsisMain() {
  ProcessByBrick p;
  UserInterface &ui = Application::GetUserInterface();

  // Get Parameters
  table = new Table(ui.GetString("TABLENAME"));
  table->Read(ui.GetFilename("FROM"));

  field = ui.GetString("FIELD");
  startRecord = ui.GetInteger("STARTREC");
  startElement = ui.GetInteger("STARTELEM");

  if(ui.WasEntered("NUMREC")) {
    numRecords = ui.GetInteger("NUMREC");
  }
  else {
    numRecords = table->Records() - (startRecord - 1);
  }

  if(ui.WasEntered("NUMELEM")) {
    numElements = ui.GetInteger("NUMELEM");
  }
  else {
    numElements = (*table)[startRecord][field].Size() - (startElement - 1);
  }

  p.SetOutputCube("TO", numElements, numRecords);
  p.SetBrickSize(numElements, numRecords, 1);
  p.StartProcess(readTable);
  p.EndProcess();

  delete table;
}

void readTable(Isis::Buffer &outcube) {
  for(int record = startRecord; record < startRecord + numRecords; record ++) {
    for(int element = startElement; element < startElement + numElements; element ++) {
      int index = (numElements * (record-1)) + (element - startElement);

      if((*table)[record-1][field].IsReal()){
        std::vector<float> data = (*table)[record-1][field];
        outcube[index] = data.at(element-1);
      }
      else if((*table)[record-1][field].IsInteger()){
        std::vector<int> data = (*table)[record-1][field];
        outcube[index] = data.at(element-1);
      }
      else if((*table)[record-1][field].IsDouble()){
        std::vector<double> data = (*table)[record-1][field];
        outcube[index] = data.at(element-1);
      }
    }
  }
}
