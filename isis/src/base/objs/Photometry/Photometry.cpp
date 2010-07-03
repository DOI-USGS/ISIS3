#include "Pvl.h"
#include "Photometry.h"
#include "PhotoModelFactory.h"
#include "PhotoModel.h"
#include "AtmosModelFactory.h"
#include "AtmosModel.h"
#include "NormModelFactory.h"
#include "NormModel.h"
#include "Plugin.h"
#include "Filename.h"

namespace Isis {
  /**
   * Create Photometry object.
   * 
   * @param pvl  A pvl object containing a valid Photometry specification
   * 
   * @see photometry.doc
   */
  Photometry::Photometry (Pvl &pvl) {
    p_phtAmodel = NULL;
    p_phtPmodel = NULL;
    p_phtNmodel = NULL;
    p_phtPmodel = PhotoModelFactory::Create(pvl);
    if (pvl.HasObject("AtmosphericModel")) {
      p_phtAmodel = AtmosModelFactory::Create(pvl, *p_phtPmodel);
      p_phtNmodel = NormModelFactory::Create(pvl, *p_phtPmodel,
          *p_phtAmodel);
    } else {
      p_phtNmodel = NormModelFactory::Create(pvl, *p_phtPmodel);
    }
  }

  //! Destroy Photometry object
  Photometry::~Photometry() {
    if(p_phtAmodel != NULL) {
      delete p_phtAmodel;
      p_phtAmodel = NULL;
    }

    if(p_phtPmodel != NULL) {
      delete p_phtPmodel;
      p_phtPmodel = NULL;
    }

    if(p_phtNmodel != NULL) {
      delete p_phtNmodel;
      p_phtNmodel = NULL;
    }
  }

  /**
   * Set the wavelength parameter. This value is obtained
   * from the BandBin Center keyword of the image.
   *
   */
  void Photometry::SetPhotomWl(double wl) {
    p_phtNmodel->SetNormWavelength(wl);
  }

  /**
   * Calculate the surface brightness using only ellipsoid
   * 
   * @return  Returns the surface brightness 
   *          
   */
  void Photometry::Compute(double pha, double inc, double ema,
                           double dn, double &albedo, double &mult,
			   double &base) {

    // Calculate the surface brightness
    p_phtNmodel->CalcNrmAlbedo(pha,inc,ema,dn,albedo,mult,base);
    return;
  }

  /**
   * Calculate the surface brightness using ellipsoid and dem
   * 
   * @return  Returns the surface brightness 
   *          
   */
  void Photometry::Compute(double pha, double inc, double ema,
                           double deminc, double demema, double dn,
			   double &albedo, double &mult, double &base) {

    // Calculate the surface brightness
    p_phtNmodel->CalcNrmAlbedo(pha,inc,ema,deminc,demema,dn,albedo,mult,base);
    return;
  }
}
