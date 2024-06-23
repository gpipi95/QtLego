#include "qlegodevice.h"
#include "qlegodevicemanager.h"
#include "qlegomotor.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QLoggingCategory>
#include <QThread>
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
        QLegoDevice* device = nullptr;
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
            case '1':
                device = manager->device();
                if (device) {
                    qDebug() << "device is not null.";
                    // test motor A
                    auto motorA = device->getMotorA();
                    if (motorA) {
                        qDebug() << "Motor is not null.";
                        motorA->setPower(50);
                        QThread::msleep(4000);
                        motorA->setPower(0);
                    }
                }
                break;
            case '2':
                device = manager->device();
                if (device) {
                    qDebug() << "device is not null.";
                    // test motor A
                    auto motorB = device->getMotorB();
                    if (motorB) {
                        qDebug() << "Motor is not null.";
                        motorB->setPower(50);
                        QThread::msleep(4000);
                        motorB->setPower(0);
                    }
                }
                break;
            case '3':
                device = manager->device();
                if (device) {
                    qDebug() << "device is not null.";
                    // test motor A
                    auto motorAB = device->getMotorSyncAB();
                    if (motorAB) {
                        qDebug() << "Motor is not null.";
                        motorAB->setPower(50);
                        QThread::msleep(4000);
                        motorAB->setPower(0);
                    }
                }
                break;
            case '4':
                device = manager->device();
                if (device) {
                    qDebug() << "device is not null.";
                    // test motor A
                    auto motorHead = device->getMotorHead();
                    if (motorHead) {
                        qDebug() << "Motor is not null.";
                        motorHead->setPower(40);
                        QThread::msleep(1000);
                        motorHead->setPower(-40);
                        QThread::msleep(1000);
                        motorHead->setPower(0);
                    }
                }
                break;
            default:
                break;
            }
        }
    });

    timer->start();

    QObject::connect(manager, &QLegoDeviceManager::scanFinished, [manager]() {
        auto device = manager->device();
        if (device) {
            qDebug() << "device is not null.";
            // test motor head
            //            auto motorHead = device->getMotorHead();
            //            if (motorHead) {
            //                qDebug() << "MotorHead is not null.";
            //                motorHead->setPower(50);
            //                QThread::msleep(2000);
            //                motorHead->setPower(0);
            //            }
        }
    });

    return app.exec();
}
