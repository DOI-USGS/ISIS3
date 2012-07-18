#include "Preference.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "Kernels.h"

//  Used extensively in Kernels interface
typedef std::vector<std::string>  StrList;

using namespace std;
using namespace Isis;

string stripPath(string input) {
  QString result = QString::fromStdString(input).replace(
      QRegExp("(.*/)([^/]*/[^/]*/[^/]*/[^/]*$)"),
      "$\\2");

  return result.toStdString();
}

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  string inputFile = "$ISIS3DATA/mgs/testData/ab102401.lev2.cub";
  if (--argc == 1) { inputFile = argv[1]; }

  cout << "\n\nTesting Kernels class using file " << inputFile << "\n";

  Kernels myKernels(inputFile);
  cout << "\nList of kernels found - Total: " << myKernels.size() << "\n";
  StrList kfiles = myKernels.getKernelList();
  transform(kfiles.begin(), kfiles.end(), kfiles.begin(), &stripPath);
  copy(kfiles.begin(), kfiles.end(), ostream_iterator<std::string>(cout, "\n"));

  cout << "\nTypes of kernels found\n";
  StrList ktypes = myKernels.getKernelTypes();
  copy(ktypes.begin(), ktypes.end(), ostream_iterator<std::string>(cout, "\n"));

  //  Test to see if we have any kernels loaded at all
  Kernels query;
  query.Discover();
  cout << "\nInitial currently loaded kernel files = " << query.size() << "\n";
  StrList kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  //  Load all the kernels
  myKernels.Load();
  query.Discover();
  cout << "\nAfter LoadALL option, kernels loaded = " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  // Unload and check for proper status
  myKernels.UnLoad();
  query.Discover();
  cout << "\nUnLoading All, count after = " << query.size() << "\n";

  //  Now load the SPK kernels after unloading
  myKernels.Load("SPK");
  query.Discover();
  cout << "\nLoaded SPK kernels = " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  // Load kernels needed for Time manipulation
  myKernels.Load("LSK,SCLK");
  myKernels.UnLoad("SPK");
  query.Discover();
  cout << "\nLoad LSK, SCLK for Time manip, unload SPK kernels = " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  // Check double load behavior
  Kernels clone;
  clone.Merge(query);
  clone.Manage();
  clone.UnLoad();

  myKernels.UpdateLoadStatus();
  cout << "\nNumber loaded: " << myKernels.getLoadedList().size() << "\n";
  myKernels.Load("LSK,SCLK");
  // Load same files
  clone.Load();
  query.Discover();
  cout << "\nCheck Double-Load of LSK, SCLK = " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  // Unload each set
  clone.UnLoad();
  query.Discover();
  cout << "\nUnload the cloned set = " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));
  clone.UnManage();

  //  Load SPK set
  myKernels.UnLoad();
  myKernels.Load("LSK,FK,DAF,SPK");
  query.Discover();
  cout << "\nCheck SPK load  (LSK,FK,DAF,SPK)= " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  // Now unload SPKs, preserve LSK and load CK stuff
  myKernels.UnLoad("DAF,SPK");
  cout << "Unload DAF,SPK\n";
  myKernels.Load("SCLK,IK,CK");
  query.Discover();
  cout << "\nCheck CK load  (SCLK,IK,CK) = " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  // Now reload all and check
  myKernels.Load("LSK,FK,SCLK,IK,CK");
  query.Discover();
  cout << "\nCheck CK reload  (LSK,FK,SCLK,IK,CK) = " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  //  Clear the pool and start fresh.  Clear all instances and reinitialize NAIF
  clone.Clear();
  query.Clear();
  myKernels.Clear();
  myKernels.InitializeNaifKernelPool();

  // Left two kernels open, ensure we have none left
  query.Discover();
  cout << "\n\nEnsure clean pool...Count: " << query.size() << "\n";

  // Load a bogus file and check for missing
  myKernels.Add("$base/kernels/lsk/dne.lsk");
  cout << "\nLoad of bogus file, should have one missing: " << myKernels.Missing()
       << "\n";
  myKernels.Clear();

  // Now add a set be hand
  myKernels.Add("$base/kernels/lsk/naif0009.tls");
  myKernels.Add("$base/kernels/spk/de405.bsp");
  myKernels.Add("$clementine1/kernels/ck/clem_ulcn2005_type2_1sc.bc");
  myKernels.Add("$clementine1/kernels/fk/clem_v11.tf");
  myKernels.Add("$clementine1/kernels/sclk/dspse002.tsc");
  myKernels.Add("$clementine1/kernels/spk/SPKMERGE_940219_940504_CLEMV001b.bsp");
  myKernels.Add("$clementine1/kernels/iak/uvvisAddendum003.ti");

  cout << "\n\nAdd Kernels directly - Count: " << myKernels.size() 
       << ", Missing: "<< myKernels.Missing() << "\n";
  cout << "\nList of kernels in object..\n";
  kfiles = myKernels.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kfiles.begin(), kfiles.end(), ostream_iterator<std::string>(cout, "\n"));

  cout << "\nList of kernel types\n";
  ktypes = myKernels.getKernelTypes();
  copy(ktypes.begin(), ktypes.end(), ostream_iterator<std::string>(cout, "\n"));

  // Find unknown types
  kfiles = myKernels.getKernelList("UNKNOWN");
  cout << "\nUnknown kernels in list: " << kfiles.size() << "\n";
  copy(kfiles.begin(), kfiles.end(), ostream_iterator<std::string>(cout, "\n"));


  // Load them all
  myKernels.Load();
  kloaded = myKernels.getLoadedList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  cout << "\nLoading all, total loaded: " << kloaded.size() << "\n";
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  //  Now double check list
  query.Discover();
  cout << "\nCheck Load Status = " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  // Unload SPK and CKs
  myKernels.UnLoad("SPK,CK");
  query.Discover();
  cout << "\nUnload SPK,CK - Loaded: " << query.size() << "\n";
  kloaded = query.getKernelList();
  transform(kloaded.begin(), kloaded.end(), kloaded.begin(), &stripPath);
  copy(kloaded.begin(), kloaded.end(), ostream_iterator<std::string>(cout, "\n"));

  myKernels.UnLoad();
  query.Discover();
  cout << "\n\nAll Done - Should be 0 discovered: " << query.size() << "\n";
  // All done...

  return (0);
}
