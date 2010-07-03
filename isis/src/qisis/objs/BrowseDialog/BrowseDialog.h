#ifndef BrowseDialog_h
#define BrowseDialog_h

#include "FileDialog.h"

namespace Qisis {
   /**
  * @brief Class for browsing cubes.
  *
  * @ingroup Visualization Tools
  *
  * @author Stacy Alley
  *
  * @internal
  *  
   * @history 2008-01-18 Stacy Alley - Changed the constructor to
   *          accept a QStringList which serves as the default filters
   *          for the file dialog boxes.
   * 
   * @history 2008-01-28 Stacy Alley - Changed the constructor
   *          again to accept a QDir which is the default directory
   *          the file dialog box should point to. When the user
   *          changes directories, the dir variable which is passed
   *          by reference set to that new directory.
  */
 

  class BrowseDialog : public FileDialog {
    Q_OBJECT
    public:
      BrowseDialog (QString title, QStringList &filterList, QDir &dir, QWidget *parent=0);
      
    public slots:
      void displayCube();
      void done(int r);

    private:
      QDir &p_dir;//!<The directory to open the dialog with.

  };
};

#endif
