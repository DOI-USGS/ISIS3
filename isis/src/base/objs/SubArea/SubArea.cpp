/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/10/15 22:21:19 $
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

#include <float.h>
#include <iostream>
#include <string>
#include <sstream>

#include "SubArea.h"
#include "AlphaCube.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "iException.h"
#include "iString.h"

using namespace std;
namespace Isis {

 /**
  * Defines the subarea.
  *
  * @param orignl This is the number of lines in the original image.
  *
  * @param origns This is the number of samples in the original image.
  *
  * @param sl This is the line in the original image where the subarea will
  *           start at. This value must be greater than or equal to 1 and 
  *           less than or equal to the number of lines in the original 
  *           image.
  *
  * @param ss This is the sample in the original image where the subarea
  *           will start at. This value must be greater than or equal to 1
  *           and less than or equal to the number of samples in the 
  *           original image.
  *
  * @param el This is the ending line of the subarea to extract from the 
  *           original image. This value must be greater than or equal to
  *           sl and less than or equal to the number of lines in the
  *           original image. The actual number of lines that will be in
  *           the subarea will be (el-sl+1)/linc.
  *
  * @param es This is the ending sample of the subarea to extract from the 
  *           original image. This value must be greater than or equal to
  *           ss and less than or equal to the number of samples in the
  *           original image. The actual number of samples that will be in
  *           the subarea will be (es-ss+1)/sinc.
  *
  * @param linc This is the line increment that will be used to extract the
  *           subarea from the original image. It must be greater than 0.
  *
  * @param sinc This is the sample increment that will be used to extract
  *           the subarea from the original image. It must be greater than
  *           0.
  *
  */
  void SubArea::SetSubArea (const int orignl, const int origns,
                    const int sl, const int ss, const int el,
                    const int es, const double linc, const double sinc) {

    // Save size of original image file
    p_nl = orignl;
    p_ns = origns;

    // Save the subarea information
    p_sl = sl;
    p_ss = ss;
    p_el = el; 
    p_es = es;

    if (p_sl > p_el) {
      string msg = "Invalid start/end line range [sl,el] specified for subarea";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    if (p_ss > p_es) {
      string msg = "Invalid start/end sample range [ss,es] specified for subarea";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    p_linc = linc;
    if (p_linc <= 0.0) {
      string msg = "Invalid line increment [linc] specified for subarea";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    p_sinc = sinc;
    if (p_sinc <= 0.0) {
      string msg = "Invalid sample increment [sinc] specified for subarea";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  }

 /**
  * Modifies a label for a file containing a subarea. The AlphaCube, Mapping,
  * and Instrument groups are all affected when a subarea is extracted from
  * another file. If the linc is not equal to the sinc, then the Instrument
  * and Mapping groups will be removed from the label because they will no
  * longer be valid. If the linc is equal to the sinc and they are not equal
  * to 1, then the map scale and resolution in the Mapping group needs to be
  * updated. The latitude and longitude ranges become invalid when the subarea
  * does not cover the entire sample and line range of the original cube.
  * Update the upper left corner x,y values if the projection is still valid
  * and the starting line and/or starting sample have been changed from their
  * location in the original file.
  *
  * @param icube This is the input cube that will have the subarea
  *           extracted from it. The label of this cube will be used to
  *           create updated Mapping, Instrument, and AlphaCube groups
  *           for the label of the output cube containing the subarea.
  *
  * @param ocube This is the output cube file containing the subarea. The
  *           label of this cube will be modified by extracting the Mapping,
  *           Instrument, and AlphaCube groups from the input label and
  *           putting them in this label.
  *
  * @param results This is the Results group that will go into the application
  *           log file. This group must be created by the calling application.
  *           Information will be added to it if the Mapping or Instrument
  *           groups are deleted from the output image label.
  *
  */
  void SubArea::UpdateLabel(Cube *icube, Cube *ocube, PvlGroup &results) {

    Pvl inlabel = *icube->Label(); 

    // If the linc and sinc are not equal, then the Instrument and
    // Mapping groups are no longer valid.
    if (p_linc != p_sinc) {
      if (inlabel.FindObject("IsisCube").HasGroup("Instrument")) {
        inlabel.FindObject("IsisCube").DeleteGroup("Instrument");
        results += PvlKeyword ("InstrumentGroupDeleted", "True");
      }
      if (inlabel.FindObject("IsisCube").HasGroup("Mapping")) {
        inlabel.FindObject("IsisCube").DeleteGroup("Mapping");
        results += PvlKeyword ("MappingGroupDeleted", "True");
      }
    }

    if (inlabel.FindObject("IsisCube").HasGroup("Mapping")) {
      // Update the upper left corner X,Y values if the starting line or
      // starting sample are changed. 
      if (p_sl != 1 || p_ss != 1) {
        Projection &proj = *icube->Projection();
        proj.SetWorld(p_ss-0.5,p_sl-0.5);
        PvlGroup &mapgroup = inlabel.FindObject("IsisCube").FindGroup("Mapping", Pvl::Traverse);
        mapgroup.AddKeyword(PvlKeyword("UpperLeftCornerX",proj.XCoord()),
                            Pvl::Replace);
        mapgroup.AddKeyword(PvlKeyword("UpperLeftCornerY",proj.YCoord()),
                           Pvl::Replace);
      }

      // If the linc and sinc are not equal to 1, then update the
      // mapping scale and resolution.
      if (p_linc == p_sinc && p_linc != 1.0) {
        PvlGroup &mapgroup = inlabel.FindObject("IsisCube").FindGroup("Mapping", Pvl::Traverse);
        double pixres = mapgroup["PixelResolution"];
        mapgroup["PixelResolution"] = pixres * p_linc;
        double scale = mapgroup["Scale"];
        mapgroup["Scale"] = scale / p_linc;
      }

      // If the outer bounds of the image are changed, then the 
      // latitude,longitude range is no longer valid.
      if (p_sl != 1 || p_ss != 1 || p_el != p_nl || p_es != p_ns) {
        PvlGroup &mapgroup = inlabel.FindObject("IsisCube").FindGroup("Mapping", Pvl::Traverse);
        if (mapgroup.HasKeyword("MinimumLatitude")) {
          mapgroup.DeleteKeyword("MinimumLatitude");
        }
        if (mapgroup.HasKeyword("MaximumLatitude")) {
          mapgroup.DeleteKeyword("MaximumLatitude");
        }
        if (mapgroup.HasKeyword("MinimumLongitude")) {
          mapgroup.DeleteKeyword("MinimumLongitude");
        }
        if (mapgroup.HasKeyword("MaximumLongitude")) {
          mapgroup.DeleteKeyword("MaximumLongitude");
        }
      }
    }

    // Make changes to the output cube label
    if (ocube->HasGroup("Instrument")) {
      ocube->DeleteGroup("Instrument");
    }
    if (inlabel.FindObject("IsisCube").HasGroup("Instrument")) {
      PvlGroup inst;
      inst = inlabel.FindObject("IsisCube").FindGroup("Instrument");
      ocube->PutGroup(inst);
    }

    if (ocube->HasGroup("Mapping")) {
      ocube->DeleteGroup("Mapping");
    }
    if (inlabel.FindObject("IsisCube").HasGroup("Mapping")) {
      PvlGroup mapgrp;
      mapgrp = inlabel.FindObject("IsisCube").FindGroup("Mapping");
      ocube->PutGroup(mapgrp);
    }
   
    // Update the AlphaCube group - this group will only be updated if
    // a Mapping group does not exist in the labels. 
    AlphaCube aCube(p_ns,p_nl,ocube->Samples(),ocube->Lines(),
                    p_ss-0.5,p_sl-0.5,p_es+0.5,p_el+0.5);
    aCube.UpdateGroup(*ocube->Label());
  }
}
