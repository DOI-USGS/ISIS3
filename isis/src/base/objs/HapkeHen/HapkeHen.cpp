#include <cmath>
#include "Constants.h"
#include "HapkeHen.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "iException.h"
#include "iString.h"

using namespace std;

namespace Isis {

  HapkeHen::HapkeHen (Pvl &pvl) : HapkePhotoModel(pvl) {
    p_photoHg1 = 0.0;
    p_photoHg2 = 0.0;

    PvlGroup &algorithm = pvl.FindObject("PhotometricModel").FindGroup("Algorithm",Pvl::Traverse);

    if (algorithm.HasKeyword("Hg1")) {
      SetPhotoHg1(algorithm["Hg1"]);
    }

    if (algorithm.HasKeyword("Hg2")) {
      SetPhotoHg2(algorithm["Hg2"]);
    }
  }

  /*
   * Computes normal albedo mult factor w/o opp surge from            
   * Hapke input parameters: W,H,BO,HG,THETA.                         
   * Full-up Hapke's Law with macroscopic roughness.  The photometric 
   * function multiplied back in will be modified to take out oppos-  
   * ition effect.  This requires saving the actual value of B0 while 
   * temporarily setting it to zero in the common block to compute    
   * the overall normalization. 
   *  
   * @param phase      Value of phase angle, in degrees.
   * @param incidence  Value of incidence angle, in degrees.
   * @param emission   Value of emission angle, in degrees. 
   * @returns <b>double</b>
   *                                                                  
   *                                                                  
   * @history 1989-08-02 Unknown author in Isis2 under name pht_hapke           
   * @history 1991-08-07 Tammy Becker  relinked hapke to new photompr           
   * @history 1997-02-16 James M Anderson - changed nonstandard degree trig     
   *                     to use radians                                         
   * @history 1999-01-11 KTT - Remove mu,munot,and alpha from the argument      
   *                     list and pass in only ema,inc, and phase.  Remove      
   *                     lat and lon from argument list because they aren't     
   *                     used.                                                  
   * @history 1999-03-01 K Teal Thompson  Implement 1999-01-08 Randy Kirk 
   *                     Original Specs & Code.  Declare vars, add implicit none.
   * @history 1999-11-18 Randy Kirk - fixed minor typos, implemented return with
   *                     smooth Hapke (Theta=0) result before doing rough Hapke 
   *                     calculations, allow single-particle-phase params = 0
   * @history 2008-01-14 Janet Barrett - Imported into Isis3. Changed name from pht_hapke to PhotoModelAlgorithm() 
   * @history 2008-11-05 Jeannie Walldren - Added documentation 
   *          from Isis2 files. Replaced Isis::PI with PI since this is in Isis namespace.
   * 
   */
  double HapkeHen::PhotoModelAlgorithm (double phase, double incidence,
        double emission) {
    double pht_hapkehen;
    double pharad;  //phase angle in radians
    double incrad;  // incidence angle in radians
    double emarad; // emission angle in radians
    double munot;
    double mu;
    double cost;
    double sint;
    double tan2t;
    double gamma;
    double hgs;
    double sing;
    double cosg;
    double tang;
    double bg;
    double pg;
    double pg1;
    double pg2;
    double sini;
    double coti;
    double cot2i;
    double ecoti;
    double ecot2i;
    double u0p0;
    double sine;
    double cote;
    double cot2e;
    double cosei;
    double sinei;
    double caz;
    double az;
    double az2;
    double faz;
    double tanaz2;
    double sin2a2;
    double api;
    double ecote;
    double ecot2e;
    double up0;
    double q;
    double ecei;
    double s2ei;
    double u0p;
    double up;
    double ecee;
    double s2ee;
    double rr1;
    double rr2;

    pharad = phase * PI / 180.0;
    incrad = incidence * PI / 180.0;
    emarad = emission * PI / 180.0;
    munot = cos(incrad);
    mu = cos(emarad);

    if (p_photoTheta != p_photoThetaold) {
      cost = cos(p_photoTheta * PI / 180.0);
      sint = sin(p_photoTheta * PI / 180.0);
      p_photoCott = cost / max(1.0e-10, sint);
      p_photoCot2t = p_photoCott * p_photoCott;
      p_photoTant = sint / cost;
      tan2t = p_photoTant * p_photoTant;
      p_photoSr = sqrt(1.0 + PI * tan2t);
      p_photoOsr = 1.0 / p_photoSr;
      SetOldTheta(p_photoTheta);
    }

    if (incidence >= 90.0) {
      pht_hapkehen = 0.0;
      return pht_hapkehen;
    }

    gamma = sqrt(1.0 - p_photoWh);
    hgs = p_photoHg1 * p_photoHg1;

    sing = sin(pharad);
    cosg = cos(pharad);
    tang = sing / max(1.0e-10, cosg);

    if (p_photoHh == 0.0) {
      bg = 0.0;
    } else {
      if (phase <= 90.0) {
        bg = p_photoB0 / max(-5.0, 1.0 + tang / p_photoHh);
      } else {
        bg = 0.0;
      }
    }

    pg1 = (1.0 - p_photoHg2) * (1.0 - hgs) / pow((1.0 + hgs + 2.0 * 
        p_photoHg1 * cosg), 1.5); 
    pg2 = p_photoHg2 * (1.0 - hgs) / pow((1.0 + hgs - 2.0 * 
        p_photoHg1 * cosg), 1.5);
    pg = pg1 + pg2;

    if (p_photoTheta <= 0.0) {
      pht_hapkehen = p_photoWh / 4.0 * munot / (munot + mu) * ((1.0 + bg) *
          pg - 1.0 + Hfunc(munot,gamma) * Hfunc(mu,gamma));
      return pht_hapkehen;
    }

    sini = sin(incrad);
    coti = munot / max(1.0e-10, sini);
    cot2i = coti * coti;
    ecoti = exp(min(-p_photoCot2t * cot2i / PI , 23.0));
    ecot2i = exp(min(-2.0 * p_photoCott * coti / PI, 23.0));
    u0p0 = p_photoOsr * (munot + sini * p_photoTant * ecoti / (2.0 - ecot2i));

    sine = sin(emarad);
    cote = mu / max(1.0e-10, sine);
    cot2e = cote * cote;

    cosei = mu * munot;
    sinei = sine * sini;

    if (sinei == 0.0) {
      caz = 1.0;
      az = 0.0;
    } else {
      caz = (cosg - cosei) / sinei;
      if (caz <= -1.0) {
        az = 180.0;
      } else if (caz > 1.0) {
        az = 0.0;
      } else {
        az = acos(caz) * 180.0 / PI;
      }
    }

    az2 = az / 2.0;
    if (az2 >= 90.0) {
      faz = 0.0;
    } else {
      tanaz2 = tan(az2 * PI / 180.0);
      faz = exp(min(-2.0 * tanaz2, 23.0));
    }

    sin2a2 = pow(sin(az2 * PI / 180.0), 2.0);
    api = az / 180.0;

    ecote = exp(min(-p_photoCot2t * cot2e / PI, 23.0));
    ecot2e = exp(min(-2.0 * p_photoCott * cote / PI, 23.0));
    up0 = p_photoOsr * (mu + sine * p_photoTant * ecote / (2.0 - ecot2e));

    if (incidence <= emission) {
      q = p_photoOsr * munot / u0p0;
    } else {
      q = p_photoOsr * mu / up0;
    }

    if (incidence <= emission) {
      ecei = (2.0 - ecot2e - api * ecot2i);
      s2ei = sin2a2 * ecoti;
      u0p = p_photoOsr * (munot + sini * p_photoTant * (caz * ecote + s2ei) / ecei);
      up = p_photoOsr * (mu + sine * p_photoTant * (ecote - s2ei) / ecei);
    } else {
      ecee = (2.0 - ecot2i - api * ecot2e);
      s2ee = sin2a2 * ecote;
      u0p = p_photoOsr * (munot + sini * p_photoTant * (ecoti - s2ee) / ecee);
      up = p_photoOsr * (mu + sine * p_photoTant * (caz * ecoti + s2ee) / ecee);
    }

    rr1 = p_photoWh / 4.0 * u0p / (u0p + up) * ((1.0 + bg) * pg -
        1.0 + Hfunc(u0p, gamma) * Hfunc(up, gamma));
    rr2 = up * munot / (up0 * u0p0 * p_photoSr * (1.0 - faz + faz * q));
    pht_hapkehen = rr1 * rr2;

    return pht_hapkehen;
  }

