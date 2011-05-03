#if !defined(SpiceKernel_h)
#define SpiceKernel_h
/**                                                                       
 * @file                                                                  
 * $Revision$ 
 * $Date$
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
 *  
 *   $Id$
 */                                                                       

#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>

#include "iString.h"
#include "iException.h"
#include "SpiceSegment.h"

namespace Isis {

/**
 * @brief Container for SPICE kernel creation
 * 
 * This class serves as a container for ISIS cube files to prep for writing the
 * contents to a NAIF SPICE kernel.  Each file added is a CK or SPK segment 
 * depending upon the type specified in the K template parameter. When the ISIS 
 * cube SPICE segment is added, the contents of the Table BLOB 
 * (InstrumentRotation for CKs, InstrumentPosition for SPKs) have been read and 
 * transformed to the appropriate state intended to be compatible with kernels 
 * issued by each mission source. 
 * 
 * It is designed for ease of use.  Here is an example to create the most basic
 * of CK kernel from a single ISIS file:
 * @code
 *   SpiceKernel<SpkSegment> kernel;
 *   kernel.add("mycube.cub");
 * @endcode
 * 
 * Note that processing ISIS cubes is expensive in terms of NAIF kernel
 * management.  Lots of NAIF kernel activity is incurred in resolving all the
 * necessary requirements to get the SPICE data in a form that satisfies NAIF
 * kernel specifications.
 *  
 * To get access to the segments a Visitor design pattern is used whereby the 
 * generic Visitor class need only write an paren operator that accepts a 
 * segment of the K type.  This class interates through all the segments calling 
 * the operator(K &segment) method for each one in the container. 
 *  
 * Note that when a new K Segment is added, the list is immediately sorted based 
 * on time.  The sort is a stable sort, meaning if the segments are added in a 
 * time chronologically increasing time, the original list order is preserved. 
 *  
 * @ingroup Utility
 * 
 * @author 2010-11-22 Kris Becker
 * @internal
 * @history 2010-12-09 Kris Becker Add documentation and example 
 * @history 2011-04-02 Kris Becker Modified to a template container class and 
 *          updated documentation
 */
template <typename K>
  class SpiceKernel {
    public:
      typedef K SegmentType;
      typedef std::vector<SegmentType> SegmentList;
      typedef typename SegmentList::iterator       SegmentIter;
      typedef typename SegmentList::const_iterator ConstSegmentIter;
  
      /** Constructor  */
      SpiceKernel() { }
      /** Destructor  */
      virtual ~SpiceKernel() { }
  
      /** Returns the number of segments */
      int size() const { return (_segments.size()); }

      /**
       * Const iterator into list
       *
       * @return ConstSegmentIter Returns a const iterator to the list
       */
      ConstSegmentIter begin() const {
        return _segments.begin();
      }

      /**
       * Const iterator to end of list
       *
       * @return ConstSegmentIter  Returns the const end of the list
       */
      ConstSegmentIter end() const {
        return _segments.end();
      }

      /**
       * Returns the start of the list for iterating purposes
       *
       * @return SegmentIter Returns an iterator on the collection
       */
      SegmentIter begin() {
        return _segments.begin();
      }

      /**
       * Returns the end of the list
       *
       * @return SegmentIter Returns the end of the list for determining the end
       *         of the iteration loop
       */
      SegmentIter end() {
        return _segments.end();
      }

      /**
       * @brief Add a new segment to the kernel
       * 
       * This method accepts a new segment and inserts it in the list - actually
       * appends it and then resorts the list so they are ordered by time. 
       *  
       * @param segment New segment to add to list
       */
      void add(const K &segment) {
        _segments.push_back(segment);
        std::stable_sort(_segments.begin(), _segments.end());
      }

      /**
       * @brief Template method Visitor implementation
       * 
       * This method accepts a Visitor that defines an operator() of the form
       * Visitor::operator()(K &segment).  For every segment in the list,
       * the operator will be called.  The caller is assured the list is sorted
       * via the operator<(const K &segment) of the segment type when they were 
       * added. 
       *  
       * @param Visitor  Visitor class that accepts each segment in the list
       */
      template <typename Visitor>
        void Accept(Visitor &v) const {
           ConstSegmentIter k(begin());
           while (k != end()) {
             v(*k);
             k++;
           }
          return;  
        }

      /**
       * @brief Const template methoid visitor implementation
       * 
       * This is a const Visitor method implementation with same functionality
       * as the non-const version.
       * 
       * @param Visitor  Visitor class that accepts each segment in the list
       */
      template <typename Visitor>
        void Accept(const Visitor &v) const {
           ConstSegmentIter k(begin());
           while (k != end()) {
             v(*k);
             k++;
           }
          return;  
        }


    private:
      SegmentList  _segments;  ///< List of arbitrary segments
  };




};     // namespace Isis
#endif

