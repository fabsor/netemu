/****************************************************************************
** Meta object code from reading C++ file 'masterserversettings.h'
**
** Created: Wed May 5 20:44:25 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SettingsWidgets/masterserversettings.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'masterserversettings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MasterServerSettings[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MasterServerSettings[] = {
    "MasterServerSettings\0\0"
    "OnMasterServerAddressChanged()\0"
};

const QMetaObject MasterServerSettings::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MasterServerSettings,
      qt_meta_data_MasterServerSettings, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MasterServerSettings::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MasterServerSettings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MasterServerSettings::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MasterServerSettings))
        return static_cast<void*>(const_cast< MasterServerSettings*>(this));
    return QWidget::qt_metacast(_clname);
}

int MasterServerSettings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnMasterServerAddressChanged(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
