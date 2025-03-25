#include "client.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>
#include <QDebug>

Client::Client(QWidget *parent) : QWidget(parent) {
    setFixedSize(600, 400);
    canvas = QPixmap(size());
    canvas.fill(Qt::white);

    lastPoint = QPoint(-1, -1);

    tcpSocket = new QTcpSocket(this);
    udpSocket = new QUdpSocket(this);

    connect(tcpSocket, &QTcpSocket::connected, this, &Client::connectedToServer);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::readTcpData);
    connect(udpSocket, &QUdpSocket::readyRead, this, &Client::readUdpData);

    tcpSocket->connectToHost("127.0.0.1", 1234);
    udpSocket->bind(5678, QUdpSocket::ShareAddress);
}

Client::~Client() {
    tcpSocket->close();
    udpSocket->close();
}

void Client::connectedToServer() {
    qDebug() << "Connecté au serveur";
}

void Client::readTcpData() {
    QByteArray data = tcpSocket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    if (obj["type"] == "handshake") {
        myColor = QColor(obj["color"].toString());
        qDebug() << "Couleur assignée :" << myColor.name();
    }
}

void Client::readUdpData() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        QDataStream stream(datagram);
        int x1, y1, x2, y2, r, g, b;
        stream >> x1 >> y1 >> x2 >> y2 >> r >> g >> b;

        QPainter painter(&canvas);
        QPen pen(QColor(r, g, b));
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawLine(QPoint(x1, y1), QPoint(x2, y2));

        update();
    }
}


void Client::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, canvas);
}

void Client::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if (lastPoint != QPoint(-1, -1)) { // Vérifie qu'on ne commence pas sur un point invalide
            sendCanvasUpdate(lastPoint, event->pos());
        }
        lastPoint = event->pos();
    }
}


void Client::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastPoint = QPoint(-1, -1); // Réinitialiser à une position invalide
    }
}


void Client::sendCanvasUpdate(QPoint from, QPoint to) {
    QPainter painter(&canvas);
    QPen pen(myColor);
    pen.setWidth(3); // Définit une taille fixe pour le trait
    painter.setPen(pen);
    painter.drawLine(from, to);
    update();

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << from.x() << from.y() << to.x() << to.y() << myColor.red() << myColor.green() << myColor.blue();

    udpSocket->writeDatagram(data, QHostAddress::Broadcast, 5678);
}
