#include "Isis.h"

#include <QFile>
#include <QVector>

#include "Buffer.h"
#include "Chip.h"
#include "Cube.h"
#include "FileList.h"
#include "Filename.h"
#include "OverlapStatistics.h"
#include "ProcessByLine.h"
#include "UserInterface.h"
#include "iException.h"
#include "iString.h"


using namespace Isis;
using std::string;


void blend(Buffer &in);
bool inIntersection(int sample, int line);
bool validValue(int oSample, int oLine);
Chip * createRamp(Chip *pic1, Chip *pic2, int stop);

void readOutputs(string outName, const FileList &inputs, FileList &outputs);
void generateOutputs(const FileList &inputs, FileList &outputs);


Chip *blendRamp;
Chip *i1;
Chip *i2;
int s1;
int s2;
int l1;
int l2;
int line;


void IsisMain() {
  blendRamp = NULL;
  i1 = NULL;
  i2 = NULL;

  // Get the user interface
  UserInterface &ui = Application::GetUserInterface();

  FileList inputs(ui.GetFilename("FROMLIST"));
  if (inputs.size() < 2) {
    string msg = "FROMLIST must have at least two images to blend";
    throw iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }

  FileList outputs;
  ui.WasEntered("TOLIST") ?
      readOutputs(ui.GetFilename("TOLIST"), inputs, outputs) :
      generateOutputs(inputs, outputs);

  for (unsigned int i = 0; i < inputs.size(); i++) {
    QString input(Filename(inputs[i]).Expanded());
    QString output(Filename(outputs[i]).Expanded());

    QFile::remove(output);
    if (!QFile::copy(input, output)) {
      string msg = "Cannot create output cube [" + output.toStdString() + "]";
      throw iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
  }

  for (unsigned int i = 0; i < outputs.size() - 1; i++) {
    Cube from1;
    from1.open(outputs[i]);

    bool hasOverlap = false;
    for (unsigned int j = i + 1; j < outputs.size(); j++) {
      Cube from2;
      from2.open(outputs[j]);

      OverlapStatistics oStats(from1, from2);

      if (oStats.HasOverlap()) {
        hasOverlap = true;

        i1 = new Chip(oStats.Samples() + 2, oStats.Lines() + 2);
        int from1CenterSample =
            (oStats.StartSampleX() + oStats.EndSampleX()) / 2;
        int from1CenterLine =
            (oStats.StartLineX() + oStats.EndLineX()) / 2;
        i1->TackCube(from1CenterSample, from1CenterLine);
        i1->Load(from1);

        i2 = new Chip(oStats.Samples() + 2, oStats.Lines() + 2);
        int from2CenterSample =
            (oStats.StartSampleY() + oStats.EndSampleY()) / 2;
        int from2CenterLine =
            (oStats.StartLineY() + oStats.EndLineY()) / 2;
        i2->TackCube(from2CenterSample, from2CenterLine);
        i2->Load(from2);

        // compute the linear regression fit of the mvstats data
        int stop = ui.GetInteger("STOP");
        blendRamp = createRamp(i1, i2, stop);

        // Apply the correction
        s1 = oStats.StartSampleX() - 1;
        s2 = oStats.EndSampleX() + 1;
        l1 = oStats.StartLineX() - 1;
        l2 = oStats.EndLineX() + 1;

        line = 1;

        // We will be processing by line
        ProcessByLine p;
        CubeAttributeInput att;

        iString cubeName = outputs[i];
        p.SetInputCube(cubeName, att, ReadWrite);

        p.StartProcess(blend);
        p.EndProcess();
        p.ClearInputCubes();

        s1 = oStats.StartSampleY() - 1;
        s2 = oStats.EndSampleY() + 1;
        l1 = oStats.StartLineY() - 1;
        l2 = oStats.EndLineY() + 1;

        line = 1;

        cubeName = outputs[j];
        p.SetInputCube(cubeName, att, ReadWrite);

        p.StartProcess(blend);
        p.EndProcess();
        p.ClearInputCubes();

        delete blendRamp;
        blendRamp = NULL;

        delete i1;
        i1 = NULL;

        delete i2;
        i2 = NULL;
      }
    }

    // Make sure cube projection overlaps at least one other cube
    if (!hasOverlap) {
      string msg = "Input Cube [" + from1.getFilename() +
          "] does not overlap another cube";
      throw iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }
  }
}


void blend(Buffer &in) {
  for (int i = 0; i < in.size(); i++) {
    int sample = i + 1;
    int oSample = sample - s1 + 1;
    int oLine = line - l1 + 1;

    if (inIntersection(oSample, oLine) && validValue(oSample, oLine)) {
      double blendValue = blendRamp->GetValue(oSample, oLine);
      in[i] =
          i2->GetValue(oSample, oLine) * blendValue +
          i1->GetValue(oSample, oLine) * (1.0 - blendValue);
    }
    else {
      in[i] = in[i];
    }
  }

  line++;
}


bool inIntersection(int sample, int line) {
  int samples = blendRamp->Samples();
  int lines = blendRamp->Lines();
  return sample >= 1 && sample <= samples && line >= 1 && line <= lines;
}


bool validValue(int oSample, int oLine) {
  return blendRamp->GetValue(oSample, oLine) >= 0.0;
}


Chip * createRamp(Chip *pic1, Chip *pic2, int stop) {
  // x and y dimensions of the original pictures
  int x = pic1->Samples();
  int y = pic1->Lines();

  if (x != pic2->Samples() || y != pic2->Lines()) {
    string msg = "The two pictures need to be of the exact same dimensions";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }

  // Create the two overlap arrays
  QVector<int> ol1(x * y, -1);
  QVector<int> ol2(x * y, -1);

  // Lines and columns bounding data
  int r1 = y - 1;
  int r2 = 0;
  int c1 = x - 1;
  int c2 = 0;

  // Extract profiles of images
//   QQueue<int> nodes;
  int sum = 0;
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      int t1 = j * x + i;

      double tempval = pic1->GetValue(i + 1, j + 1);
      if (IsValidPixel(tempval)) ol1[t1] = 0;
      //if (tempval != 0.0) ol1[t1] = 0;

      tempval = pic2->GetValue(i + 1, j + 1);
      if (IsValidPixel(tempval)) ol2[t1] = 0;
      //if (tempval != 0.0) ol2[t1] = 0;

      // What does this do?
      if (ol1[t1] == 0) sum += ol2[t1];

      // Find left and right limits of overlapping area
      if (ol1[t1] != -1 && ol2[t1] != -1) {
        if (j < r1) r1 = j;
        if (j > r2) r2 = j;
        if (i < c1) c1 = i;
        if (i > c2) c2 = i;
      }
    }
  }

  // ol1 and ol2 were being left zero (sum==0) causing the ramp to fail setting
  // the outer edges of both ol1 and ol2 to -1 fixed the problem.
  if (sum == 0) {
    for (int i = 0; i < x; i++) {
      ol1[x * y - (i + 1)] = -1;
      ol1[i] = -1;
    }
    for (int j = 1; j < y; j++) {
      ol1[x * j] = -1;
      ol1[x * j - 1] = -1;
    }
  }

  // Loop through the overlap arrays filling in the appropriate value.  On the
  // first iteration we search for any pixels with a neighbor of -1 and we set
  // that pixel to 1.  On all other iterations we look for neighbors with values
  // num-1. If "stop" number is specified, we stop searching for distances and
  // set all values to value of "stop".
  int k = 1;
  int ct = 1; // Counter

  // Fill-in numbers
  int n = -1;
  int num = 1;
  while (ct > 0) {
    ct = 0;

    for (int i = c1; i <= c2; i++) {
      for (int j = r1; j <= r2; j++) {
        int t1 = j * x + i;

        // Neighbor pixel positions
        int up = (j - 1) * x + i;
        int down = (j + 1) * x + i;
        int left = t1 - 1;
        int right = t1 + 1;

        // Safety against falling off the edge
        if (j == 0) up = t1;
        if (j == y - 1) down = t1;
        if (i == 0) left = t1;
        if (i == x - 1) right = t1;

        if (ol1[t1] == 0 && ((ol1[left] == n) || (ol1[right] == n) ||
              (ol1[up] == n) || (ol1[down] == n))) {
          ol1[t1] = num;
          ct += 1;
        }

        // If the sum of the overlap == 0 then we don't need to do this part
        if (sum != 0) {
          if (ol2[t1] == 0 && ((ol2[left] == n) || (ol2[right] == n) ||
                (ol2[up] == n) || (ol2[down] == n))) {
            ol2[t1] = num;
            ct += 1;
          }
        }
      }
    }

    num += 1;
    n = num - 1;
    k += 1;

    // We've searched enough. Set all remaining values to 'stop'
    if (k == stop) {
      for (int i = c1; i <= c2; i++) {
        for (int j = r1; j <= r2; j++) {
          int t1 = j * x + i;
          if (ol1[t1] == 0) ol1[t1] = stop;
          if (sum != 0) {
            if (ol2[t1] == 0) ol2[t1] = stop;
          }
        }
      }

      ct = 0;
    }
  }

  // Loop through last time and create ramp using the special cases
  Chip *ramp = new Chip(x, y);
  ramp->SetAllValues(-1.0);
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      int t1 = j * x + i;
      if (ol1[t1] != -1 && ol2[t1] != -1) {
        double value = (sum != 0) ? ((double) ol2[t1]) / (ol2[t1] + ol1[t1]) :
            1.0 - ol1[t1] / (2.0 * k);
        ramp->SetValue(i + 1, j + 1, value);
      }
    }
  }

  return ramp;
}


void readOutputs(string outName, const FileList &inputs, FileList &outputs) {
  outputs.Read(outName);

  // Make sure each file in the tolist matches a file in the fromlist
  if (outputs.size() != inputs.size()) {
    string msg = "There must be exactly one output image in the TOLIST for "
        "each input image in the FROMLIST";
    throw iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }

  // Make sure that all output files do not have the same names as their
  // corresponding input files
  for (unsigned i = 0; i < outputs.size(); i++) {
    if (outputs[i].compare(inputs[i]) == 0) {
      string msg = "The to list file [" + outputs[i] +
        "] has the same name as its corresponding from list file.";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }
}


void generateOutputs(const FileList &inputs, FileList &outputs) {
  for (unsigned int i = 0; i < inputs.size(); i++) {
    Filename file(inputs[i]);
    iString filename = file.Path() + "/" + file.Basename() +
      ".blend." + file.Extension();
    outputs.push_back(filename);
  }
}

