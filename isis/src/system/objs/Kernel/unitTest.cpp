#include <iostream>

#include <QString>
#include <QStringList>

#include "IException.h"
#include "Kernel.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

void printKernelInfo(Kernel &kernel);
void printLessThanOperatorComparisons();

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  Kernel::Type predicted     = Kernel::Type(1);
  Kernel::Type nadir         = Kernel::Type(2);
  Kernel::Type reconstructed = Kernel::Type(4);
  Kernel::Type smithed       = Kernel::Type(8);
  Kernel::Type unknown       = Kernel::Type(0);
  Kernel::Type junk          = Kernel::Type(3);

  cout << "Testing static conversion methods..." << endl;
  cout << endl;
  cout << "Convert Type to char array..." << endl;
  cout << "1     " << Kernel::typeEnum(predicted) << endl;    
  cout << "2     " << Kernel::typeEnum(nadir) << endl;        
  cout << "4     " << Kernel::typeEnum(reconstructed) << endl;
  cout << "8     " << Kernel::typeEnum(smithed) << endl;      
  cout << "0     " << Kernel::typeEnum(unknown) << endl;
  cout << "3     " << Kernel::typeEnum(junk) << endl;
  cout << endl;
  cout << "Convert QString to Type..." << endl;
  cout << "Predicted     " << Kernel::typeEnum("Predicted") << endl;
  cout << "NADIR         " << Kernel::typeEnum("NADIR") << endl;
  cout << "reconstructed " << Kernel::typeEnum("reconstructed") << endl;
  cout << "SmItHeD       " << Kernel::typeEnum("SmItHeD") << endl;
  cout << "Unknown       " << Kernel::typeEnum("Unknown") << endl;
  cout << "junk          " << Kernel::typeEnum("junk") << endl;
  cout << endl;
  cout << endl;
  cout << endl;
  cout << "Testing empty constructor Kernel()... " << endl;
  Kernel kern;
  printKernelInfo(kern);
  
  cout << "Testing setKernels and setType... " << endl;
  QStringList data;
  data.push_back("kernelFile1"); 
  data.push_back("kernelFile2"); 
  kern.setKernels(data);
  kern.setType(predicted);
  printKernelInfo(kern);

  cout << "Testing constructor Kernel(type, data)... " << endl;
  Kernel otherKernel(nadir,  data);
  printKernelInfo(otherKernel);

  cout << "Testing kernel.push_back()... " << endl;
  otherKernel.push_back("kernelFile3");
  kern.setType(reconstructed);
  printKernelInfo(otherKernel);

  cout << "Test < operator: is row < column? " << endl;
  printLessThanOperatorComparisons();
  cout << endl;
  cout << endl;
  cout << endl;
  cout << "Testing errors... " << endl;
  cout << endl;
  cout << "Testing invalid enumeration: " << endl;
  cout << endl;
  return 0;
}

/**
 * Method that prints the kernel information
 * 
 * @param kernel Kernel object
 */
void printKernelInfo(Kernel &kernel) {
  cout << "Kernel Type (enum): " << kernel.type() << endl;
  cout << "Kernel Type (QString): " << Kernel::typeEnum(kernel.type()) << endl;

  QStringList kFiles = kernel.kernels();
  cout << "Number of Kernel Files: " << kFiles.size() << endl;
  cout << "Kernel Files: " << endl;
  for (int i = 0; i < kFiles.size(); i++) {
    cout << "\t" << kFiles[i] << endl;
  }
  cout << "Alternate way of accessing info, should match previous lines" << endl;
  cout << "Number of Kernel Files: " << kernel.size() << endl;
  cout << "Kernel Files: " << endl;
  for (int i = 0; i < kernel.size(); i++) {
    cout << "\t" << kernel[i] << endl;
  }
  cout << endl;
  cout << endl;
  cout << endl;
  return;
}

/**
 * Method that prints the comparison table using the Kernel class definition of 
 * the < operator 
 */
void printLessThanOperatorComparisons() {
  QStringList data;
  Kernel unknown(Kernel::Type(0),  data);
  Kernel predicted(Kernel::Type(1),  data);
  Kernel nadir(Kernel::Type(2),  data);
  Kernel reconstructed(Kernel::Type(4),  data);
  Kernel smithed(Kernel::Type(8),  data);
  cout << "\t\t\tUnknown\tPredicted\tNadir\tRecon\tSmithed" << endl; 
  cout << "Unknown\t\t\t"   << (unknown < unknown) << "\t\t"
                            << (unknown < predicted) << "\t\t"
                            << (unknown < nadir) << "\t\t"
                            << (unknown < reconstructed) << "\t\t" 
                            << (unknown < smithed) << endl; 
  cout << "Predicted\t\t"   << (predicted < unknown) << "\t\t"
                            << (predicted < predicted) << "\t\t" 
                            << (predicted < nadir) << "\t\t" 
                            << (predicted < reconstructed) << "\t\t" 
                            << (predicted < smithed) << endl; 
  cout << "Nadir\t\t\t"     << (nadir < unknown) << "\t\t" 
                            << (nadir < predicted) << "\t\t" 
                            << (nadir < nadir) << "\t\t" 
                            << (nadir < reconstructed) << "\t\t" 
                            << (nadir < smithed) << endl; 
  cout << "Reconstructed\t" << (reconstructed < unknown) << "\t\t" 
                            << (reconstructed < predicted) << "\t\t" 
                            << (reconstructed < nadir) << "\t\t" 
                            << (reconstructed < reconstructed) << "\t\t" 
                            << (reconstructed < smithed) << endl; 
  cout << "Smithed\t\t\t"   << (smithed < unknown) << "\t\t" 
                            << (smithed < predicted) << "\t\t" 
                            << (smithed < nadir) << "\t\t" 
                            << (smithed < reconstructed) << "\t\t" 
                            << (smithed < smithed) << endl; 
}
