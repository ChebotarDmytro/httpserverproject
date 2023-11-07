#include "HttpServer.h"

HttpServer::HttpServer(QObject *parent) : QObject(parent) {
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &HttpServer::newConnection);
}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start(int port) {
    return m_server->listen(QHostAddress::LocalHost, port);
}

void HttpServer::stop() {
    if (m_server->isListening()) {
        m_server->close();
    }

    for (QTcpSocket* clientSocket : m_server->findChildren<QTcpSocket*>()) {
        clientSocket->close();
        clientSocket->deleteLater();
    }
}

void HttpServer::newConnection() {
    if(m_server && m_server->hasPendingConnections()) {
        QTcpSocket* clientSocket = m_server->nextPendingConnection();
        connect(clientSocket, &QTcpSocket::readyRead, this, &HttpServer::processRequest);
    }
}

void HttpServer::processRequest() {
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QByteArray requestData = clientSocket->readAll();
    QByteArray response;

    QUrl requestUrl;
    HttpMethod httpMethod = HttpMethod::UNKNOWN;

    if(!parseRequest(requestData, requestUrl, httpMethod)) {
        response = "HTTP/1.1 400 Bad Request\r\n\r\n";
    } else {
        switch (httpMethod) {
            case HttpMethod::GET:
                response = manageGetRequest(requestUrl);
                break;
            case HttpMethod::POST:
                response = managePostRequest();
                break;
            case HttpMethod::DELETE:
                response = manageDeleteRequest(requestUrl);
                break;
            default:
                response = "HTTP/1.1 501 Not Implemented\r\n\r\n";
                break;
        }
    }

    clientSocket->write(response);
    clientSocket->waitForBytesWritten();
    clientSocket->close();
    clientSocket->deleteLater();
}

bool HttpServer::parseRequest(const QByteArray &requestData, QUrl &requestUrl, HttpMethod &httpMethod) {
    if(requestData.isEmpty()) {
        return false;
    }

    QString requestStr(requestData);
    qDebug() << "requestStr: " << requestStr;

    QStringList requestLines = requestStr.split("\r\n", Qt::SkipEmptyParts);

    if (requestLines.isEmpty()) {
        return false;
    }

    QString firstLine = requestLines.first();
    QStringList words = firstLine.split(" ", Qt::SkipEmptyParts);

    if (words.size() < 2) {
        return false;
    }

    QString httpMethodStr = words[0];
    QString uri = words[1];

    static const QHash<QString, HttpMethod> httpMethods = {{"GET", HttpMethod::GET},
                                                           {"POST", HttpMethod::POST},
                                                           {"DELETE", HttpMethod::DELETE}};
    if(!httpMethods.contains(httpMethodStr)) {
        return false;
    }

    requestUrl = QUrl(uri);

    if(httpMethodStr == "POST") {
        HttpRequestData data;

        for (const QString &line :  requestLines) {
            if (line.startsWith("Content-Type:")) {
                data.contentType = line.section(":", 1).trimmed();
            } else if (line.startsWith("Content-Length:")) {
                data.contentLength = line.section(":", 1).trimmed().toInt();
            }
        }

        static const QByteArray delimiter = "\r\n\r\n";
        if (data.contentLength > 0 && requestData.contains(delimiter)) {
            int bodyStart = requestData.indexOf(delimiter);
            data.body = QByteArray(requestData.mid(bodyStart +
                                                   delimiter.toStdString().size(), data.contentLength));
            m_dataStore[uri] = data;
        }
    }

    httpMethod = httpMethods.value(httpMethodStr);
    return true;
}

QByteArray HttpServer::manageGetRequest(const QUrl &requestUrl) {
    QByteArray response;
    QString uri = requestUrl.path();

    if (m_dataStore.contains(uri)) {
        QByteArray content = m_dataStore[uri].body;
        QString contentType = m_dataStore[uri].contentType;

        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + contentType.toStdString() + "\r\n";
        response += "Content-Length: " + QByteArray::number(content.length()) + "\r\n\r\n";
        response += content;
    } else {
        response = "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    return response;
}

QByteArray HttpServer::managePostRequest() {
    QByteArray response;
    response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Length: 0\r\n\r\n";

    return response;
}

QByteArray HttpServer::manageDeleteRequest(const QUrl &requestUrl) {
    QByteArray response;
    QString uri = requestUrl.path();

    if (m_dataStore.contains(uri)) {
        m_dataStore.remove(uri);

        response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Length: 0\r\n\r\n";
    } else {
        response = "HTTP/1.1 404 Not Found\r\n\r\n";
    }

    return response;
}
