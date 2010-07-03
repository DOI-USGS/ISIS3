#include "LoCameraFiducialMap.h"
#include "CameraGroundMap.h"
#include "CameraSkyMap.h"
#include "iString.h"
#include "Affine.h"

using namespace std;
namespace Isis {
  namespace Lo {
    /** Construct mapping between Lunar Orbiter detectors and focal plane x/y
     *
     * @param naifIkCode  Naif code of the Lunar Orbiter instrument for reading coefficients
     *
     */
    LoCameraFiducialMap::LoCameraFiducialMap(PvlGroup &inst, const int naifIkCode) {
      // Get the Instrument label information needed to define the fiducial map for this frame
      p_naifIkCode = naifIkCode;
      ReadFiducials(inst);

      // Set the x-axis direction.  The medium camera is reversed.
      int xdir;
      if (naifIkCode%2 == 0) {
        xdir = -1;
      }
      else {
        xdir = 1;
      }
      CreateTrans( xdir );
    }


    void LoCameraFiducialMap::ReadFiducials(PvlGroup &inst) {

      // Try to read the fiducials from the labels
      try {
        // Fiducial mapping to define the Focal Plane map
        PvlKeyword &fSamps = inst["FiducialSamples"];
        PvlKeyword &fLines = inst["FiducialLines"];
        PvlKeyword &fXs = inst["FiducialXCoordinates"];
        PvlKeyword &fYs = inst["FiducialYCoordinates"];

        for (int i=0; i<fSamps.Size(); i++) {
          p_fidSamples.push_back(fSamps[i]);
          p_fidLines.push_back(fLines[i]);
          p_fidXCoords.push_back(fXs[i]);
          p_fidYCoords.push_back(fYs[i]);
        }
      } catch ( iException &e ) {
        std::string msg = "Unable to read fiducial mapping from cube labels - ";
        msg += "Input cube must be processed in Isis 2 through lofixlabel ";
        msg += "and converted to Isis 3 with pds2isis";
        throw Isis::iException::Message(iException::User,msg,_FILEINFO_);
      }
    }

    void LoCameraFiducialMap::CreateTrans( int xdir ) {
      // Setup focal plane map
      Affine *fptrans = new Affine();

      try {
        fptrans->Solve ( &p_fidSamples[0], (double *) &p_fidLines[0],
                   (double *) &p_fidXCoords[0], (double *) &p_fidYCoords[0],
                   p_fidSamples.size());

      } catch ( iException &e ) {
        std::string msg = "Unable to create fiducial map";
        throw Isis::iException::Message(iException::User,msg,_FILEINFO_);
      }

      // Get the coefficients
      vector<double> transx;
      vector<double> transy;
      transx = fptrans->Coefficients(1);
      transy = fptrans->Coefficients(2);

      // Medium camera has a reversed x-axis
      for (int icoef=0; icoef<3; icoef++) {transx[icoef] *= xdir;};

      // Correct the Affine order - move the constant to the front
      transx.insert(transx.begin(), transx[2]);
      transx.pop_back();
      transy.insert(transy.begin(), transy[2]);
      transy.pop_back();

      string icode = "INS" + iString(p_naifIkCode);
      string icodex = icode + "_TRANSX";
      string icodey = icode + "_TRANSY";
      pdpool_c(icodex.c_str(), 3, (double (*)) &transx[0]);
      pdpool_c(icodey.c_str(), 3, (double (*)) &transy[0]);

      vector<double> transs;
      vector<double> transl;
      transs = fptrans->InverseCoefficients(1);
      transl = fptrans->InverseCoefficients(2);

      // Correct the Affine order - move the constant to the front
      transs.insert(transs.begin(), transs[2]);
      transs.pop_back();
      transl.insert(transl.begin(), transl[2]);
      transl.pop_back();

      // Medium camera has a reversed x-axis
      transs[1] *= xdir;
      transl[1] *= xdir;

      string icodes = icode + "_ITRANSS";
      string icodel = icode + "_ITRANSL";
      pdpool_c(icodes.c_str(), 3, (double (*)) &transs[0]);
      pdpool_c(icodel.c_str(), 3, (double (*)) &transl[0]);
    }
  }
}

