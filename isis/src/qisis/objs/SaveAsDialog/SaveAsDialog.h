#ifndef SaveAsDialog_h
#define SaveAsDialog_h

/**
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
 
#include <QCheckBox>
#include <QRadioButton>
#include "FileDialog.h"

namespace Isis {

  /**
  * @brief Widget to save(Save As) Isis cubes(used in qview) to display the 
  * FileDialog to select the output cube. This dialog additionally displays 
  * radio buttons for choices FullImage, ExportAsIs, ExportFullRes. These choices 
  * are located at the bottom of the dialog. 
  *  
  * FullImage     - copies the entire image into the user specified output file 
  * ExportAsIs    - copies the image as displayed in the qview app window 
  * ExportFullRes - copies the image as displayed in the qview app window but with 
  *                 full resolution 
  *
  * @ingroup Visualization Tools
  *
  * @author Sharmila Prasad
  *
  * @internal
  * @history 2011-05-11  Sharmila Prasad - Initial Version
  */
  class SaveAsDialog : public FileDialog {
      Q_OBJECT
    public:
      SaveAsDialog(QString pTitle, QStringList &pFilterList, QDir &pDir, QWidget *pParent = 0);
      enum saveAsType {FullImage, ExportAsIs, ExportFullRes};
      int getSaveAsType();
      
    public slots:
      void setFullImage(bool);
      void setAsIs(bool);
      void setFullResolution(bool);

    private:
      QDir &p_dir;//!<The directory to open the dialog with
      QRadioButton *p_fullImage;     //!< FullImage Button
      QRadioButton *p_exportAsIs;    //!< ExportAsIs Button
      QRadioButton *p_exportFullRes; //!< ExportFullRes Button
      saveAsType p_saveAsType;       //!< Current Save Type
  };
};

#endif
