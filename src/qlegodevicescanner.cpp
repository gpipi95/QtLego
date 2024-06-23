#include "qlegodevicescanner.h"
#include "qlegocommon.h"
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtCore/QLoggingCategory>
#include <QtCore/QMetaEnum>
#include <QtCore/QString>

Q_LOGGING_CATEGORY(scannerLogger, "lego.scanner");

// TODO: Need to verify full names before can use.
static QStringList deviceNames({ "LEGO Move Hub", "Technic" });

/*!
  \class QLegoDeviceScanner
  \brief The QLegoDeviceScanner class scans for QLegoDevices.
  \inmodule QtLego
  \ingroup devices

  Users will use QLegoDeviceScanner to scan for and detect QLegoDevice instances, rather than
  create new device instances themselves.

  QLegoDeviceScanner currently only scans using Bluetooth LE, but future versions
  may support scanning by Bluetooth Classic or serial ports.

  \section1 Scanning and Connecting

  When a new device has been detected, the \l{QLegoDeviceScanner::deviceFound()} signal will
  be emitted to notify that a new device has been connected. This is the primary way for
  users to access QLegoDevice instances, instead of creating it themselves.

  \section1 Example

  The following example scans for new devices, then prints the address and firmware version
  of each device detected before waiting 10 seconds before disconnecting.

  \code
  auto scanner = new QLegoDeviceScanner();

  QObject::connect(scanner, &QLegoDeviceScanner::deviceFound, [=](QLegoDevice *device) {
      qDebug() << "  Address:" << device->address();
      qDebug() << "  Firmware:" << device->firmware();
      QTimer::singleShot(10000, device, &QLegoDevice::disconnect);
  });
  \endcode
*/

/*!
    \fn void QLegoDevice::errorMessage(const QString &msg)

    This signal is emitted when an error has occurred.
*/

/*!
    \fn void QLegoDevice::deviceFound(QLegoDevice *device);

    This signal is emitted when a new device has been detected.
*/

/*!
    \fn void QLegoDevice::finished();

    This signal is emitted when the scanner has finished searching for devices.
*/

/*!
    Constructs a QLegoDeviceScanner object.
*/
QLegoDeviceScanner::QLegoDeviceScanner(QObject* parent)
    : QObject(parent)
    , m_agent(new QBluetoothDeviceDiscoveryAgent)
    , m_scanning(false)
{
    m_agent->setLowEnergyDiscoveryTimeout(5000);

    // clang-format off
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &QLegoDeviceScanner::addDevice);
    connect(m_agent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(&QBluetoothDeviceDiscoveryAgent::error), this, &QLegoDeviceScanner::deviceScanError);
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::finished, this, &QLegoDeviceScanner::deviceScanFinished);
    connect(m_agent, &QBluetoothDeviceDiscoveryAgent::canceled, this, [this](){ m_scanning = false; });
    // clang-format on
}

QLegoDeviceScanner::~QLegoDeviceScanner()
{
    delete m_agent;
}

/*!
    \property QLegoDeviceScanner::scanning
    \brief returns whether a scan is currently in progress.
*/
bool QLegoDeviceScanner::scanning() const
{
    return m_scanning;
}

/*!
    Begin scanning for devices.
*/
void QLegoDeviceScanner::scan()
{
    m_scanning = true;
    m_agent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    emit scanningUpdated();
}

void QLegoDeviceScanner::stopScan()
{
    if (m_scanning || m_agent->isActive()) {
        m_agent->stop();
        m_scanning = false;
        emit scanningUpdated();
    }
}

bool QLegoDeviceScanner::isLegoHub(const QBluetoothDeviceInfo& info)
{
    return isLegoHub(info.name());
}
bool QLegoDeviceScanner::isLegoHub(const QString& name)
{
    // lego boost ok, others not tested
    foreach (auto n, deviceNames) {
        if (name.contains(n, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

void QLegoDeviceScanner::addDevice(const QBluetoothDeviceInfo& info)
{
    if (isLegoHub(info)) {
        qCDebug(scannerLogger) << "found" << info.name();
        const auto address = getAddress(info);
        if (m_addresses.contains(address)) {
            qDebug() << "Device" << address << "already added";
            return;
        } else {
            m_addresses << address;
        }

#if 0
        // Deactivate. KEEP! Will use when finished with service scan.
        const auto ids = info.manufacturerIds();
        bool found = false;
        for (auto id : ids) {
            // Not 100% sure about this; others used manufacturerData[3]
            const auto data = info.manufacturerData(id);
            const auto chars = data.constData();
            const unsigned char ch = chars[1];
            switch (ch) {
                case ManufacturerData::MoveHub:

                case ManufacturerData::TechnicHub:

            }
        }
#endif
    }
}

void QLegoDeviceScanner::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError) {
        emit errorMessage("The Bluetooth adaptor is powered off.");
    } else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError) {
        emit errorMessage("Writing or reading from the device resulted in an error.");
    } else {
        const auto index = m_agent->metaObject()->indexOfEnumerator("Error");
        static QMetaEnum qme = m_agent->metaObject()->enumerator(index);
        emit errorMessage("Error: " + QLatin1String(qme.valueToKey(error)));
    }
}

void QLegoDeviceScanner::deviceScanFinished()
{
    m_legoDevicesInfo.clear();
    m_scanning = false;
    emit scanningUpdated();
    foreach (auto d, m_agent->discoveredDevices()) {
        if (isLegoHub(d)) {
            m_legoDevicesInfo << d;
        }
    }
    emit finished(m_legoDevicesInfo);
    emit devicesFoundUpdated();
}

/*!
    \property QLegoDeviceScanner::devicesFound
    \brief the number of devices the scanner has detected.
*/
int QLegoDeviceScanner::devicesFound() const
{
    return m_legoDevicesInfo.size();
}

/*
https://github.com/nathankellenicki/node-poweredup/blob/master/src/consts.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/nobleabstraction.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/poweredup-node.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/utils.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/hubs/basehub.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/hubs/lpf2hub.ts
https://github.com/nathankellenicki/node-poweredup/blob/master/src/hubs/movehub.ts
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/src/bluetooth
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/src/bluetooth/bluez
https://doc.qt.io/qt-5/qbluetoothdeviceinfo.html#manufacturerData
https://doc.qt.io/qt-5/qtbluetooth-module.html
https://doc.qt.io/qt-5/qlowenergycontroller.html
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/examples/bluetooth/lowenergyscanner?h=5.15
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/examples/bluetooth/lowenergyscanner/deviceinfo.cpp?h=5.15
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/examples/bluetooth/lowenergyscanner/device.h?h=5.15
https://code.qt.io/cgit/qt/qtconnectivity.git/tree/examples/bluetooth/lowenergyscanner/device.cpp?h=5.15
https://doc.qt.io/qt-5/qlowenergycontroller.html
https://doc.qt.io/qt-5/qbluetoothdeviceinfo.html
https://doc.qt.io/qt-5/qbluetoothaddress.html
https://doc.qt.io/qt-5/qtbluetooth-le-overview.html
https://lego.github.io/lego-ble-wireless-protocol-docs/

public subscribeToCharacteristic (uuid: string, callback: (data: Buffer) => void) {
        uuid = this._sanitizeUUID(uuid);
        this._characteristics[uuid].on("data", (data: Buffer) => {
            return callback(data);
        });
        this._characteristics[uuid].subscribe((err) => {
            if (err) {
                throw new Error(err);
            }
        });
}
*/
