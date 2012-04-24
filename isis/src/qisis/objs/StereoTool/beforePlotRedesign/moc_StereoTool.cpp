/****************************************************************************
** Meta object code from reading C++ file 'StereoTool.h'
**
** Created: Wed Feb 29 11:22:30 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "StereoTool.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'StereoTool.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Isis__StereoTool[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      32,   17,   17,   17, 0x05,
      53,   51,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     113,   97,   17,   17, 0x0a,
     156,  148,   17,   17, 0x2a,
     189,  183,   17,   17, 0x0a,
     216,  183,   17,   17, 0x0a,
     243,   17,   17,   17, 0x09,
     264,   17,   17,   17, 0x09,
     279,   17,   17,   17, 0x08,
     290,   17,   17,   17, 0x08,
     310,   17,   17,   17, 0x08,
     331,   17,   17,   17, 0x08,
     341,   17,   17,   17, 0x08,
     358,   17,   17,   17, 0x08,
     384,  377,   17,   17, 0x08,
     408,   17,   17,   17, 0x08,
     431,   17,   17,   17, 0x08,
     446,   17,   17,   17, 0x08,
     464,   17,   17,   17, 0x08,
     483,   17,   17,   17, 0x08,
     498,   17,   17,   17, 0x08,
     517,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Isis__StereoTool[] = {
    "Isis::StereoTool\0\0tieToolSave()\0"
    "editPointChanged()\0,\0"
    "stretchChipViewport(Stretch*,CubeViewport*)\0"
    "lat,lon,pointId\0createPoint(double,double,QString)\0"
    "lat,lon\0createPoint(double,double)\0"
    "point\0modifyPoint(ControlPoint*)\0"
    "deletePoint(ControlPoint*)\0"
    "rubberBandComplete()\0activateTool()\0"
    "showHelp()\0paintAllViewports()\0"
    "calculateElevation()\0profile()\0"
    "saveElevations()\0saveAsElevations()\0"
    "radius\0userBaseRadius(QString)\0"
    "updateRadiusLineEdit()\0measureSaved()\0"
    "setTemplateFile()\0viewTemplateFile()\0"
    "clearProfile()\0createStartPoint()\0"
    "createEndPoint()\0"
};

const QMetaObject Isis::StereoTool::staticMetaObject = {
    { &Tool::staticMetaObject, qt_meta_stringdata_Isis__StereoTool,
      qt_meta_data_Isis__StereoTool, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Isis::StereoTool::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Isis::StereoTool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Isis::StereoTool::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Isis__StereoTool))
        return static_cast<void*>(const_cast< StereoTool*>(this));
    return Tool::qt_metacast(_clname);
}

int Isis::StereoTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Tool::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: tieToolSave(); break;
        case 1: editPointChanged(); break;
        case 2: stretchChipViewport((*reinterpret_cast< Stretch*(*)>(_a[1])),(*reinterpret_cast< CubeViewport*(*)>(_a[2]))); break;
        case 3: createPoint((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 4: createPoint((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 5: modifyPoint((*reinterpret_cast< ControlPoint*(*)>(_a[1]))); break;
        case 6: deletePoint((*reinterpret_cast< ControlPoint*(*)>(_a[1]))); break;
        case 7: rubberBandComplete(); break;
        case 8: activateTool(); break;
        case 9: showHelp(); break;
        case 10: paintAllViewports(); break;
        case 11: calculateElevation(); break;
        case 12: profile(); break;
        case 13: saveElevations(); break;
        case 14: saveAsElevations(); break;
        case 15: userBaseRadius((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 16: updateRadiusLineEdit(); break;
        case 17: measureSaved(); break;
        case 18: setTemplateFile(); break;
        case 19: viewTemplateFile(); break;
        case 20: clearProfile(); break;
        case 21: createStartPoint(); break;
        case 22: createEndPoint(); break;
        default: ;
        }
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void Isis::StereoTool::tieToolSave()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Isis::StereoTool::editPointChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Isis::StereoTool::stretchChipViewport(Stretch * _t1, CubeViewport * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
