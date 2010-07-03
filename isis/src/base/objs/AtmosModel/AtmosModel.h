#ifndef AtmosModel_h
#define AtmosModel_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.10 $                                                             
 * $Date: 2008/11/07 23:48:13 $                                                                 
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
 */
  class AtmosModel {
    public:
      AtmosModel (Pvl &pvl, PhotoModel &pmodel);
      //! Empty destructor
      virtual ~AtmosModel() {};

      // These methods were moved here from the NumericalMethods class
      static double G11Prime(double tau); 
      static double Ei(double x) throw (iException &);
      static double En(unsigned int n, double x) throw (iException &);
      // Calculate atmospheric scattering effect
      void CalcAtmEffect(double pha, double inc, double ema, double *pstd, 
                         double *trans, double *trans0, double *sbar);
      // Used to calculate atmosphere at standard conditions
      virtual void SetStandardConditions(bool standard);
      // Obtain hemispheric and bihemispheric albedo by integrating the photometric function
      void GenerateAhTable();
      // Perform integration for Hapke Henyey-Greenstein atmosphere correction
      void GenerateHahgTables();
      // Set parameters needed for atmospheric correction
      void SetAtmosAtmSwitch (const int atmswitch);
      void SetAtmosBha       (const double bha   );
      void SetAtmosBharef    (const double bharef);
      void SetAtmosHga       (const double hga   );
      void SetAtmosHgaref    (const double hgaref);
      void SetAtmosInc       (const double inc   );
      void SetAtmosNulneg    (const string nulneg);
      void SetAtmosPhi       (const double phi   );
      void SetAtmosTau       (const double tau   );
      void SetAtmosTauref    (const double tauref);
      void SetAtmosWha       (const double wha   );
      void SetAtmosWharef    (const double wharef);
      
      //! Return atmospheric algorithm name
      string AlgorithmName () const { return p_atmosAlgorithmName; };

      //! Return atmospheric Bha value
      double AtmosBha () const { return p_atmosBha; };
      //! Return atmospheric Tau value
      double AtmosTau () const { return p_atmosTau; };
      //! Return atmospheric Wha value
      double AtmosWha () const { return p_atmosWha; };
      //! Return atmospheric Hga value
      double AtmosHga () const { return p_atmosHga; };
      //! Return atmospheric Bharef value
      double AtmosBharef () const { return p_atmosBharef; };
      //! Return atmospheric Hgaref value
      double AtmosHgaref () const { return p_atmosHgaref; };
      //! Return atmospheric Tauref value
      double AtmosTauref () const { return p_atmosTauref; };
      //! Return atmospheric Wharef value
      double AtmosWharef () const { return p_atmosWharef; };
      //! Return atmospheric Nulneg value
      bool AtmosNulneg () const { return p_atmosNulneg; };
      //! Return atmospheric Ab value
      double AtmosAb () const { return p_atmosAb; };
      //! Return atmospheric Hahgsb value
      double AtmosHahgsb () const { return p_atmosHahgsb; };
      //! Return atmospheric Ninc value
      int AtmosNinc () const { return p_atmosNinc; };

      //! Return atmospheric IncTable value
      vector <double> AtmosIncTable () { return p_atmosIncTable; };
      //! Return atmospheric AhTable value
      vector <double> AtmosAhTable () { return p_atmosAhTable; };
      //! Return atmospheric HahgtTable value
      vector <double> AtmosHahgtTable () { return p_atmosHahgtTable; };
      //! Return atmospheric Hahgt0Table value
      vector <double> AtmosHahgt0Table () { return p_atmosHahgt0Table; };

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
      NumericalApproximation AtmosAhSpline() {return p_atmosAhSpline;};
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
      NumericalApproximation AtmosHahgtSpline() {return p_atmosHahgtSpline;};
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
      NumericalApproximation AtmosHahgt0Spline() {return p_atmosHahgt0Spline;};
          
    protected:
      virtual void AtmosModelAlgorithm (double phase, double incidence, double emission) = 0;
      
      void SetAlgorithmName(string name) { p_atmosAlgorithmName = name; }
      void SetAtmosNulneg(bool nulneg) { p_atmosNulneg = nulneg; }
      void SetOldTau(double tau) { p_atmosTauold = tau; }
      void SetOldWha(double wha) { p_atmosWhaold = wha; }

      PhotoModel *GetPhotoModel() const { return p_atmosPM; }
      bool StandardConditions() const { return p_standardConditions; }
      bool TauOrWhaChanged() const;
      double Eulgam() const { return p_atmosEulgam; }

      int p_atmosAtmSwitch;
      int p_atmosNinc;

      double p_atmosBha;
      double p_atmosBharef;
      double p_atmosBhasave;
      double p_atmosHgaref;
      double p_atmosHgasave;
      double p_atmosTauref;
      double p_atmosTausave;
      double p_atmosWharef;
      double p_atmosWhasave;

      double p_pstd;   //!Pure atmospheric-scattering term.
      double p_trans;  //!Transmission of surface reflected light through the atmosphere overall.
      double p_trans0; //!Transmission of surface reflected light through the atmosphere with no scatterings in the atmosphere.
      double p_sbar;   //!Illumination of the ground by the sky.
      double p_atmosHga;
      double p_atmosTau;
      double p_atmosWha;
      double p_atmosAb;
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
