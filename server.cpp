#include "server.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>

Server::Server(QObject *parent) : QObject(parent) {
    tcpServer = new QTcpServer(this);
    udpSocket = new QUdpSocket(this);

    connect(tcpServer, &QTcpServer::newConnection, this, &Server::newConnection);

    if (tcpServer->listen(QHostAddress::Any, 1234)) {
        qDebug() << "Serveur TCP en écoute sur le port 1234";
    }

    if (udpSocket->bind(5678, QUdpSocket::ShareAddress)) {
        connect(udpSocket, &QUdpSocket::readyRead, this, &Server::readUdpData);
    }
}

Server::~Server() {
    tcpServer->close();
    udpSocket->close();
}

void Server::newConnection() {
    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::readTcpData);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::clientDisconnected);

    QString color = QString("#%1%2%3")
                        .arg(QRandomGenerator::global()->bounded(256), 2, 16, QChar('0'))
                        .arg(QRandomGenerator::global()->bounded(256), 2, 16, QChar('0'))
                        .arg(QRandomGenerator::global()->bounded(256), 2, 16, QChar('0'));

    clients[clientSocket] = color;

    // Envoi du handshake avec la couleur
    QJsonObject handshake;
    handshake["color"] = color;
    handshake["type"] = "handshake";
    clientSocket->write(QJsonDocument(handshake).toJson());

    qDebug() << "Nouveau client connecté avec couleur" << color;

    // **💡 Envoyer l'historique du canvas en UDP**
    for (const QByteArray &data : canvasHistory) {
        udpSocket->writeDatagram(data, clientSocket->peerAddress(), 5678);
    }
}

void Server::readTcpData() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = client->readAll();
    broadcastCanvasUpdate(data, client);
}

void Server::readUdpData() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        broadcastCanvasUpdate(datagram);
    }
}

void Server::broadcastCanvasUpdate(const QByteArray &data, QTcpSocket *exclude) {
    canvasHistory.append(data); // Sauvegarde les modifications

    for (QTcpSocket *client : clients.keys()) {
        if (client != exclude) {
            client->write(data);
        }
    }

    udpSocket->writeDatagram(data, QHostAddress::Broadcast, 5678);
}

void Server::clientDisconnected() {
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    if (clients.contains(client)) {
        qDebug() << "Client déconnecté";
        clients.remove(client);
    }
}
