/****************************************************************************
** Meta object code from reading C++ file 'JigsawDialog.h'
**
** Created: Mon Apr 21 17:30:35 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "JigsawDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'JigsawDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Isis__JigsawDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      51,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Isis__JigsawDialog[] = {
    "Isis::JigsawDialog\0\0on_JigsawSetupButton_pressed()\0"
    "on_JigsawRunButton_clicked()\0"
};

void Isis::JigsawDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        JigsawDialog *_t = static_cast<JigsawDialog *>(_o);
        switch (_id) {
        case 0: _t->on_JigsawSetupButton_pressed(); break;
        case 1: _t->on_JigsawRunButton_clicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData Isis::JigsawDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Isis::JigsawDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Isis__JigsawDialog,
      qt_meta_data_Isis__JigsawDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Isis::JigsawDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Isis::JigsawDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Isis::JigsawDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Isis__JigsawDialog))
        return static_cast<void*>(const_cast< JigsawDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Isis::JigsawDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
