#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;
using namespace this_thread;
using namespace chrono_literals;

class MangalaGameAuto {
private:
    vector<int> board;
    int currentPlayer;

public:
    MangalaGameAuto() : board(14, 4), currentPlayer(1) {}

    void printBoard() {
        cout << "   ";
        for (int i = 13; i > 7; --i) {
            cout << board[i] << " ";
        }
        cout << endl << board[0] << "                   " << board[7] << endl;
        cout << "   ";
        for (int i = 1; i < 7; ++i) {
            cout << board[i] << " ";
        }
        cout << endl;
    }

    void makeMove(int index) {
        int stones = board[index];
        board[index] = 0;
        while (stones > 0) {
            index = (index + 1) % 14;
            if (index == 0 && currentPlayer == 2) continue;
            board[index]++;
            stones--;
        }
        if (index != 0 && board[index] == 1 && board[14 - index] > 0 && index <= 6) {
            board[7] += board[index] + board[14 - index];
            board[index] = 0;
            board[14 - index] = 0;
        }
        if (index == 0) {
            switchPlayer();
        }
    }

    void switchPlayer() {
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }

    int checkWinner() {
        if (board[0] > board[7]) return 1;
        else if (board[7] > board[0]) return 2;
        else return 0;
    }

    int getCurrentPlayer() const {
        return currentPlayer;
    }

    vector<int> getValidMoves() {
        vector<int> validMoves;
        if (currentPlayer == 1) {
            for (int i = 1; i <= 6; ++i) {
                if (board[i] > 0) {
                    validMoves.push_back(i);
                }
            }
        } else {
            for (int i = 8; i <= 13; ++i) {
                if (board[i] > 0) {
                    validMoves.push_back(i);
                }
            }
        }
        return validMoves;
    }

    int evaluateMove(int move) {
        int afterMoveScore = board[(move + board[move]) % 14];
        if (afterMoveScore == 1) {
            return 1; // Favorable move
        }
        return 0; // Neutral move
    }

    int findBestMove() {
        vector<int> validMoves = getValidMoves();
        int bestMove = validMoves[0];
        int bestScore = -1;
        for (int move : validMoves) {
            int moveScore = evaluateMove(move);
            if (moveScore > bestScore) {
                bestScore = moveScore;
                bestMove = move;
            }
        }
        return bestMove;
    }
};

int main() {
    MangalaGameAuto game;
    while (true) {
        cout << "Player " << game.getCurrentPlayer() << "'s turn." << endl;
        game.printBoard();
        sleep_for(1s);
        int move;
        if (game.getCurrentPlayer() == 1) {
            move = game.findBestMove();
        } else {
            move = game.findBestMove();
        }
        cout << "Move: " << move << endl;
        game.makeMove(move);
        int winner = game.checkWinner();
        if (winner != 0) {
            game.printBoard();
            cout << "Player " << winner << " wins!" << endl;
            break;
        }
        sleep_for(1s);
    }
    return 0;
}

