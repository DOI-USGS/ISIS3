#ifndef AtmosModel_h
#define AtmosModel_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <vector>
#include "PhotoModel.h"
#include "NumericalApproximation.h"
#include "NumericalAtmosApprox.h"

using namespace std;
namespace Isis {
  class Pvl;

  /**
   * @brief Isotropic atmos scattering model
   *
   * @ingroup RadiometricAndPhotometricCorrection
   * @author 1998-12-21 Randy Kirk
   *
   * @internal
   *  @history 2007-02-20 Janet Barrett - Imported from Isis2.
   *  @history 2007-07-31 Steven Lambright - Fixed unit test
   *  @history 2007-08-15 Steven Lambright - Refactored
   *  @history 2008-03-07 Janet Barrett - Added code to set standard
   *                      conditions. Also added bharef, hgaref, tauref,
   *                      and wharef variables and supporting methods.
   *  @history 2008-06-18 Christopher Austin - Fixed much documentation
   *  @history 2008-07-09 Steven Lambright - Fixed unit test
   *  @history 2008-11-05 Jeannie Walldren - Moved numerical
   *           methods and functions to NumericalMethods and
   *           NumericalAtmosApprox classes. Moved G11Prime(),
   *           Ei(), and En() from NumericalMethods into this
   *           class. Added splines to protected variables and
   *           removed second derivative vector protected
   *           variables (p_atmosAhTable2, p_atmosHahgtTable2,
   *           p_atmosHahgt0Table2) that are no longer needed.
   *           Replaced arrays with vectors in protected
   *           variables. Added documentation from Isis2 files.
   *           Removed "Isis::" and "std::" in AtmosModel.cpp
   *           since that file uses std and Isis namespaces.
   *  @history 2008-11-07 Jeannie Walldren - Fixed documentation
   *  @history 2011-08-17 Sharmila Prasad -Added API's for HNORM, Additive Offset
   *  @history 2011-09-14 Janet Barrett - Got rid of bharef, hgaref,
   *           and wharef variables and supporting methods.
   *  @history 2011-12-19 Janet Barrett - Added the p_atmosEstTau variable
   *           that determines if the optical depth "tau" will be estimated
   *           using shadow modeling. Added the GenerateHahgTablesShadow
   *           method for doing the shadow modeling. Added a getter method
   *           for accessing the Munot (normally the cosine of the incidence
   *           angle) value in the atmospheric classes. Added a setter method
   *           for setting the p_atmosEstTau variable which is used by the
   *           atmospheric classes.
   */
  class AtmosModel {
    public:
      AtmosModel(Pvl &pvl, PhotoModel &pmodel);
      //! Empty destructor
      virtual ~AtmosModel() {};

      // These methods were moved here from the NumericalMethods class
      static double G11Prime(double tau);
      static double Ei(double x);
      static double En(unsigned int n, double x);
      // Calculate atmospheric scattering effect
      void CalcAtmEffect(double pha, double inc, double ema, double *pstd,
                         double *trans, double *trans0, double *sbar, double *transs);
      // Used to calculate atmosphere at standard conditions
      virtual void SetStandardConditions(bool standard);
      // Obtain hemispheric and bihemispheric albedo by integrating the photometric function
      void GenerateAhTable();
      // Perform integration for Hapke Henyey-Greenstein atmosphere correction
      void GenerateHahgTables();
      // Perform integration for Hapke Henyey-Greenstein atmosphere correction. This
      // version is used for shadow modeling and does not tabulate the first and third
      // integrals like GenerateHahgTables. It only evaluates the middle integral
      // that corrects the sbar variable (which is the illumination of the ground
      // by the sky).
      void GenerateHahgTablesShadow();
      // Set parameters needed for atmospheric correction
      void SetAtmosAtmSwitch(const int atmswitch);
      void SetAtmosBha(const double bha);
      void SetAtmosHga(const double hga);
      void SetAtmosInc(const double inc);
      void SetAtmosNulneg(const string nulneg);
      void SetAtmosPhi(const double phi);
      void SetAtmosTau(const double tau);
      void SetAtmosTauref(const double tauref);
      void SetAtmosWha(const double wha);
      void SetAtmosHnorm(const double hnorm);
      void SetAtmosIord(const string offset);
      void SetAtmosEstTau(const string esttau);

      //! Return atmospheric algorithm name
      string AlgorithmName() const {
        return p_atmosAlgorithmName;
      };

      //! Allow additive offset in fit?
      bool AtmosAdditiveOffset() const {
        return p_atmosAddOffset;
      };

      //! Return atmospheric Hnorm value
      double AtmosHnorm() const {
        return p_atmosHnorm;
      };

      //! Return atmospheric Bha value
      double AtmosBha() const {
        return p_atmosBha;
      };
      //! Return atmospheric Tau value
      double AtmosTau() const {
        return p_atmosTau;
      };
      //! Return atmospheric Wha value
      double AtmosWha() const {
        return p_atmosWha;
      };
      //! Return atmospheric Hga value
      double AtmosHga() const {
        return p_atmosHga;
      };
      //! Return atmospheric Tauref value
      double AtmosTauref() const {
        return p_atmosTauref;
      };
      //! Return atmospheric Nulneg value
      bool AtmosNulneg() const {
        return p_atmosNulneg;
      };
      //! Return atmospheric Ab value
      double AtmosAb() const {
        return p_atmosAb;
      };
      //! Return atmospheric Hahgsb value
      double AtmosHahgsb() const {
        return p_atmosHahgsb;
      };
      //! Return atmospheric Ninc value
      int AtmosNinc() const {
        return p_atmosNinc;
      };
      //! Return atmospheric Munot value
      double AtmosMunot() const {
        return p_atmosMunot;
      };

