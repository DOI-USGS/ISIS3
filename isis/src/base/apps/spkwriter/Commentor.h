#ifndef Commentor_h
#define Commentor_h
/**                                                                       
 * @file                                                                  
 * $Revision: 5850 $ 
 * $Date: 2014-05-27 15:22:24 -0700 (Tue, 27 May 2014) $
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
 *   $Id: Commentor.h 5850 2014-05-27 22:22:24Z jwbacker@GS.DOI.NET $
 */                                                                       
#include <string>
#include "TextFile.h"

namespace Isis {

/**
 * @brief Generic container for Kernel comments 
 *  
 * This class provides an accumulator for the comments that are generated for a 
 * SPICE kernel.  It accepts the comments from a file if provided by the caller 
 * or, otherwise they are generated using the default comment for a given SPICE 
 * kernel type.  Additionally, an operator()(K &source) segment method is 
 * provided to collect the comments generated for each segment in a list.  This 
 * is typically the list that is to be written to an eventual SPICE kernel file.
 *  
 * @author 2011-04-01 Kris Becker 
 * @internal 
 * @history 2011-04-11 Kris Becker Completed documentation 
 * @history 2013-12-19 Kris Becker Simplified by removing user comment file 
 *                                 handling.  See SpkKernelWriter. 
 */
template <class K>
  class Commentor {
    public:
      /** Constructor w/initialization  */
      Commentor() { init(); }

      /** Destructor  */
      virtual ~Commentor() { }
       
      /** Returns full size of current comments internalized  */
      int size() const { return (m_comComment.size() + m_comSetComments.size()); }

      /**
       * @brief operator() method to collect comments from each segment
       * 
       * This operator method is provided for use in the SpiceKernel class usage
       * whereby each segment is called and the comments are retrieved from each
       * segment when called.
       * 
       * @param K Segment where specific comments are retreived
       */
      void operator()(const K &source)  {
         m_comSetComments.append(source.getComment());
         return;
      }

      /** Allows user to set comment header string */
      void setCommentHeader(const QString &comment) {
        m_comComment = comment;
        return;
      }

      /** Returns the current contents of the collected comments */
      QString comments() const { 
        return (m_comComment + m_comSetComments);
      }

      /** Clear out all comments collected for starting over */
      void Clear() {
        m_comSetComments.clear();
        return;
      }

    private:
      QString m_comComment;
      QString m_comSetComments;

      /** Initialize setting all content to empty strings */
      void init() {
        m_comComment = m_comSetComments = "";
      }
  };

};     // namespace Isis
#endif


