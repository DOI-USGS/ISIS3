#include <cmath>
#include "Isis.h"
#include "ProcessByBoxcar.h"
#include "Pvl.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "Filename.h"
#include "iException.h"

using namespace std; 
using namespace Isis;

void useFilter (Buffer &in, double &v);
void setFilter (int size, double stdDev);
double *coefs;
void IsisMain() {

  ProcessByBoxcar p;
  p.SetInputCube ("FROM");
  p.SetOutputCube ("TO");
  
  //Set up the user interface
  UserInterface &ui = Application::GetUserInterface();

  //Get the standard deviation from the user
  double stdDev = ui.GetDouble("STDDEV");
  
  //Get the length of each side of the kernel from the user
  // (Note: kernel is always square)
  int size = ui.GetInteger("SIZE");
  
  //Set the Boxcar size based on the input size
  p.SetBoxcarSize (size,size);
  
  //Reference a pointer to an array of kernel data values
  coefs = new double[size*size]; 
  setFilter (size,stdDev);  
  
  p.StartProcess(useFilter); 
  p.EndProcess ();
  
  delete [] coefs;
}

void setFilter (int size, double stdDev){
  //Iterate through the input kernel's data values to fill the coefs array
  const double PI=3.141592653589793;
  int i =0;
  cout << exp((double)1.0) << endl;
  for (double y= -(size/2) ; y <= (size/2) ; y++){
    for (double x=-(size/2) ; x <= (size/2) ; x++){	
      /*
      Assign gaussian weights based on the following equation
                                                    x^2+y^2
                                               -- ----------- 
                G(x,y) =           1              2(stdDev)^2
      			    --------------- e^ 
			    2(pi)(stdDev)^2
      */
      
      coefs[i] = (x * x ) + (y * y);
      coefs[i] = (coefs[i] ) / (-2 * (stdDev * stdDev));
      coefs[i] = exp (coefs[i]);
      coefs[i] = (1 / (2 * PI * stdDev * stdDev) ) * coefs [i];
      i++;
    }
  }
}


void useFilter (Buffer &in, double &v){
  v = 0;
  for (int i= 0 ; i < in.size() ; i++){
      if (!IsSpecial(in[i])){
        v += in[i] * coefs[i] ;
      }
    }
}
