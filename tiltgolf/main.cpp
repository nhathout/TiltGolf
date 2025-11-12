#include <QtCore>
#include <QtGui>

#include "tiltgolf.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TiltGolf game;
    game.setWindowTitle("Kinetic Scrolling");
    game.resize(320, 320);
    game.show();
    return app.exec();
}
