#pragma once

#include <optional>

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QWidget>

#include "Board.h"
#include "ChessEngine.h"
#include "Position.h"

namespace chess {

class ChessWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ChessWindow(QWidget* parent = nullptr);

private slots:
    void handleCellClick(int row, int column);
    void startNewGame();
    void resetBoard();
    void runTests();
    void exitApplication();

private:
    void buildUi();
    void refreshBoard();
    void updateStatusPanel(const QString& message = QString());
    void clearSelection();
    QString testExecutablePath() const;

    Board board_;
    ChessEngine engine_ {2};

    QTableWidget* boardWidget_ {nullptr};

    QPushButton* newGameButton_ {nullptr};
    QPushButton* resetBoardButton_ {nullptr};
    QPushButton* runTestsButton_ {nullptr};
    QPushButton* exitButton_ {nullptr};

    QLabel* turnValueLabel_ {nullptr};
    QLabel* engineValueLabel_ {nullptr};
    QLabel* gameValueLabel_ {nullptr};
    QLabel* messageValueLabel_ {nullptr};

    std::optional<Position> selectedSquare_;
};

} // namespace chess
