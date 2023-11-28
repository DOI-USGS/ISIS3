#ifndef ImageDisplayProperties_H
#define ImageDisplayProperties_H
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2012/06/12 06:30:00 $
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

#include <QObject>
#include <QMetaType> // required since we're adding to QVariant
#include <QColor>    // required since QColor is in a slot

#include "DisplayProperties.h"

class QAction;
class QBitArray;
class QXmlStreamWriter;

namespace Isis {
  class Cube;
  class FileName;
  class Project;
  class Pvl;
  class PvlObject;
  class UniversalGroundMap;

  /**
   * @brief This is the GUI communication mechanism for cubes
   *
   * This class is the connector between various GUI interfaces for cubes.
   *   We use this to communicate shared properties that various widgets need
   *   to know/should react to in a generic way.
   *
   * This is how this class is supposed to "connect" widgets:
   *
   *  widgetA         widgetB           widgetC
   *     |               |                 |
   *     ------DisplayProperties -------
   *
   * When a user selects a cube in widgetA, widgetB and widgetC now have a
   *   chance to also select the same cube. This applies to all shared
   *   properties. Some of the properties are actions - such as zoomFit. This
   *   also allows a widget with no zooming (such as a list) to have an option
   *   to zoom (if any of the widgets support it*) and have that option work.
   *   There is no state associated with zoomFit - it's an action connected
   *   to a signal.
   *
   * The proper way to detect a cube going away is to connect to the
   *   destroyed signal (from the parent QObject). Once that is emitted you
   *   cannot call any methods on this object.
   *
   * @author 2011-05-05 Steven Lambright
   *
   * @internal
   *   @history 2011-05-11 Steven Lambright - Added accessors for data that is
   *                       complicated to get or expensive (i.e. Camera
   *                       statistics and the footprint).
   *   @history 2011-05-18 Steven Lambright - Fixed the second constructor
   *   @history 2012-04-13 Steven Lambright, Stuart Sides, Ken Edmundson, Tracie
   *                           Sucharski - Renamed CubeDisplayProperties to
   *                           DisplayProperties and reduced functionality to
   *                           just DisplayProperties.
   *   @history 2012-06-12 Ken Edmundson - Made DisplayProperties a base class,
   *                           derived ControlNetworkDisplayProperties and
   *                           ImageDisplayProperties from it.
   */
  class ImageDisplayProperties : public DisplayProperties {
      Q_OBJECT
    public:
      /**
       * This is a list of properties and actions that are possible.
       */
       enum Property {
         //! Null display property for bit-flag purposes
         None             = 0,
         //! The color of the cube, default randomized (QColor)
         Color            = 1,
         //! The selection state of this cube (bool)
         Selected         = 2,
         //! True if the cube should show DN values if possible (bool)
         ShowDNs          = 4,
         //! True if the cube should show a fill area if possible (bool)
         ShowFill         = 8,
         //! True if the cube should show its display name (bool)
         ShowLabel        = 16,
         //! True if the cube should be outlined (bool)
         ShowOutline      = 32,
         //! Data ignored. Tells if the cube supports the zoomFit action
         Zooming          = 64, // This is here for qmos' benefit. It is necessary.
         //! Data ignored. Tells if the cube supports the "move*" actions
         ZOrdering        = 128, // This is here for qmos' benefit. It is necessary.
         //! Every display property for footprint views, provided for convenience
         FootprintViewProperties = Color | Selected | ShowDNs | ShowFill | ShowLabel | ShowOutline |
                                   Zooming | ZOrdering
       };

      ImageDisplayProperties(QString displayName, QObject *parent = NULL);
      virtual ~ImageDisplayProperties();

      static QColor randomColor();

    signals:
      //! Z Order up one
      void moveUpOne();
      //! Z Order to top
      void moveToTop();

      //! Z Order down one
      void moveDownOne();
      //! Z Order to bottom
      void moveToBottom();

      //! Fit in window
      void zoomFit();

    public slots:
      void setColor(QColor newColor);
      void setShowDNs(bool);
      void setShowFill(bool);
      void setShowLabel(bool);
      void setShowOutline(bool);
      void setSelected(bool);

    private:
      ImageDisplayProperties(const ImageDisplayProperties &);
      ImageDisplayProperties &operator=(const ImageDisplayProperties &);

  };
}

Q_DECLARE_METATYPE(QList<Isis::ImageDisplayProperties *>);

#endif

