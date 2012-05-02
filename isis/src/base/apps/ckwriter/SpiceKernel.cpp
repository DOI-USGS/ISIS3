/**
 * @file
 * $Revision$
 * $Date$
 * $Id$
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
#include <numeric>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>

#include "SpiceKernel.h"
#include "CkKernelWriter.h"
#include "FileName.h"
#include "Pvl.h"
#include "Cube.h"
#include "iString.h"
#include "TextFile.h"
#include "Application.h"
#include "NaifStatus.h"
#include "IException.h"

#include "naif/SpiceUsr.h"

using namespace std;

namespace Isis {

 SpiceKernel::SpiceKernel() {
   init();
 }

void SpiceKernel::add(const std::string &cfile) {
 Cube cube;
 cube.open(cfile);
 add(cube);
 return;
}

void SpiceKernel::add(Cube &cube) {
  _segments.push_back(SpiceSegment(cube));
  return;
}
const SpiceSegment &SpiceKernel::operator[](const int i) const {
 return (_segments[i]);
}


bool CheckSegment(const SpiceSegment *s1, const SpiceSegment *s2) {
  return (s1->startTime() < s2->startTime());
}

std::string SpiceKernel::getSummary(const std::string &commfile) const {
  vector<const SpiceSegment *> seglist;
  for ( int i = 0 ; i < size() ; i++) {
    seglist.push_back(&_segments[i]);
  }

  // Sorts the Segment pointers
  stable_sort(seglist.begin(), seglist.end(), CheckSegment);

  string comment = getCkComment(commfile);

  // Collect comments from each segment
  for ( unsigned int i = 0 ; i < seglist.size() ; i++ ) {
    comment += seglist[i]->getComment();
  }
  return (comment);
}

 void SpiceKernel::write(const std::string &kname, const std::string &comfile,
                         const int cktype) const {
   vector<const SpiceSegment *> seglist;
   int comChars(0);
   for ( int i = 0 ; i < size() ; i++) {
     seglist.push_back(&_segments[i]);
     comChars += _segments[i].getComment().size();
   }

   stable_sort(seglist.begin(), seglist.end(), CheckSegment);

   string comment = getCkComment(comfile);
   comChars += comment.size();

   //  Create the output file.
   try {
     NaifStatus::CheckErrors();
     CkKernelWriter ckwriter(kname, comChars+512, cktype);
     ckwriter.addComment(comment);
     NaifStatus::CheckErrors();

   // Write sorted segments
     for ( unsigned int i = 0 ; i < seglist.size() ; i++ ) {
       try {
         ckwriter.write(*seglist[i]);
         comment = seglist[i]->getComment();
         ckwriter.addComment(comment);
         NaifStatus::CheckErrors();
       } catch ( IException &ie ) {
         ostringstream mess;
         mess << "Failed to write segment, ID = " << seglist[i]->Id();
         throw IException(ie, IException::Programmer, mess.str(), _FILEINFO_);
       }
     }
   }
   catch ( IException &ie ) {
     ostringstream mess;
     mess << "Could not create output CK kernel file: " << kname;
     throw IException(ie, IException::User, mess.str(), _FILEINFO_);
   }
   return;
 }

 void SpiceKernel::init() {
   _segments.clear();
   return;
 }

 std::string SpiceKernel::getCkComment(const std::string &comFile) const {
   ostringstream comment;
   if ( !comFile.empty() ) {
     TextFile txt(comFile);
     string cline;
     while ( txt.GetLineNoFilter(cline )) {
       comment << cline << "\n";
     }
   }
   else {
     comment << "\
****************************************************************************\n\
  USGS ISIS (ckwriter) Generated CK Kernel\n\
  Created By:   " << Application::UserName() << "\n\
  Date Created: " << Application::DateTime() << "\n\
****************************************************************************\n\
\n\
Orientation Data in the File\n\
-----------------------------------------------------------------------\n\
\n\
      This file contains orientation and potentially derived angular\n\
      rates (where possible/specified).\n\
\n\
\n\
Status\n\
-----------------------------------------------------------------------\n\
\n\
      This kernel was generated for the purpose of storing C-Smithed\n\
      pointing updates generated through ISIS processing techniques\n\
      (control nets, jitter analysis, etc...).  These CK kernels\n\
      are intended to mimick CKs provided by individual mission\n\
      (NAV teams).\n\
\n\
Pedigree\n\
-----------------------------------------------------------------------\n\
\n\
      This file was generated by an automated process.  The ISIS\n\
      application ckwriter was used to read CK kernel data\n\
      contained within an ISIS cube file.  It then writes it as an\n\
      individual segment in the CK.  Hence, a list of files can be\n\
      written to a single CK kernel.  However, mixing the instruments\n\
      contained in a single CK kernel is discouraged.\n\
\n\
      Individual segments coming from files will have a single record\n\
      written for the center of the exposure (time) for framing \n\
      instruments or a record/image line for line scan instruments.\n\
\n\
      Creating type 3 CK kernels must contain at least 3 records for\n\
      framing instruments to avoid roundoff error for the center of the\n\
      exposure time of an image.  Framing instruments should pad time\n\
      using the spiceinit application options.\n\
\n\
\n\
Angular Rates\n\
-----------------------------------------------------------------------\n\
\n\
      This kernel may or may not contain angular velocity vectors. Efforts\n\
      are made to preserve and provide angular velocities where they\n\
      originally existed.\n\
\n\
\n\
Usage Note\n\
-----------------------------------------------------------------------\n\
\n\
      To make use of this file in a typical SPICE based application,\n\
      you must supply a leapseconds kernel, a mission spacecraft clock\n\
      kernel, and the instrument/spacecraft frame kernel.  These files\n\
      provide the supporting ancillary data to properly query this\n\
      C-kernel for attitude content.  They should be the same kernels that\n\
      were originally used to initialize the image.\n\
\n\
      Segments in this file are actually individual ISIS files where the\n\
      internally cached SPICE data is extracted and transformed into the\n\
      appropriate content to satisfy NAIF\'s SPICE kernel storage\n\
      requirements.  The contents of this kernel are summarized below.\n\
\n\
Segment (by file) Summary\n\
-----------------------------------------------------------------------\n\
\n\
      The follow sections describe each segment in this CK kernel.  Each\n\
      segment is a file in the input list.  When running ISIS spiceinit,\n\
      the kernels listed for each file should be supplied to ensure proper\n\
      geometry can be reproduced accurately.\n\
\n";
   }

   return (string(comment.str()));
}

};  // namespace Isis

