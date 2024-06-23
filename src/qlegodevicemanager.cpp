#include "qlegodevicemanager.h"

#include "qlegodevicescanner.h"
#include "qlegomotor.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(deviceManagerLogger, "lego.deviceManager");

QLegoDeviceManager::QLegoDeviceManager(QObject* parent)
    : QObject { parent }
    , m_scanner(nullptr)
{
    m_scanner = new QLegoDeviceScanner();

    QObject::connect(m_scanner, &QLegoDeviceScanner::finished, this, &QLegoDeviceManager::createDevices);

    QObject::connect(m_scanner, &QLegoDeviceScanner::errorMessage, this, [this](const QString& msg) {
        qWarning() << msg;
    });
}

void QLegoDeviceManager::scan()
{
    if (m_scanner) {
        if (!m_scanner->scanning()) {
            m_scanner->scan();
        }
    }
}

void QLegoDeviceManager::disconnectAll()
{
    foreach (auto d, m_devices) {
        if (d) {
            d->disconnect();
        }
    }
}

void QLegoDeviceManager::connectDevice(int index)
{
    if (m_devices.size() > index) {
        m_devices.at(index)->connectToDevice();
    }
}

QLegoDevice* QLegoDeviceManager::device(int index)
{
    if (m_devices.size() > index) {
        return m_devices.at(index);
    }
    return nullptr;
}

void QLegoDeviceManager::createDevices(QList<QBluetoothDeviceInfo> legoDevicesInfo)
{
    foreach (auto info, legoDevicesInfo) {
        QLegoDevice* device = QLegoDevice::createDevice(info);
        if (device) {

            m_devices.append(device);
            QObject::connect(device, &QLegoDevice::ready, this, [this]() {
                QLegoDevice* d = dynamic_cast<QLegoDevice*>(sender());
                qCDebug(deviceManagerLogger) << "Device " << d->name() << " is ready:"
                                             << "\n"
                                             << "Addr: " << d->address() << "\n"
                                             << "Type: " << d->deviceType() << "\n"
                                             << "Firmware: " << d->firmware();
                emit scanFinished();
            });

            QObject::connect(device, &QLegoDevice::disconnected, this, [this]() {
                QLegoDevice* d = dynamic_cast<QLegoDevice*>(sender());
                if (d) {
                    d->deleteLater();
                    m_devices.removeOne(d);
                }
            });
        }
    }
    qCDebug(deviceManagerLogger) << "Created " << m_devices.size() << " devices";
    //  if only has one device then auto connect to it.
    if (m_devices.size() == 1) {
        connectDevice();
    }
}
