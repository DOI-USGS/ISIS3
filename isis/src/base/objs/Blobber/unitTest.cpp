#include <iostream>
#include "Preference.h"
#include "SpecialPixel.h"
#include "Blobber.h"
#include "iException.h"
#include "Cube.h"

using namespace std;
using namespace Isis;

int main (int argc, char *argv[]) {
  int nfailed(0);
  try {

      std::cout << "*** Blobber unitTest ***\n";
      Preference::Preferences(true);
      string fname = "$base/testData/blobTruth.cub";
      std::cout << "\nFile Source: " << fname << std::endl;

// This is assumed to be Hirise cube source
      string blobName  = "HiRISE Calibration Ancillary";
      string fieldName = "DarkPixels";
      string name      = "Blobber";

// Test access from Cube object class
      std::cout << "\n** Test Cube Class access...\n";
      Cube cube;
      cube.Open(fname);
      Blobber blob1(cube, blobName, fieldName, name);
      std::cout << "Blob Name:           " << blob1.getName() << std::endl;
      std::cout << "Object Name:         " << blob1.getBlobName() << std::endl;
      std::cout << "Field Name:          " << blob1.getFieldName() << std::endl;
      std::cout << "Number Blob Lines:   " << blob1.Lines() << std::endl;
      std::cout << "Number Blob Samples: " << blob1.Samples() << std::endl;
      std::cout << "Total Pixels:        " << blob1.size() << std::endl;

      BigInt lines(blob1.Lines()), samples(blob1.Samples()), 
             ntotal(blob1.size());

//  Ensure product of lines & sample are equal to size()
      if (ntotal != (lines * samples)) {
        std::cout << "--> FAILED < Total Blob Size (" << ntotal 
                  << ") does not equal product of Lines, Samples ("
                  << (lines * samples) << ")!\n";
        nfailed++;
      }

//  Test access to pixels
      double pixel0 = blob1[0][0];
      double pixelN = blob1[lines-1][samples-1];
      double pixelx = blob1[lines/2][samples/2];

      std::cout << "\n** Blob Values...\n";
      std::cout << "Pixel[0][0] = " << PixelToString(pixel0) << std::endl;
      std::cout << "Pixel[" << (lines/2) << "][" << (samples/2) << "] = "
                << PixelToString(pixelx) << std::endl;
      std::cout << "Pixel[" << (lines-1) << "][" << (samples-1) << "] = "
                << PixelToString(pixelN) << std::endl;

//  Now reload using Cube explicit method
      blob1.load(cube);
      if (pixel0 != blob1[0][0]) {
        std::cout << "--> FAILED < Pixel[Line=0][Sample=0] = " << pixel0
                  << "  != " << blob1[0][0] << " after reload!!" << std::endl;
        nfailed++;
      }

      if (pixelx != blob1[lines/2][samples/2]) {
        std::cout << "--> FAILED < Pixel[Line=" << (lines/2) 
                  << "][Sample=" << (samples/2)
                  << "] = " << pixelx << "  != " 
                  << blob1[lines/2][samples/2]
                  << " after reload!!" << std::endl;
        nfailed++;
      }


      if (pixelN != blob1[lines-1][samples-1]) {
        std::cout << "--> FAILED < Pixel[Line=" << (lines-1) 
                  << "][Sample=" << (samples-1)
                  << "] = " << pixelN << "  != " 
                  << blob1[lines-1][samples-1]
                  << " after reload!!" << std::endl;
        nfailed++;
      }

// Test Cube access to blob
      std::cout << "\n** Test Blobber direct access (via filename)...\n";
      Blobber blob3(blobName, fieldName, name);
      blob3.load(fname);

      if (pixelx != blob3[lines/2][samples/2]) {
        std::cout << "--> FAILED < Pixel[Line=" << (lines/2) 
                  << "][Sample=" << (samples/2)
                  << "] = " << pixelx << "  != " 
                  << blob1[lines/2][samples/2]
                  << " from named file source!!" << std::endl;
        nfailed++;
      }

// Test copying
      std::cout << "\n** Test Blobber(Blobber) constructor...\n";
      Blobber blob4(blob1);
      if (pixelx != blob4[lines/2][samples/2]) {
        std::cout << "--> FAILED < Pixel[Line=" << (lines/2) 
                  << "][Sample=" << (samples/2)
                  << "] = " << pixelx << "  != " 
                  << blob1[lines/2][samples/2]
                  << " from Blobber(blob) constructor!!" << std::endl;
        nfailed++;

     }

      std::cout << "\n** Test Blobber copy assignment (constructor)...\n";
      Blobber blob5 = blob1;
      if (pixelx != blob5[lines/2][samples/2]) {
        std::cout << "--> FAILED < Pixel[Line=" << (lines/2) 
                  << "][Sample=" << (samples/2)
                  << "] = " << pixelx << "  != " 
                  << blob1[lines/2][samples/2]
                  << " from Blobber blob = oblob constructor!!" << std::endl;
        nfailed++;
      }


//  Test deep copy
     std::cout << "\n** Test Blobber deep copy...\n";
     Blobber blob6;
     blob6 = blob1.deepcopy();
      if (pixelx != blob6[lines/2][samples/2]) {
        std::cout << "--> FAILED < Pixel[Line=" << (lines/2) 
                  << "][Sample=" << (samples/2)
                  << "] = " << pixelx << "  != " 
                  << blob1[lines/2][samples/2]
                  << " from deepcopy!!" << std::endl;
        nfailed++;
      }

//  Now change pixel in copied version
      blob6[lines/2][samples/2] += 1.0;
      if (blob1[lines/2][samples/2] == blob6[lines/2][samples/2]) {
        std::cout << "--> FAILED < Pixel[Line=" << (lines/2) 
                  << "][Sample=" << (samples/2)
                  << "] = " << pixelx << "  == " 
                  << blob1[lines/2][samples/2]
                  << " deepcopy failed to make separate copy!!" << std::endl;
        nfailed++;
      }
  }
  catch (Isis::iException &e) {
    e.Report();
  }

  //  Return status
  return (nfailed);
}

