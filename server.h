#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QMap>

class Server : public QObject {
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

private slots:
    void newConnection();
    void readTcpData();
    void readUdpData();
    void clientDisconnected();

private:
    QTcpServer *tcpServer;
    QUdpSocket *udpSocket;
    QList<QByteArray> canvasHistory;
    QMap<QTcpSocket*, QString> clients; // Associe chaque client Ã une couleur
    void broadcastCanvasUpdate(const QByteArray &data, QTcpSocket *exclude = nullptr);
};

#endif // SERVER_H