      //! Return atmospheric IncTable value
      vector <double> AtmosIncTable() {
        return p_atmosIncTable;
      };
      //! Return atmospheric AhTable value
      vector <double> AtmosAhTable() {
        return p_atmosAhTable;
      };
      //! Return atmospheric HahgtTable value
      vector <double> AtmosHahgtTable() {
        return p_atmosHahgtTable;
      };
      //! Return atmospheric Hahgt0Table value
      vector <double> AtmosHahgt0Table() {
        return p_atmosHahgt0Table;
      };

      /**
       * If GenerateAhTable() has been called this returns a clamped
       * cubic spline of the data set (@a p_atmosIncTable,
       * @a p_atmosAhTable) with first derivatives of the endpoints
       * equal to 1.0e30. Otherwise, it is a natural cubic spline with
       * an empty data set.
       *
       * @returns @b NumericalApproximation Cubic spline
       * @internal
       *   @history 2008-11-05 Jeannie Walldren - Original version
       */
      NumericalApproximation AtmosAhSpline() {
        return p_atmosAhSpline;
      };
      /**
       * If GenerateHahgTables() has been called this returns a
       * clamped cubic spline of the data set (@a p_atmosIncTable,
       * @a p_atmosHahgtTable) with first derivatives of the endpoints
       * equal to 1.0e30. Otherwise, it is a natural cubic spline with
       * an empty data set.
       *
       * @returns @b NumericalApproximation Cubic spline
       * @internal
       *   @history 2008-11-05 Jeannie Walldren - Original version
       */
      NumericalApproximation AtmosHahgtSpline() {
        return p_atmosHahgtSpline;
      };
      /**
       * If GenerateHahgTables() has been called this returns a
       * clamped cubic spline of the data set (@a p_atmosIncTable,
       * @a p_atmosHahgt0Table) with first derivatives of the
       * endpoints equal to 1.0e30. Otherwise, it is a natural cubic
       * spline with an empty data set.
       *
       * @returns @b NumericalApproximation Cubic spline
       * @internal
       *   @history 2008-11-05 Jeannie Walldren - Original version
       */
      NumericalApproximation AtmosHahgt0Spline() {
        return p_atmosHahgt0Spline;
      };

    protected:
      virtual void AtmosModelAlgorithm(double phase, double incidence, double emission) = 0;

      void SetAlgorithmName(string name) {
        p_atmosAlgorithmName = name;
      }
      void SetAtmosNulneg(bool nulneg) {
        p_atmosNulneg = nulneg;
      }
      void SetAtmosIord(bool offset) {
        p_atmosAddOffset = offset;
      }
      void SetAtmosEstTau(bool esttau) {
        p_atmosEstTau = esttau;
      }
      void SetOldTau(double tau) {
        p_atmosTauold = tau;
      }
      void SetOldWha(double wha) {
        p_atmosWhaold = wha;
      }

      PhotoModel *GetPhotoModel() const {
        return p_atmosPM;
      }
      bool StandardConditions() const {
        return p_standardConditions;
      }
      bool TauOrWhaChanged() const;
      double Eulgam() const {
        return p_atmosEulgam;
      }

      int p_atmosAtmSwitch;
      int p_atmosNinc;

      double p_atmosBha;
      double p_atmosBhasave;
      double p_atmosHgasave;
      double p_atmosTauref;
      double p_atmosTausave;
      double p_atmosWhasave;

      double p_pstd;      //!< Pure atmospheric-scattering term.
      double p_trans;     //!< Transmission of surface reflected light through the atmosphere overall.
      double p_trans0;    //!< Transmission of surface reflected light through the atmosphere with no scatterings in the atmosphere.
      double p_transs;    //!< Transmission of light that must be subtracted from the flat surface model to get the shadow model.
      double p_sbar;      //!< Illumination of the ground by the sky.
      double p_atmosHga;
      double p_atmosTau;
      double p_atmosWha;
      double p_atmosAb;
      double p_atmosHnorm;     //!< Atmospheric shell thickness normalized to planet radius.
      bool   p_atmosAddOffset; //!< Allow additive offset in fit
      bool   p_atmosEstTau;    //!< Estimate optical depth tau using shadows
      vector <double> p_atmosIncTable;
      vector <double> p_atmosAhTable;
      double p_atmosHahgsb;
      vector <double> p_atmosHahgtTable;
      vector <double> p_atmosHahgt0Table;
      double p_atmosInc;
      double p_atmosPhi;
      double p_atmosMunot;
      double p_atmosSini;
      double p_atmosCosphi;
      double p_atmosEulgam;

      //! Spline object for the atmospheric Ah Table.  Properties are set in GenerateAhTable().
      NumericalApproximation p_atmosAhSpline;
      //! Spline object for the atmospheric Hahg Table.  Properties are set in GenerateHahgTables().
      NumericalApproximation p_atmosHahgtSpline;
      //! Spline object for the atmospheric Hahg0 Table.  Properties are set in GenerateHahgTables().
      NumericalApproximation p_atmosHahgt0Spline;

    private:
      bool p_standardConditions;

      string p_atmosAlgorithmName;

      PhotoModel *p_atmosPM;

      bool p_atmosNulneg;

      double p_atmosTauold;
      double p_atmosWhaold;
      friend class NumericalAtmosApprox;
  };
};

#endif
