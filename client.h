#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QUdpSocket>
#include <QWidget>
#include <QPainter>

class Client : public QWidget {
    Q_OBJECT
public:
    explicit Client(QWidget *parent = nullptr);
    ~Client();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


private slots:
    void connectedToServer();
    void readTcpData();
    void readUdpData();

private:
    QTcpSocket *tcpSocket;
    QUdpSocket *udpSocket;
    QColor myColor;
    QPixmap canvas;
    QPoint lastPoint;

    void sendCanvasUpdate(QPoint x, QPoint y);
};

#endif // CLIENT_H
