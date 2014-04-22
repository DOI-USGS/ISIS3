/****************************************************************************
** Meta object code from reading C++ file 'JigsawSetupDialog.h'
**
** Created: Mon Apr 21 17:15:25 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "JigsawSetupDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'JigsawSetupDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Isis__JigsawSetupDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      33,   25,   24,   24, 0x08,
      73,   67,   24,   24, 0x08,
     120,   67,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Isis__JigsawSetupDialog[] = {
    "Isis::JigsawSetupDialog\0\0checked\0"
    "on_OutlierRejection_toggled(bool)\0"
    "index\0on_SpacecraftCombobox_currentIndexChanged(int)\0"
    "on_PointingCombobox_currentIndexChanged(int)\0"
};

void Isis::JigsawSetupDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        JigsawSetupDialog *_t = static_cast<JigsawSetupDialog *>(_o);
        switch (_id) {
        case 0: _t->on_OutlierRejection_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->on_SpacecraftCombobox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_PointingCombobox_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Isis::JigsawSetupDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Isis::JigsawSetupDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Isis__JigsawSetupDialog,
      qt_meta_data_Isis__JigsawSetupDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Isis::JigsawSetupDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Isis::JigsawSetupDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Isis::JigsawSetupDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Isis__JigsawSetupDialog))
        return static_cast<void*>(const_cast< JigsawSetupDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Isis::JigsawSetupDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
