#include "ui/window.h"
#include "model/AppModel.h"
#include "controller/FileBasketController.h"
#include "model/FileListModel.h"

#include <QApplication>
#include <QMutex>
#include <QFile>
#include <QDateTime>

QFile logFile;
QMutex logMutex;

void logMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QMutexLocker locker(&logMutex);
    if(!logFile.isOpen())
        return;

    QTextStream out(&logFile);

    QString level;
    switch(type)
    {
        case QtDebugMsg:    level = "DEBUG";    break;
        case QtInfoMsg:     level = "INFO";     break;
        case QtWarningMsg:  level = "WARNING";  break;
        case QtCriticalMsg: level = "CRITICAL"; break;
        case QtFatalMsg:    level = "FATAL";    break;
    }

    QString currentTime = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    if(type == QtDebugMsg || type == QtInfoMsg || type == QtWarningMsg)
    {
        out << currentTime << " [" << level <<"] " << msg << "\n";
    }
    else
    {
        out << currentTime << " [" << level <<"] " << msg
        << " | " << context.file << " -> " << context.function <<"\n";
    }
}

void initializeLogFile()
{
    logFile.setFileName("FileBasket.log");
    logFile.open(QIODevice::Append | QIODevice::Text);
    qInstallMessageHandler(logMessageHandler);
    qInfo() <<"---Log file is initialized---";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("dev");
    QCoreApplication::setApplicationName("FileBasket");

    initializeLogFile();

    AppModel model;
    FileBasketController controller(model);

    QFile file(":/UI/StyleLight.qss");
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        a.setStyleSheet(styleSheet);
    }

    Window w(model, controller);

    QObject::connect(&w, &Window::requestAddFiles, &controller, &FileBasketController::addFiles);
    QObject::connect(&w, &Window::requestCopy, &controller, &FileBasketController::copyTo);
    QObject::connect(&w, &Window::requestMove, &controller, &FileBasketController::moveTo);
    QObject::connect(&w, &Window::requestActiveTabChanged, &controller, &FileBasketController::setActiveTab);

    w.show();
    return a.exec();
}
