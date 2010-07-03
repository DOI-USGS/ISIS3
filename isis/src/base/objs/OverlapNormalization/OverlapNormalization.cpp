/**
 * @file
 * $Revision: 1.3 $
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

#include "OverlapNormalization.h"
#include "iException.h"
#include <iomanip>

using namespace std;
namespace Isis {      
 
 /**
  * Constructs an OverlapNormalization object.  Compares and 
  * stores the vector, and initializes the basis and least 
  * squares functions. 
  *
  * @param statsList The list of Statistics objects corresponding 
  *                  to specific data sets (e.g., cubes)
  */
  OverlapNormalization::OverlapNormalization (std::vector<Statistics> statsList) {
    p_gainFunction = NULL;
    p_gainLsq = NULL;
    p_offsetFunction = NULL;
    p_offsetLsq = NULL;

    p_statsList = statsList;      
    p_gainFunction = new BasisFunction ("BasisFunction",
                                        statsList.size(),statsList.size());
    p_gainLsq = new LeastSquares (*p_gainFunction);
    p_offsetFunction = new BasisFunction ("BasisFunction",
                                          statsList.size(),statsList.size());  
    p_offsetLsq = new LeastSquares (*p_offsetFunction); 
    
    p_gains.resize(statsList.size());
    p_offsets.resize(statsList.size());
    for (unsigned int i=0; i<statsList.size(); i++) {
      p_gains[i] = 1.0;
      p_offsets[i] = 0.0;      
    }
    p_solved = false;
  } 
  
 /**
  * Destroys the OverlapNormalization object, frees up
  * pointers 
  */
  OverlapNormalization::~OverlapNormalization() {
    if (p_gainFunction != NULL) delete p_gainFunction;
    if (p_offsetFunction != NULL) delete p_offsetFunction;
    if (p_gainLsq != NULL) delete p_gainLsq;
    if (p_offsetLsq != NULL) delete p_offsetLsq;        
  };  

 /**
  * Attempts to add the given overlap data to a collection of 
  * valid overlaps, and returns the success or failure of that 
  * attempt. 
  *
  * @param area1 The statistics for the overlap area of the first
  *              overlapping data set
  * @param index1 The index in the list of Statistics of the 
  *            first data set
  * @param area2 The statistics for the overlap area of the 
  *              second data set
  * @param index2 The index in the list of Statistics of the 
  *            second overlapping data set
  * @param weight Relative significance of this overlap.  Default 
  *               value = 1.0
  *  
  * @return AddStatus An enumeration representing either a 
  *         successful add, or the reason for failure
  *  
  * @throws Isis::iException::Programmer - Identifying index 1 
  *             must exist in the statsList
  * @throws Isis::iException::Programmer - Identifying index 2 
  *             must exist in the statsList
  * @throws Isis::iException::Programmer - Weights must be all 
  *             positive real numbers
  */
  OverlapNormalization::AddStatus OverlapNormalization::AddOverlap (
                                const Statistics &area1, const unsigned index1,
                                const Statistics &area2, const unsigned index2,
                                double weight) {
    if (index1 >= p_statsList.size()) {
      string msg = "The index 1 is outside the bounds of the list.";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }
    if (index2 >=  p_statsList.size()) {
      string msg = "The index 2 is outside the bounds of the list.";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }

    // If there is no overlapping area, then the overlap is invalid
    if (area1.ValidPixels() == 0 || area2.ValidPixels() == 0) {
      return NoOverlap;    
    }

    // The weight must be a positive real number 
    if (weight <= 0.0) {
      string msg = "All weights must be positive real numbers.";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    } 

    OverlapNormalization::Overlap o;
    o.area1 = area1;
    o.area2 = area2;
    o.index1 = index1;
    o.index2 = index2;

    double avg1 = area1.Average();
    double avg2 = area2.Average();

    // Averages must not be 0 to avoid messing up least squares
    if (avg1 == 0 || avg2 == 0) return NoContrast;

    p_overlapList.push_back(o);    
    p_deltas.push_back(avg2 - avg1);
    p_weights.push_back(weight); 
    p_solved = false;
    return Success;
  }

 /**
  * Attempts to solve the least squares equation for all data 
  * sets, and returns the success or failure of that attempt. 
  *
  * @param type The enumeration clarifying whether the offset, 
  *             gain, or both should be solved here
  *  
  * @return bool Is the least squares equation now solved 
  *  
  * @throws Isis::iException::User - Number of overlaps and
  *             holds must be greater than the number of data
  *             sets
  */
  void OverlapNormalization::Solve(SolutionType type) {
    // Make sure that there is at least one overlap
    if (p_overlapList.size() == 0) {
      std::string msg = "None of the input images overlap";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // Make sure the number of valid overlaps + hold images is greater than the
    // number of input images (otherwise the least squares equation will be
    // unsolvable due to having more unknowns than knowns)
    if (p_overlapList.size() + p_idHoldList.size() < p_statsList.size()) {
      std::string msg = "The number of overlaps and holds must be greater than";
      msg += " the number of input images";
      throw iException::Message(iException::User,msg,_FILEINFO_);
    }

    // Calculate offsets
    if (type != Gains) {
      // Add knowns to least squares for each overlap
      for (int overlap=0; overlap<(int)p_overlapList.size(); overlap++) {
        Overlap curOverlap = p_overlapList[overlap];
        int id1 = curOverlap.index1;
        int id2 = curOverlap.index2;

        vector<double> input;
        input.resize(p_statsList.size());
        for (int i=0; i<(int)input.size(); i++) input[i] = 0.0;
        input[id1] = 1.0;
        input[id2] = -1.0;
        
        p_offsetLsq->AddKnown(input,p_deltas[overlap],p_weights[overlap]);
      }

      // Add a known to the least squares for each hold image
      for (int h=0; h<(int)p_idHoldList.size(); h++) {
        int hold = p_idHoldList[h];

        vector<double> input;
        input.resize(p_statsList.size());
        for (int i=0; i<(int)input.size(); i++) input[i] = 0.0;
        input[hold] = 1.0;
        p_offsetLsq->AddKnown(input,0.0,1e30);
      }

      // Solve the least squares and get the offset coefficients to apply to the
      // images
      p_offsets.resize(p_statsList.size());
      p_offsetLsq->Solve(Isis::LeastSquares::QRD);
      for (int i=0; i<p_offsetFunction->Coefficients(); i++) {
        p_offsets[i] = p_offsetFunction->Coefficient(i);
      }
    }

    // Calculate Gains
    if (type != Offsets) {
      // Add knowns to least squares for each overlap
      for (int overlap=0; overlap<(int)p_overlapList.size(); overlap++) {
        Overlap curOverlap = p_overlapList[overlap];
        int id1 = curOverlap.index1;
        int id2 = curOverlap.index2;

        vector<double> input;
        input.resize(p_statsList.size());
        for (int i=0; i<(int)input.size(); i++) input[i] = 0.0;
        input[id1] = 1.0;
        input[id2] = -1.0; 

        double tanp;

        if (curOverlap.area1.StandardDeviation() == 0.0) {
          tanp = 0.0;    // Set gain to 1.0      
        }
        else {
          tanp = curOverlap.area2.StandardDeviation()
              / curOverlap.area1.StandardDeviation();
        }
        
        if (tanp > 0.0) {
          p_gainLsq->AddKnown(input,log(tanp),p_weights[overlap]);
        }
        else {
          p_gainLsq->AddKnown(input,0.0,1e30);   // Set gain to 1.0
        }
      }
  
      // Add a known to the least squares for each hold image
      for (int h=0; h<(int)p_idHoldList.size(); h++) {
        int hold = p_idHoldList[h];

        vector<double> input;
        input.resize(p_statsList.size());
        for (int i=0; i<(int)input.size(); i++) input[i] = 0.0;
        input[hold] = 1.0;
        p_gainLsq->AddKnown(input,0.0,1e30);
      }

      // Solve the least squares and get the gain coefficients to apply to the
      // images
      p_gains.resize(p_statsList.size());
      p_gainLsq->Solve(Isis::LeastSquares::QRD);
      for (int i=0; i<p_gainFunction->Coefficients(); i++) {
        p_gains[i] = exp(p_gainFunction->Coefficient(i));
      }
    }    

    p_solved = true;
  }

 /**
  * Returns the calculated average DN value for the given 
  * data set 
  *
  * @param index The index in the Statistics list corresponding 
  *              to the data set desired
  *  
  * @return double The average for the data 
  *  
  * @throws Isis::iException::Programmer - Identifying index must
  *             exist in the statsList
  */
  double OverlapNormalization::Average (const unsigned index) const {
    if (index >= p_statsList.size()) {
      string msg = "The index was out of bounds for the list of statistics.";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    } 

    return p_statsList[index].Average();
  }

 /**
  * Returns the calculated gain (multiplier) for the given 
  * data set 
  *  
  * @param index The index in the Statistics list corresponding 
  *              to the data set desired
  *
  * @return double The gain for the data 
  *  
  * @throws Isis::iException::Programmer - Identifying index must
  *             exist in the statsList  
  */
  double OverlapNormalization::Gain (const unsigned index) const {
    if (index >= p_statsList.size()) {
      string msg = "The index was out of bounds for the list of statistics.";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    } 

    return p_gains[index];
  }

 /**
  * Returns the calculated offset (base) for the given 
  * data set 
  *
  * @param index The index in the Statistics list corresponding 
  *              to the data set desired
  *  
  * @return double The offset for the data 
  *  
  * @throws Isis::iException::Programmer - Identifying index must
  *             exist in the statsList
  */
  double OverlapNormalization::Offset (const unsigned index) const {
    if (index >= p_statsList.size()) {
      string msg = "The index was out of bounds for the list of statistics.";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    } 

    return p_offsets[index];
  }

 /**
  * Returns a new DN from an old using the calculated gains and 
  * offsets of the data set the pixel belongs to 
  *
  * @param dn The value of the pixel prior to equalization 
  *  
  * @param index The index in the Statistics list corresponding 
  *              to the data set for the pixel
  *  
  * @return double The newly calculated DN value
  *
  * @throws Isis::iException::Programmer - Least Squares equation
  *             must be solved before returning the gain
  */
  double OverlapNormalization::Evaluate (double dn, unsigned index) const {
    if (!p_solved) {
      string msg = "The least squares equation has not been successfully ";
      msg += "solved yet.";
      throw iException::Message(iException::Programmer,msg,_FILEINFO_);
    }

    if (Isis::IsSpecial(dn)) return dn;
    return (dn - Average(index)) * Gain(index) + Average(index) + Offset(index);
  }
}
