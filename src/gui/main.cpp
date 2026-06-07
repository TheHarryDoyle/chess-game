#include <QApplication>

#include "chess/ChessWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    chess::ChessWindow window;
    window.show();

    return app.exec();
}
