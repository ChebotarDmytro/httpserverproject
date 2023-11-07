#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QByteArray>
#include <QHash>
#include <QUrl>

struct HttpRequestData {
    QString contentType{};
    int contentLength{0};
    QByteArray body{};
};

class HttpServer : public QObject {
    Q_OBJECT

public:
    HttpServer(QObject* parent = nullptr);
    ~HttpServer();

    bool start(int port);
    void stop();

public slots:
    void newConnection();
    void processRequest();

private:
    enum class HttpMethod {
        GET,
        POST,
        DELETE,
        UNKNOWN
    };

    bool parseRequest(const QByteArray& requestData, QUrl& requestUrl, HttpMethod& httpMethod);

    QByteArray manageGetRequest(const QUrl& requestUrl);
    QByteArray managePostRequest();
    QByteArray manageDeleteRequest(const QUrl& requestUrl);

    QTcpServer* m_server = nullptr;
    QHash<QString, HttpRequestData> m_dataStore{};
};
