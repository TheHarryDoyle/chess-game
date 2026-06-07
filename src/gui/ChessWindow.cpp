#include "chess/ChessWindow.h"

#include <QAbstractItemView>
#include <QCoreApplication>
#include <QFileInfo>
#include <QFont>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProcess>
#include <QVBoxLayout>

#include "chess/Move.h"
#include "chess/Piece.h"

namespace chess {

namespace {
QString colorText(Color color) {
    return (color == Color::White) ? "White" : "Black";
}
}

ChessWindow::ChessWindow(QWidget* parent)
    : QMainWindow(parent) {
    buildUi();
    refreshBoard();
    updateStatusPanel("Select a piece, then click a legal destination square.");
}

void ChessWindow::buildUi() {
    auto* central = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(14);

    auto* toolbarFrame = new QFrame(central);
    toolbarFrame->setFrameShape(QFrame::StyledPanel);
    auto* toolbarLayout = new QHBoxLayout(toolbarFrame);
    toolbarLayout->setContentsMargins(12, 12, 12, 12);
    toolbarLayout->setSpacing(10);

    newGameButton_ = new QPushButton("New Game", toolbarFrame);
    resetBoardButton_ = new QPushButton("Reset Board", toolbarFrame);
    runTestsButton_ = new QPushButton("Run Tests", toolbarFrame);
    exitButton_ = new QPushButton("Exit", toolbarFrame);

    for (auto* button : {newGameButton_, resetBoardButton_, runTestsButton_, exitButton_}) {
        button->setMinimumHeight(34);
        button->setCursor(Qt::PointingHandCursor);
    }

    toolbarLayout->addWidget(newGameButton_);
    toolbarLayout->addWidget(resetBoardButton_);
    toolbarLayout->addWidget(runTestsButton_);
    toolbarLayout->addWidget(exitButton_);
    toolbarLayout->addStretch();

    auto* contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(16);

    boardWidget_ = new QTableWidget(8, 8, central);
    boardWidget_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    boardWidget_->setSelectionMode(QAbstractItemView::NoSelection);
    boardWidget_->setFocusPolicy(Qt::NoFocus);
    boardWidget_->setMinimumSize(640, 640);
    boardWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    boardWidget_->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    boardWidget_->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    boardWidget_->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    boardWidget_->horizontalHeader()->setMinimumSectionSize(24);
    boardWidget_->verticalHeader()->setMinimumSectionSize(24);
    boardWidget_->setHorizontalHeaderLabels({"a", "b", "c", "d", "e", "f", "g", "h"});
    boardWidget_->setVerticalHeaderLabels({"8", "7", "6", "5", "4", "3", "2", "1"});

    QFont pieceFont;
    pieceFont.setPointSize(24);
    boardWidget_->setFont(pieceFont);

    auto* statusFrame = new QFrame(central);
    statusFrame->setFrameShape(QFrame::StyledPanel);
    statusFrame->setMinimumWidth(230);
    auto* statusLayout = new QVBoxLayout(statusFrame);
    statusLayout->setContentsMargins(16, 16, 16, 16);
    statusLayout->setSpacing(10);

    auto* statusTitle = new QLabel("Status", statusFrame);
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    statusTitle->setFont(titleFont);

    auto* turnLabel = new QLabel("Turn:", statusFrame);
    auto* engineLabel = new QLabel("Engine:", statusFrame);
    auto* gameLabel = new QLabel("Game:", statusFrame);
    auto* messageLabel = new QLabel("Message:", statusFrame);

    turnValueLabel_ = new QLabel(statusFrame);
    engineValueLabel_ = new QLabel(statusFrame);
    gameValueLabel_ = new QLabel(statusFrame);
    messageValueLabel_ = new QLabel(statusFrame);
    messageValueLabel_->setWordWrap(true);

    statusLayout->addWidget(statusTitle);
    statusLayout->addSpacing(8);
    statusLayout->addWidget(turnLabel);
    statusLayout->addWidget(turnValueLabel_);
    statusLayout->addSpacing(4);
    statusLayout->addWidget(engineLabel);
    statusLayout->addWidget(engineValueLabel_);
    statusLayout->addSpacing(4);
    statusLayout->addWidget(gameLabel);
    statusLayout->addWidget(gameValueLabel_);
    statusLayout->addSpacing(4);
    statusLayout->addWidget(messageLabel);
    statusLayout->addWidget(messageValueLabel_);
    statusLayout->addStretch();

    contentLayout->addWidget(boardWidget_, 1);
    contentLayout->addWidget(statusFrame);

    rootLayout->addWidget(toolbarFrame);
    rootLayout->addLayout(contentLayout);

    setCentralWidget(central);
    setWindowTitle("Chess Application");
    resize(980, 760);

    connect(boardWidget_, &QTableWidget::cellClicked, this, &ChessWindow::handleCellClick);
    connect(newGameButton_, &QPushButton::clicked, this, &ChessWindow::startNewGame);
    connect(resetBoardButton_, &QPushButton::clicked, this, &ChessWindow::resetBoard);
    connect(runTestsButton_, &QPushButton::clicked, this, &ChessWindow::runTests);
    connect(exitButton_, &QPushButton::clicked, this, &ChessWindow::exitApplication);
}

void ChessWindow::refreshBoard() {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            auto* item = boardWidget_->item(row, col);
            if (item == nullptr) {
                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                boardWidget_->setItem(row, col, item);
            }

            const Piece piece = board_.pieceAt({row, col});
            item->setText(QString::fromUtf8(pieceToUnicode(piece)));

            const bool lightSquare = ((row + col) % 2 == 0);
            item->setBackground(lightSquare ? QColor("#f0d9b5") : QColor("#b58863"));
        }
    }

    if (selectedSquare_.has_value()) {
        const Position selected = selectedSquare_.value();
        if (auto* item = boardWidget_->item(selected.row, selected.col)) {
            item->setBackground(QColor("#8fd19e"));
        }
    }
}

