#include <QApplication>
#include <QCoreApplication>
#include <QString>
#include <QDebug>
#include "client.h"
#include "server.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        qDebug() << "Veuillez mettre un (et un seul) argument :";
        qDebug() << argv[0] << "server  -> Pour lancer le serveur";
        qDebug() << argv[0] << "client  -> Pour créer un client sur le serveur";
        return 1; // Quitte si aucun argument n'est fourni
    }

    QString mode = argv[1];

    if (mode == "server") {
        // Lancer une application console pour le serveur
        QCoreApplication a(argc, argv);
        Server server;
        return a.exec();
    }
    else if (mode == "client") {
        // Lancer une application graphique pour le client
        QApplication a(argc, argv);
        Client client;
        client.show();
        return a.exec();
    }
    else {
        qDebug() << "Argument invalide. Utilisez 'server' ou 'client'.";
        return 1;
    }
}
