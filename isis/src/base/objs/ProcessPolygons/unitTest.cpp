#include "Isis.h"
#include "ProcessPolygons.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "TextFile.h"

using namespace Isis;
using namespace std;

bool threeVectors(std::vector<double> &samples, std::vector<double> &lines, 
                  std::vector<double> &value);
bool oneBandOneValue(std::vector<double> &samples, std::vector<double> &lines, 
                     int &band,double &value);
void writeAscii(Isis::Buffer &in);

std::vector<double> diamondSamples, diamondLines, values1, values2;
std::vector<int> bands;

void IsisMain() {
  std::vector<double> samples, lines, values;

  cout << "Testing Isis::ProcessPolygons Class ... " << endl;
  ProcessPolygons p;
  ProcessByLine pbl;

  CubeAttributeOutput out_atts;
  out_atts.PixelType(Real);

  const string output1 = "processPolygonsTest.cub";
  const string output2 = "processPolygonsTest_count.cub";
  p.SetOutputCube(output1 , output2 , out_atts, 4, 4, 2);

  samples.clear();
  lines.clear();
  
  double value = 0;
  for (int b = 1; b<3; b++) {
    for(int l = 0; l<4; l++) {
      for(int s = 0; s<4; s++) { 
    
        samples.push_back(s);
        samples.push_back(s+5.0);
        samples.push_back(s);
        samples.push_back(s-5.0);
       
        lines.push_back(l-5.0);
        lines.push_back(l);
        lines.push_back(l+5.0);
        lines.push_back(l);
        value = s+l+b;

        p.Rasterize(samples, lines, b, value);
    
        samples.clear();
        lines.clear();
      }
    }
  }
  
  p.EndProcess();


  /*   Write out the data    */

 CubeAttributeInput atts;

  cout << "Filename: " << output1 << endl;
  pbl.SetInputCube(output1, atts, 0);
  pbl.StartProcess(writeAscii);
  pbl.EndProcess();

  cout << "Filename: " << output2 << endl;
  pbl.SetInputCube(output2, atts, 0);
  pbl.StartProcess(writeAscii);
  pbl.EndProcess();

  remove(output1.c_str());
  remove(output2.c_str());
}

/** 
 * This method reads in the cube file line by line and prints 
 * out the DN value of each non-null pixel. 
 * 
 * @param in
 */
void writeAscii (Isis::Buffer &in) {
  bool notNull= false; 
  int index = in.size() - 1;
  for (int i=0; i<in.size(); i++) {
    if(in[index -i] > 0) {
      cout <<"Band: " << in.Band() << " DN: " << in[index -i] << " ";
      notNull = true;
    }   
  }
  if(notNull) {
    cout << std::endl;
  }
}

