#ifndef OverlapNormalization_h
#define OverlapNormalization_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2009/11/25 22:09:21 $                                                                 
 *                                                             
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.                                                           
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,              
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */

#include "Statistics.h"
#include "BasisFunction.h"
#include "LeastSquares.h"

namespace Isis {

/**
 * Calculate the bases and multipliers for normalizing 
 * overlapping "data sets" (e.g., cubes).  Specifically, this 
 * class is designed for normalizing data in equalization 
 * applications such as equalizer and hiequal. Given collections 
 * for statistical data sets, overlapping statistics between 
 * these sets, and at least one data set to be "held", this 
 * class will use a least squares solution to calculate gain and 
 * offset coefficients for every set of data in the collection. 
 *  
 * Once the multiplicative and additive corrections have been 
 * derived, this class can then be used to calculate new DN 
 * values for a known data set.  The actual equation to be used 
 * for calculating new DN values with the derived gains and 
 * offsets is: 
 *  
 * @f[ 
 * newDN = (oldDN - avg(i)) * MULT(i) + avg(i) + BASE(i) 
 * @f] 
 *  
 * where i is the index of a known data set from the statistics 
 * list. 
 *  
 * @author 2009-05-07 Travis Addair 
 *  
 * @internal
 *   @history 2009-06-05 Mackenzie Boyd - fixed unittest to work on all systems
 *   @history 2009-06-12 Travis Addair - changed public interface
 *            to use indices instead of file names and renamed
 *            from IntersectionStatistics
 *   @history 2009-06-15 Travis Addair - documented all
 *            variables/enums
 *   @history 2009-06-24 Travis Addair - changed gain and offset vectors to
 *            itialize to 1.0 and 0.0, respectively
 *   @history 2009-11-25 Travis Addair - held images are now
 *            weighted to ensure gain and offset of 1.0 and 0.0,
 *            respectively
 */

  class OverlapNormalization {
    public:

      OverlapNormalization (std::vector<Statistics> statsList);      

      virtual ~OverlapNormalization();      
  
     /**
      * The result of the attempt to add overlap data to the list of
      * valid overlaps, where Success is a successful add, NoOverlap
      * is a failure due to one or both Statistics objects containing
      * no data, and NoContrast is a failure due to one or both
      * averages being 0
      */
      enum AddStatus {

       /**
        * Overlap is valid and was added successfully
        */
        Success,        

       /**
        * Data sets do not overlap one another
        */
        NoOverlap,

       /**
        * One or both areas contain no valid average
        */
        NoContrast
      };

      AddStatus AddOverlap (const Statistics &area1, const unsigned index1,
                    const Statistics &area2, const unsigned index2,
                    double weight=1.0);
  
     /**
      * Sets the list of files to be held during the solving process.
      *
      * @param holdIndex The index of a set of data from the list of 
      *                  Statistics objects to be held
      */
      inline void AddHold (unsigned holdIndex) { p_idHoldList.push_back(holdIndex); };
  
     /**
      * Enumeration for whether user/programmer wants to calculate
      * new gains, offsets, or both when solving
      */
      enum SolutionType {

       /**
        * Calculate only the gains
        */
        Gains,

       /**
        * Calculate only the offsets
        */
        Offsets,

       /**
        * Calculate both gains and offsets
        */
        Both
      };
  
      void Solve (SolutionType type=Both);
  
      double Average (const unsigned index) const;
      double Gain (const unsigned index) const;
      double Offset (const unsigned index) const; 

      double Evaluate (double dn, unsigned index) const;
  
    private:

     /**
      * Vector of Statistics objects for each data set
      */
      std::vector<Statistics> p_statsList;

     /**
      * Vector of indices corresponding to the p_statsList vector
      * representing data sets to be held in solution
      */
      std::vector<int> p_idHoldList;
  
     /**
      * Store statistics pertaining to the overlapping areas and 
      * indices (corresponding to the statistics list) for two data 
      * sets 
      */
      class Overlap {
        public:

         /**
          * Overlapping area for the first data set
          */
          Statistics area1;

         /**
          * Overlapping area for the second data set
          */
          Statistics area2;

         /**
          * Index corresponding to p_statsList for the first
          * overlapping data set
          */
          int index1;

         /**
          * Index corresponding to p_statsList for the second
          * overlapping data set
          */
          int index2;
      };
  
     /**
      * Vector of valid overlaps collected
      */
      std::vector<Overlap> p_overlapList;

     /**
      * Vector of delta values (differences between the averages of
      * two overlapping data sets) for every valid overlap
      */
      std::vector<double> p_deltas;

     /**
      * Vector of weights for every valid overlap
      */
      std::vector<double> p_weights;
  

     /**
      * Whether or not the least squares solution has been solved
      */
      bool p_solved;

     /**
      * Whether the user of this class wants to solve for the 
      * offsets, the gains, or both 
      */
      SolutionType p_solutionType;
  
     /**
      * Vector of calculated gains filled by the Solve method
      */
      std::vector<double> p_gains;

     /**
      * Vector of calculated offsets filled by the Solve method
      */
      std::vector<double> p_offsets;
  

     /**
      * The gain function to be solved
      */
      Isis::BasisFunction *p_gainFunction;

     /**
      * The offset function to be solved
      */
      Isis::BasisFunction *p_offsetFunction;
  
     /**
      * The least squares object that solves for the new gains
      */
      Isis::LeastSquares *p_gainLsq;

     /**
      * The least squares object that calculates offsets
      */
      Isis::LeastSquares *p_offsetLsq;

      //! Cannot copy this object
      OverlapNormalization &operator=(const OverlapNormalization &);

      //! Cannot copy this object
      OverlapNormalization (const OverlapNormalization &);
  };
};

#endif
