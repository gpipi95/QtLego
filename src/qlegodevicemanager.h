#ifndef QLEGODEVICEMANAGER_H
#define QLEGODEVICEMANAGER_H

#include <QObject>

#include <QBluetoothDeviceInfo>

QT_FORWARD_DECLARE_CLASS(QLegoDevice)
QT_FORWARD_DECLARE_CLASS(QLegoDeviceScanner)

class QLegoDeviceManager : public QObject {
    Q_OBJECT
public:
    explicit QLegoDeviceManager(QObject* parent = nullptr);

    void scan();
    void disconnectAll();
    void connectDevice(int index = 0);
    QLegoDevice* device(int index = 0);

private slots:
    void createDevices(QList<QBluetoothDeviceInfo> legoDevicesInfo);

signals:
private:
    QList<QLegoDevice*> m_devices;
    QLegoDeviceScanner* m_scanner;
};

#endif // QLEGODEVICEMANAGER_H
