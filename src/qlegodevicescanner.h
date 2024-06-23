#ifndef QLEGODEVICESCANNER_H
#define QLEGODEVICESCANNER_H

#include "qlegodevice.h"
#include "qlegoglobal.h"

#include <QBluetoothDeviceInfo>
#include <QList>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

QT_FORWARD_DECLARE_CLASS(QString)

class Q_LEGO_EXPORT QLegoDeviceScanner : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningUpdated)
    Q_PROPERTY(int devicesFound READ devicesFound NOTIFY devicesFoundUpdated)
public:
    explicit QLegoDeviceScanner(QObject* parent = nullptr);
    ~QLegoDeviceScanner();

    bool scanning() const;
    int devicesFound() const;

    Q_INVOKABLE void scan();
    Q_INVOKABLE void stopScan();
    bool isLegoHub(const QBluetoothDeviceInfo& info);
    bool isLegoHub(const QString& name);

private Q_SLOTS:
    void addDevice(const QBluetoothDeviceInfo& info);
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void deviceScanFinished();

Q_SIGNALS:
    void errorMessage(const QString& msg);
    void finished(QList<QBluetoothDeviceInfo> legoDevicesInfo);
    void scanningUpdated();
    void devicesFoundUpdated();

private:
    bool m_scanning;
    QBluetoothDeviceDiscoveryAgent* m_agent;
    QStringList m_addresses;
    QList<QBluetoothDeviceInfo> m_legoDevicesInfo;
};

QT_END_NAMESPACE

#endif
