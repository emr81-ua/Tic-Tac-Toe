#include <SFML/Graphics.hpp>
#include <array>
#include <string>
#include <windows.h> // Para MessageBox en Windows

const int CELL_SIZE = 60;
const int GRID_SIZE = 3;
const int BOARD_SIZE = GRID_SIZE * CELL_SIZE;
const int WINDOW_SIZE = GRID_SIZE * BOARD_SIZE;

enum class Player { None, X, O };

class SubBoard {
public:
    SubBoard() {
        board.fill(Player::None);
        winner = Player::None;
        full = false;
    }


    bool handleClick(int x, int y, Player currentPlayer) {
        int row = y / CELL_SIZE;
        int col = x / CELL_SIZE;
        if (row < 0 || row >= 3 || col < 0 || col >= 3) return false;

        // No permitir jugadas si el subtablero ya tiene ganador
        if (winner != Player::None) return false;

        if (board[row * 3 + col] == Player::None) {
            board[row * 3 + col] = currentPlayer;
            checkStatus(currentPlayer);
            return true;
        }
        return false;
    }


    void draw(sf::RenderWindow& window, int offsetX, int offsetY, bool highlight) {
        for (int i = 1; i < 3; ++i) {
            sf::RectangleShape line(sf::Vector2f(BOARD_SIZE, 2));
            line.setFillColor(sf::Color::Black);
            line.setPosition(offsetX, offsetY + i * CELL_SIZE);
            window.draw(line);

            line.setSize(sf::Vector2f(2, BOARD_SIZE));
            line.setPosition(offsetX + i * CELL_SIZE, offsetY);
            window.draw(line);
        }

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                float x = offsetX + col * CELL_SIZE + CELL_SIZE / 2.f;
                float y = offsetY + row * CELL_SIZE + CELL_SIZE / 2.f;
                Player p = board[row * 3 + col];

                if (p == Player::X) {
                    sf::RectangleShape line1(sf::Vector2f(40, 4));
                    line1.setFillColor(sf::Color::Red);
                    line1.setOrigin(20, 2);
                    line1.setPosition(x, y);
                    line1.setRotation(45);

                    sf::RectangleShape line2 = line1;
                    line2.setRotation(-45);

                    window.draw(line1);
                    window.draw(line2);
                }
                else if (p == Player::O) {
                    sf::CircleShape circle(20);
                    circle.setFillColor(sf::Color::Transparent);
                    circle.setOutlineColor(sf::Color::Blue);
                    circle.setOutlineThickness(4);
                    circle.setOrigin(20, 20);
                    circle.setPosition(x, y);
                    window.draw(circle);
                }
            }
        }

        if (highlight) {
            sf::RectangleShape border(sf::Vector2f(BOARD_SIZE, BOARD_SIZE));
            border.setFillColor(sf::Color::Transparent);
            border.setOutlineColor(sf::Color::Green);
            border.setOutlineThickness(4);
            border.setPosition(offsetX, offsetY);
            window.draw(border);
        }
    }

    Player getWinner() const { return winner; }
    bool isFull() const { return full; }

private:
    std::array<Player, 9> board;
    Player winner;
    bool full;

    void checkStatus(Player currentPlayer) {
        const int wins[8][3] = {
            {0,1,2},{3,4,5},{6,7,8},
            {0,3,6},{1,4,7},{2,5,8},
            {0,4,8},{2,4,6}
        };

        for (auto& combo : wins) {
            if (board[combo[0]] == currentPlayer && board[combo[1]] == currentPlayer && board[combo[2]] == currentPlayer) {
                winner = currentPlayer;
                return;
            }
        }

        full = true;
        for (auto& cell : board) {
            if (cell == Player::None) {
                full = false;
                break;
            }
        }
    }
};

class SuperTicTacToe {
public:
    SuperTicTacToe() { reset(); }

