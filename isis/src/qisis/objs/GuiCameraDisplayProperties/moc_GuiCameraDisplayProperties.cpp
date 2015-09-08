/****************************************************************************
** Meta object code from reading C++ file 'GuiCameraDisplayProperties.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "GuiCameraDisplayProperties.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GuiCameraDisplayProperties.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Isis__GuiCameraDisplayProperties[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      34,   33,   33,   33, 0x05,
      79,   33,   33,   33, 0x05,

 // slots: signature, parameters, type, tag, flags
     111,  102,   33,   33, 0x0a,
     128,   33,   33,   33, 0x0a,
     147,   33,   33,   33, 0x0a,
     165,   33,   33,   33, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Isis__GuiCameraDisplayProperties[] = {
    "Isis::GuiCameraDisplayProperties\0\0"
    "propertyChanged(GuiCameraDisplayProperties*)\0"
    "supportAdded(Property)\0newColor\0"
    "setColor(QColor)\0setShowLabel(bool)\0"
    "setSelected(bool)\0toggleShowLabel()\0"
};

void Isis::GuiCameraDisplayProperties::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GuiCameraDisplayProperties *_t = static_cast<GuiCameraDisplayProperties *>(_o);
        switch (_id) {
        case 0: _t->propertyChanged((*reinterpret_cast< GuiCameraDisplayProperties*(*)>(_a[1]))); break;
        case 1: _t->supportAdded((*reinterpret_cast< Property(*)>(_a[1]))); break;
        case 2: _t->setColor((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 3: _t->setShowLabel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setSelected((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->toggleShowLabel(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Isis::GuiCameraDisplayProperties::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Isis::GuiCameraDisplayProperties::staticMetaObject = {
    { &DisplayProperties::staticMetaObject, qt_meta_stringdata_Isis__GuiCameraDisplayProperties,
      qt_meta_data_Isis__GuiCameraDisplayProperties, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Isis::GuiCameraDisplayProperties::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Isis::GuiCameraDisplayProperties::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Isis::GuiCameraDisplayProperties::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Isis__GuiCameraDisplayProperties))
        return static_cast<void*>(const_cast< GuiCameraDisplayProperties*>(this));
    return DisplayProperties::qt_metacast(_clname);
}

int Isis::GuiCameraDisplayProperties::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DisplayProperties::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void Isis::GuiCameraDisplayProperties::propertyChanged(GuiCameraDisplayProperties * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Isis::GuiCameraDisplayProperties::supportAdded(Property _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
