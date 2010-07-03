/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2010/05/18 06:38:05 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <memory>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "Camera.h"
#include "Exponential.h"
#include "DbProfile.h"
#include "PvlObject.h"
#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

using namespace std;

namespace Isis {

  /**
   * @brief Method to get photometric property given angles
   *
   * This routine computes the photometric property at the given
   * cube location after ensuring a proper parameter container is
   * found for the specified band.
   *
   * @author Kris Becker - 2/21/2010
   *
   * @param i     Incidence angle at cube location
   * @param e     Emission angle at cube location
   * @param g     Phase angle at cube location
   * @param band  Band number in cube (actually is band index) for
   *              lookup purposes
   *
   * @return double Returns photometric correction using
   *         parameters
   */
  double Exponential::photometry(double i, double e, double g, int band) const {
    // Test for valid band
    if((band <= 0) || (band > (int) _bandpho.size())) {
      std::string mess = "Provided band " + iString(band) + " out of range.";
      throw iException::Message(iException::Programmer, mess, _FILEINFO_);
    }
    double ph = photometry(_bandpho[band - 1], i, e, g);
    return (_bandpho[band - 1].phoStd / ph);
  }

  /**
   * @brief Performs actual photometric correction calculations
   *
   * This routine computes photometric correction using parameters
   * for the Exponential-Buratti-Hill equation.
   *
   * @author Kris Becker - 2/21/2010
   *
   * @param parms Container of band-specific Exponential parameters
   * @param i     Incidence angle in degrees
   * @param e     Emission angle in degrees
   * @param g     Phase angle in degrees
   *
   * @return double Photometric correction parameter
   */
  double Exponential::photometry(const Parameters &parms, double i, double e, double g) const {

    //  Ensure problematic values are adjusted
    if(i == 0.0)
      i = 10.E-12;
    if(e == 0.0)
      e = 10.E-12;

    // Convert to radians
    i *= rpd_c();
    e *= rpd_c();
    g *= parms.phaUnit; //  Apply unit normalizer

    // Compute Lommel-Seeliger components
    double mu = cos(e);
    double mu0 = cos(i);

    double alpha = g;

    // Simple Exponential photometric polynomial equation with exponential opposition
    //  surge term.
    double rcal = 0.0;
    for(unsigned int i = 0; i < parms.aTerms.size(); i++) {
      rcal += parms.aTerms[i] * exp(parms.bTerms[i] * alpha);
    }

    return ((mu0 / (mu + mu0)) * rcal);
  }

  /**
   * @brief Return parameters used for all bands
   *
   * Method creates keyword vectors of band specific parameters
   * used in the photometric correction.
   *
   * @author Kris Becker - 2/22/2010
   *
   * @param pvl Output PVL container write keywords
   */
  void Exponential::Report(PvlContainer &pvl) {
    pvl += PvlKeyword("Algorithm", "Exponential");
    pvl += PvlKeyword("IncRef", _iRef, "degrees");
    pvl += PvlKeyword("EmaRef", _eRef, "degrees");
    pvl += PvlKeyword("PhaRef", _gRef, "degrees");
    PvlKeyword units("ExponentialUnits");
    PvlKeyword phostd("PhotometricStandard");
    PvlKeyword bbc("BandBinCenter");
    PvlKeyword bbct("BandBinCenterTolerance");
    PvlKeyword bbn("BandNumber");

    std::vector<PvlKeyword> aTermKeywords;
    std::vector<PvlKeyword> bTermKeywords;
    for(unsigned int i = 0; i < _bandpho[0].aTerms.size(); i++)
      aTermKeywords.push_back(PvlKeyword("A" + iString((int) i)));
    for(unsigned int i = 0; i < _bandpho[0].bTerms.size(); i++)
      bTermKeywords.push_back(PvlKeyword("B" + iString((int) i)));

    for(unsigned int i = 0; i < _bandpho.size(); i++) {
      Parameters &p = _bandpho[i];
      units.AddValue(p.units);
      phostd.AddValue(p.phoStd);
      bbc.AddValue(p.wavelength);
      bbct.AddValue(p.tolerance);
      bbn.AddValue(p.band);
      for(unsigned int j = 0; j < aTermKeywords.size(); j++)
        aTermKeywords[j].AddValue(p.aTerms[j]);
      for(unsigned int j = 0; j < bTermKeywords.size(); j++)
        bTermKeywords[j].AddValue(p.bTerms[j]);
    }
    pvl += units;
    pvl += phostd;
    pvl += bbc;
    pvl += bbct;
    pvl += bbn;
    for(unsigned int i = 0; i < aTermKeywords.size(); i++)
      pvl += aTermKeywords[i];

    for(unsigned int i = 0; i < bTermKeywords.size(); i++)
      pvl += bTermKeywords[i];

    return;
  }