void ChessWindow::updateStatusPanel(const QString& message) {
    const Color sideToMove = board_.currentTurn();

    turnValueLabel_->setText(colorText(sideToMove) + " to move");
    engineValueLabel_->setText("Ready");

    QString gameState = "Active";
    if (board_.isCheckmate(sideToMove)) {
        gameState = "Checkmate";
    } else if (board_.isStalemate(sideToMove)) {
        gameState = "Stalemate";
    } else if (board_.isInCheck(sideToMove)) {
        gameState = "Check";
    }
    gameValueLabel_->setText(gameState);

    if (message.isEmpty()) {
        messageValueLabel_->setText("Select a piece, then click a legal destination square.");
    } else {
        messageValueLabel_->setText(message);
    }
}

void ChessWindow::clearSelection() {
    selectedSquare_.reset();
}

void ChessWindow::startNewGame() {
    board_.reset();
    clearSelection();
    refreshBoard();
    updateStatusPanel("New game started.");
}

void ChessWindow::resetBoard() {
    board_.reset();
    clearSelection();
    refreshBoard();
    updateStatusPanel("Board reset to starting position.");
}

QString ChessWindow::testExecutablePath() const {
#ifdef Q_OS_WIN
    const QString exeName = "chess_tests.exe";
#else
    const QString exeName = "chess_tests";
#endif

    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidates {
        appDir + "/" + exeName,
        appDir + "/Debug/" + exeName,
        appDir + "/Release/" + exeName,
        QFileInfo(appDir + "/../" + exeName).absoluteFilePath(),
        QFileInfo(appDir + "/../Debug/" + exeName).absoluteFilePath(),
        QFileInfo(appDir + "/../Release/" + exeName).absoluteFilePath()
    };

    for (const QString& path : candidates) {
        if (QFileInfo::exists(path)) {
            return path;
        }
    }

    return {};
}

void ChessWindow::runTests() {
    const QString testPath = testExecutablePath();
    if (testPath.isEmpty()) {
        updateStatusPanel("Could not find the chess_tests executable.");
        QMessageBox::warning(this, "Run Tests", "Could not find the chess_tests executable in the build output folder.");
        return;
    }

    auto* process = new QProcess(this);
    process->setProgram(testPath);

    connect(process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
                const QString stdOut = QString::fromLocal8Bit(process->readAllStandardOutput());
                const QString stdErr = QString::fromLocal8Bit(process->readAllStandardError());

                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    updateStatusPanel("All tests passed.");
                    QMessageBox::information(this, "Run Tests", stdOut.isEmpty() ? "All tests passed." : stdOut);
                } else {
                    updateStatusPanel("Tests failed.");
                    QMessageBox::warning(this, "Run Tests", "Tests failed.\n\n" + stdOut + "\n" + stdErr);
                }

                process->deleteLater();
            });

    connect(process,
            &QProcess::errorOccurred,
            this,
            [this, process](QProcess::ProcessError) {
                updateStatusPanel("Failed to launch test process.");
                QMessageBox::warning(this, "Run Tests", "Failed to launch the test executable.");
                process->deleteLater();
            });

    updateStatusPanel("Running tests...");
    process->start();
}

void ChessWindow::exitApplication() {
    close();
}

void ChessWindow::handleCellClick(int row, int column) {
    const Position clicked {row, column};
    const Piece clickedPiece = board_.pieceAt(clicked);

    if (!selectedSquare_.has_value()) {
        if (!clickedPiece.isEmpty() && clickedPiece.color == board_.currentTurn()) {
            selectedSquare_ = clicked;
            refreshBoard();
            updateStatusPanel("Piece selected. Choose a destination square.");
        }
        return;
    }

    const Position from = selectedSquare_.value();

    if (from == clicked) {
        clearSelection();
        refreshBoard();
        updateStatusPanel("Selection cleared.");
        return;
    }

    if (board_.movePiece({from, clicked})) {
        clearSelection();
        refreshBoard();

        Color sideToMove = board_.currentTurn();
        if (board_.isCheckmate(sideToMove)) {
            updateStatusPanel((sideToMove == Color::White) ? "Checkmate. Black wins." : "Checkmate. White wins.");
            return;
        }
        if (board_.isStalemate(sideToMove)) {
            updateStatusPanel("Stalemate.");
            return;
        }

        if (sideToMove == Color::Black) {
            const auto engineMove = engine_.chooseMove(board_, Color::Black);
            if (engineMove.has_value()) {
                board_.movePiece(engineMove.value());
                sideToMove = board_.currentTurn();
            }
        }

        refreshBoard();

        if (board_.isCheckmate(sideToMove)) {
            updateStatusPanel((sideToMove == Color::White) ? "Checkmate. Black wins." : "Checkmate. White wins.");
        } else if (board_.isStalemate(sideToMove)) {
            updateStatusPanel("Stalemate.");
        } else if (board_.isInCheck(sideToMove)) {
            updateStatusPanel(colorText(sideToMove) + " to move — check.");
        } else {
            updateStatusPanel(colorText(sideToMove) + " to move.");
        }
        return;
    }

    if (!clickedPiece.isEmpty() && clickedPiece.color == board_.currentTurn()) {
        selectedSquare_ = clicked;
        refreshBoard();
        updateStatusPanel("Changed selected piece.");
        return;
    }

    updateStatusPanel("Illegal move.");
}

} // namespace chess
