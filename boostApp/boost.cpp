#include "qlegodevicemanager.h"
#include "qlegomotor.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QLoggingCategory>
#include <QTimer>
#include <QtDebug>
#include <QtEndian>

#include <conio.h>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QLoggingCategory::setFilterRules(QStringLiteral("lego.*=true"));

    auto timer = new QTimer();
    auto manager = new QLegoDeviceManager();

    manager->scan();

    timer->setInterval(500);
    timer->setSingleShot(false);

    QObject::connect(timer, &QTimer::timeout, [manager]() {
        if (_kbhit()) {
            switch (_getch()) {
            case 'x':
                qDebug() << "about to exit, disconnect all devices.";
                manager->disconnectAll();
                qApp->exit(0);
                break;
            case 'd':
                qDebug() << "disconnect all devices.";
                manager->disconnectAll();
                break;
            default:
                break;
            }
        }
    });

    timer->start();

    auto device = manager->device();

    return app.exec();
}
