/****************************************************************************
** Meta object code from reading C++ file 'maiaXmlRpcClient.h'
**
** Created: Mon 8. Apr 15:45:24 2019
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../libmaia-master_modified/maiaXmlRpcClient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'maiaXmlRpcClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MaiaXmlRpcClient[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      31,   18,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      74,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MaiaXmlRpcClient[] = {
    "MaiaXmlRpcClient\0\0reply,errors\0"
    "sslErrors(QNetworkReply*,QList<QSslError>)\0"
    "replyFinished(QNetworkReply*)\0"
};

const QMetaObject MaiaXmlRpcClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MaiaXmlRpcClient,
      qt_meta_data_MaiaXmlRpcClient, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MaiaXmlRpcClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MaiaXmlRpcClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MaiaXmlRpcClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MaiaXmlRpcClient))
        return static_cast<void*>(const_cast< MaiaXmlRpcClient*>(this));
    return QObject::qt_metacast(_clname);
}

int MaiaXmlRpcClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sslErrors((*reinterpret_cast< QNetworkReply*(*)>(_a[1])),(*reinterpret_cast< const QList<QSslError>(*)>(_a[2]))); break;
        case 1: replyFinished((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MaiaXmlRpcClient::sslErrors(QNetworkReply * _t1, const QList<QSslError> & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