  /**
   * @brief Determine Exponential parameters given a wavelength
   *
   * This method determines the set of Exponential parameters to use
   * for a given wavelength.  It iterates through all band
   * profiles as read from the PVL file and computes the
   * difference between the "wavelength" parameter and the
   * BandBinCenter keyword.  The absolute value of this value is
   * checked against the BandBinCenterTolerance paramter and if it
   * is less than or equal to it, a Parameter container is
   * returned.
   *
   * @author Kris Becker - 2/22/2010
   *
   * @param wavelength Wavelength used to find parameter set
   *
   * @return Exponential::Parameters Container of valid values.  If
   *         not found, a value of iProfile = -1 is returned.
   */
  Exponential::Parameters Exponential::findParameters(const double wavelength) const {
    for(unsigned int i = 0; i < _profiles.size(); i++) {
      const DbProfile &p = _profiles[i];
      if(p.exists("BandBinCenter")) {
        double p_center = ConfKey(p, "BandBinCenter", Null);
        double tolerance = ConfKey(p, "BandBinCenterTolerance", 1.0E-6);
        if(fabs(wavelength - p_center) <= fabs(tolerance)) {
          Parameters pars = extract(p);
          pars.iProfile = i;
          pars.wavelength = wavelength;
          pars.tolerance = tolerance;
          return (pars);
        }
      }
    }

    // Not found if we reach here
    return (Parameters());
  }

  /**
   * @brief Extracts necessary Exponential parameters from profile
   *
   * Given a profile read from the input PVL file, this method
   * extracts needed parameters (from Keywords) in the PVL profile
   * and creates a container of the converted values.
   *
   * @author Kris Becker - 2/22/2010
   *
   * @param p Profile to extract/convert
   *
   * @return Exponential::Parameters Container of extracted values
   */
  Exponential::Parameters Exponential::extract(const DbProfile &p) const {
    Parameters pars;

    for(int i = 0; i < p.size(); i++) {
      if(p.exists("A" + iString(i)) || p.exists("B" + iString(i))) {
        pars.aTerms.push_back(ConfKey(p, "A" + iString(i), 1.0));
        pars.bTerms.push_back(ConfKey(p, "B" + iString(i), 0.0));
      }
    }

    pars.wavelength = ConfKey(p, "BandBinCenter", Null);
    pars.tolerance = ConfKey(p, "BandBinCenterTolerance", Null);
    //  Determine equation units - defaults to Radians
    pars.units = ConfKey(p, "ExponentialUnits", iString("Radians"));
    pars.phaUnit = (iString::Equal(pars.units, "Degrees")) ? 1.0 : rpd_c();
    return (pars);
  }

  /**
   * @brief Initialize class from input PVL and Cube files
   *
   * This method is typically called at class instantiation time,
   * but is reentrant.  It reads the parameter PVL file and
   * extracts Photometric model and Normalization models from it.
   * The cube is needed to match all potential profiles for each
   * band.
   *
   * @author Kris Becker - 2/22/2010
   *
   * @param pvl  Input PVL parameter files
   * @param cube Input cube file to correct
   */
  void Exponential::init(PvlObject &pvl, Cube &cube) {

    //  Make it reentrant
    _profiles.clear();
    _bandpho.clear();

    //  Interate over all Photometric groups
    _normProf = DbProfile(pvl.FindObject("NormalizationModel").FindGroup("Algorithm", Pvl::Traverse));
    _iRef = ConfKey(_normProf, "IncRef", 30.0);
    _eRef = ConfKey(_normProf, "EmaRef", 0.0);
    _gRef = ConfKey(_normProf, "PhaRef", _iRef);

    PvlObject &phoObj = pvl.FindObject("PhotometricModel");
    DbProfile phoProf = DbProfile(phoObj);
    PvlObject::PvlGroupIterator algo = phoObj.BeginGroup();
    while(algo != phoObj.EndGroup()) {
      if(iString::Equal(algo->Name(), "Algorithm")) {
        _profiles.push_back(DbProfile(phoProf, DbProfile(*algo)));
      }
      ++algo;
    }

    Pvl *label = cube.Label();
    PvlKeyword center = label->FindGroup("BandBin", Pvl::Traverse)["Center"];
    string errs("");
    for(int i = 0; i < cube.Bands(); i++) {
      Parameters parms = findParameters(center[i]);
      if(parms.IsValid()) {
        parms.band = i + 1;
        _camera->SetBand(i + 1);
        parms.phoStd = photometry(parms, _iRef, _eRef, _gRef);
        _bandpho.push_back(parms);
      }
      else { // Appropriate photometric parameters not found
        ostringstream mess;
        mess << "Band " << i + 1 << " with wavelength Center = " << center[i]
             << " does not have PhotometricModel Algorithm group/profile";
        iException &e = iException::Message(iException::User, mess.str(), _FILEINFO_);
        errs += e.Errors() + "\n";
        e.Clear();
      }
    }

    // Check for errors and throw them all at the same time
    if(!errs.empty()) {
      errs += " --> Errors in the input PVL file \"" + pvl.Filename() + "\"";
      throw iException::Message(iException::User, errs, _FILEINFO_);
    }

    return;
  }

} // namespace Isis