    bool handleClick(int x, int y) {
        if (gameOver) return false;

        int boardRow = y / BOARD_SIZE;
        int boardCol = x / BOARD_SIZE;
        int subBoardIndex = boardRow * 3 + boardCol;

        if (activeBoard != -1) {
            if (subBoards[activeBoard].isFull() || subBoards[activeBoard].getWinner() != Player::None) {
                activeBoard = -1; 
            }
            else if (subBoardIndex != activeBoard) {
                return false; 
            }
        }

        int localX = x % BOARD_SIZE;
        int localY = y % BOARD_SIZE;

        if (subBoards[subBoardIndex].handleClick(localX, localY, currentPlayer)) {
            if (checkWin(currentPlayer)) {
                winner = currentPlayer;
                gameOver = true;
            }
            else {
                int nextBoardRow = (localY / CELL_SIZE);
                int nextBoardCol = (localX / CELL_SIZE);
                activeBoard = nextBoardRow * 3 + nextBoardCol;
                if (subBoards[activeBoard].isFull()) {
                    activeBoard = -1;
                }
                currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
            }
            return true;
        }
        return false;
    }

    void draw(sf::RenderWindow& window) {
        window.clear(sf::Color::White);

        for (int i = 1; i < 3; ++i) {
            sf::RectangleShape line(sf::Vector2f(WINDOW_SIZE, 4));
            line.setFillColor(sf::Color::Black);
            line.setPosition(0, i * BOARD_SIZE);
            window.draw(line);

            line.setSize(sf::Vector2f(4, WINDOW_SIZE));
            line.setPosition(i * BOARD_SIZE, 0);
            window.draw(line);
        }

        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                int index = row * 3 + col;
                bool highlight = (activeBoard == -1 || activeBoard == index);
                subBoards[index].draw(window, col * BOARD_SIZE, row * BOARD_SIZE, highlight);
            }
        }

        if (winner != Player::None) {
            float centerX = WINDOW_SIZE / 2.f;
            float centerY = WINDOW_SIZE / 2.f;

            if (winner == Player::X) {
                sf::RectangleShape line1(sf::Vector2f(80, 6));
                line1.setFillColor(sf::Color(255, 0, 0, 100)); // rojo semitransparente
                line1.setOrigin(40, 3);
                line1.setPosition(centerX, centerY);
                line1.setRotation(45);

                sf::RectangleShape line2 = line1;
                line2.setRotation(-45);

                window.draw(line1);
                window.draw(line2);
            }
            else if (winner == Player::O) {
                sf::CircleShape circle(40);
                circle.setFillColor(sf::Color::Transparent);
                circle.setOutlineColor(sf::Color(0, 0, 255, 100)); // azul semitransparente
                circle.setOutlineThickness(6);
                circle.setOrigin(40, 40);
                circle.setPosition(centerX, centerY);
                window.draw(circle);
            }
        }


        window.display();
    }

    bool isGameOver() const { return gameOver; }
    Player getWinner() const { return winner; }

    void reset() {
        subBoards.fill(SubBoard());
        currentPlayer = Player::O;
        winner = Player::None;
        gameOver = false;
        activeBoard = -1;
    }

private:
    std::array<SubBoard, 9> subBoards;
    Player currentPlayer;
    Player winner;
    bool gameOver;
    int activeBoard; // -1 indica que se puede jugar en cualquier subtablero

    bool checkWin(Player p) {
        std::array<Player, 9> meta;
        for (int i = 0; i < 9; ++i)
            meta[i] = subBoards[i].getWinner();

        const int wins[8][3] = {
            {0,1,2},{3,4,5},{6,7,8},
            {0,3,6},{1,4,7},{2,5,8},
            {0,4,8},{2,4,6}
        };

        for (auto& combo : wins) {
            if (meta[combo[0]] == p && meta[combo[1]] == p && meta[combo[2]] == p)
                return true;
        }
        return false;
    }
};

void showMessage(const std::string& message) {
    MessageBoxA(NULL, message.c_str(), "Resultado", MB_OK | MB_ICONINFORMATION);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Super Tic Tac Toe");

    SuperTicTacToe game;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!game.isGameOver() && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                game.handleClick(event.mouseButton.x, event.mouseButton.y);
        }

        game.draw(window);

        if (game.isGameOver()) {
            std::string result = (game.getWinner() == Player::X) ? "Ganó la X" : "Ganó la O";
            showMessage(result);
            game.reset();
        }
    }

    return 0;
}
