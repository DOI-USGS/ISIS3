#if !defined(Commentor_h)
#define Commentor_h
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
 */
template <class K>
  class Commentor {
    public:
      /** Constructor w/initialization  */
      Commentor() { init(); }
      /** Constructor w/caller provided file containing comments */
      Commentor(const std::string &comfile) {
        init();
        if ( !comfile.empty() ) {
          loadCommentFile(comfile);
        }
      }

      /** Destructor  */
      virtual ~Commentor() { }
       
      /** Returns full size of current comments internalized  */
      int size() const { return (_comComment.size() + _comSetComments.size()); }

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
         _comSetComments.append(source.getComment());
         return;
      }

      /** Loads general comments from a given file */
      void loadCommentFile(const std::string &comFile) {
        _comComment = readCommentFile(comFile);
        _comFile = comFile;
        return;
      }

      /** Allows user to set comment header string */
      void setCommentHeader(const std::string &comment) {
        _comComment = comment;
        return;
      }

      /** Returns the current contents of the collected comments */
      std::string Comments() const { 
        return (_comComment + _comSetComments);
      }

      /** Clear out all comments collected for starting over */
      void Clear() {
        _comSetComments.clear();
        return;
      }

    private:
      std::string _comFile;
      std::string _comComment;
      std::string _comSetComments;

      /**
       * @brief Read comments from a file
       *  
       * This method reads a text file containing comments that will be written 
       * to the SPICE kernel written. 
       *                          
       * @return std::string Returns contents of of comment file
       */
      std::string readCommentFile(const std::string comfile) {
        TextFile t(comfile);
        std::string comment;
        std::string cline;
        while ( t.GetLine(cline, false) ) {
          cline.push_back('\n');
          comment += cline;
        }
        return (comment);
      }

      /** Initialize setting all content to empty strings */
      void init() {
        _comFile = _comComment = _comSetComments = "";
      }
  };

};     // namespace Isis
#endif