 /**
   * Set the Hapke Henyey Greenstein coefficient for the single
   * particle phase function. This is one of two coefficients
   * needed for the single particle phase function. This parameter
   * is limited to values that are >-1 and <1.
   *
   * @param hg1  Hapke Henyey Greenstein coefficient, default is 0.0
   */
  void HapkeHen::SetPhotoHg1 (const double hg1) {
    if (hg1 <= -1.0 || hg1 >= 1.0) {
      string msg = "Invalid value of Hapke Henyey Greenstein hg1 [" +
          iString(hg1) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    p_photoHg1 = hg1;
  }

 /**
   * Set the Hapke Henyey Greenstein coefficient for the single
   * particle phase function. This is one of two coefficients
   * needed for the single particle phase function. This parameter
   * is limited to values that are >=0 and <=1.
   *
   * @param hg2  Hapke Henyey Greenstein coefficient, default is 0.0
   */
  void HapkeHen::SetPhotoHg2 (const double hg2) {
    if (hg2 < 0.0 || hg2 > 1.0) {
      string msg = "Invalid value of Hapke Henyey Greenstein hg2 [" +
          iString(hg2) + "]";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }
    p_photoHg2 = hg2;
  }
}

extern "C" Isis::PhotoModel *HapkeHenPlugin (Isis::Pvl &pvl) {
  return new Isis::HapkeHen(pvl);
}
