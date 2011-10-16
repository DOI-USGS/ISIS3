#ifndef MdiCubeViewport_h
#define MdiCubeViewport_h

/**
 * @file
 * $Date: 2010/06/30 03:42:28 $
 * $Revision: 1.2 $
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

// The only includes allowed in this file are the direct parents of this class!
#include "CubeViewport.h"
// There are absolutely no exceptions to this.



class QPaintEvent;
template < class T > class QList;

namespace Isis {
  class Cube;
  class Tool;
  class ViewportBuffer;

  /**
   * @brief Cube display widget for certain Isis MDI applications
   *
   * @ingroup Visualization Tools
   *
   * @author Eric Hyer - Moved MDI specific code here from CubeViewport
   *
   * @internal
   *
   * @see Workspace CubeViewport
   */
  class MdiCubeViewport : public CubeViewport {
      Q_OBJECT

    public:
      MdiCubeViewport(Cube *cube, Isis::CubeDataThread * cdt = 0,
                      QWidget *parent = 0);
      ~MdiCubeViewport();

      void forceAbstract() {}

      //! Is the viewport linked with other viewports
      bool isLinked() const {
        return p_linked;
      };

      void registerTool(Tool *tool);
      void paintEvent(QPaintEvent *e);
      void restretch(ViewportBuffer *buffer);


    signals:
      void linkChanging(bool);
      void requestRestretch(MdiCubeViewport *, int);


    public slots:
      void setLinked(bool b);
      void viewGray(int band);
      void viewRGB(int red, int green, int blue);


    private:
      bool p_linked;
      QList<Tool *> p_toolList;

  };
}

#endif
