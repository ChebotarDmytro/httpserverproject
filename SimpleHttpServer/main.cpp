#include <QCoreApplication>

#include "HttpServer.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    HttpServer httpServer;
    if (httpServer.start(8000)) {
        qDebug() << "HTTP Server is running on port 8000";
        QObject::connect(&app, &QCoreApplication::aboutToQuit, &httpServer, &HttpServer::stop);
    } else {
        qDebug() << "Failed to start the HTTP Server";
        return 1;
    }

    return app.exec();
}
