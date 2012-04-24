/****************************************************************************
** Meta object code from reading C++ file 'ProfileDialog.h'
**
** Created: Wed Feb 29 11:22:29 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "ProfileDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ProfileDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ProfileDialog[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      29,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      41,   14,   14,   14, 0x08,
      63,   14,   14,   14, 0x08,
      83,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ProfileDialog[] = {
    "ProfileDialog\0\0createStart()\0createEnd()\0"
    "createStartSelected()\0createEndSelected()\0"
    "help()\0"
};

const QMetaObject ProfileDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ProfileDialog,
      qt_meta_data_ProfileDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ProfileDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ProfileDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ProfileDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ProfileDialog))
        return static_cast<void*>(const_cast< ProfileDialog*>(this));
    if (!strcmp(_clname, "Ui::ProfileDialog"))
        return static_cast< Ui::ProfileDialog*>(const_cast< ProfileDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ProfileDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: createStart(); break;
        case 1: createEnd(); break;
        case 2: createStartSelected(); break;
        case 3: createEndSelected(); break;
        case 4: help(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ProfileDialog::createStart()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ProfileDialog::createEnd()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
