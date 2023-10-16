//hepten bozuk, havuzu atlıyor, taş bırakmıyor...
#include <iostream>
#include <vector>
#include <limits>

using namespace std;

class MangalaGame {
private:
    vector<int> board;
    int currentPlayer;
    bool sumInProgress;

public:
    MangalaGame() : board(14, 4), currentPlayer(1), sumInProgress(false) {
        board[0] = board[7] = 0;
    }

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
        if (stones == 0) {
            cout << "Yanlış Tuşa Bastınız! Tekrar Deneyiniz." << endl;
            return; // Don't make a move if the selected hole is empty.
        }
        board[index] = 0; // Empty the selected hole

        int currentIndex = index;

        while (stones > 0) {
            currentIndex = (currentIndex + 1) % 14;

            // Skip opponent's main hole
            if (currentPlayer == 1 && currentIndex == 7) {
                currentIndex = (currentIndex + 1) % 14;
            } else if (currentPlayer == 2 && currentIndex == 0) {
                currentIndex = (currentIndex + 1) % 14;
            }

            // Distribute stones
            board[currentIndex]++;
            stones--;

            // Check if the last stone ends in an empty hole on the player's side
            if (stones == 0 && board[currentIndex] == 1 && currentIndex >= 1 && currentIndex <= 6 && board[14 - currentIndex] > 0) {
                board[7] += board[currentIndex] + board[14 - currentIndex];
                board[currentIndex] = 0;
                board[14 - currentIndex] = 0;
            }
        }

        // Check if the last stone ends in the main hole
        if (currentIndex == 7) {
            sumInProgress = true; // Set the sum in progress if the last stone goes to the main hole
        }

        if (!sumInProgress) {
            // If the last stone did not end in the main hole, switch to the other player's turn
            switchPlayer();
        } else {
            sumInProgress = false; // Reset the sum in progress flag
        }
    }

    void switchPlayer() {
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }

    bool isPlayerEmpty() const {
        if (currentPlayer == 1) {
            for (int i = 1; i <= 6; ++i) {
                if (board[i] > 0) return false;
            }
            return true;
        } else {
            for (int i = 8; i <= 13; ++i) {
                if (board[i] > 0) return false;
            }
            return true;
        }
    }

    void takeOpponentStones() {
        if (currentPlayer == 1) {
            for (int i = 8; i <= 13; ++i) {
                board[0] += board[i];
                board[i] = 0;
            }
        } else {
            for (int i = 1; i <= 6; ++i) {
                board[7] += board[i];
                board[i] = 0;
            }
        }
        endGame(); // End the game and move stones to the main holes
    }

    void endGame() {
        for (int i = 1; i <= 6; ++i) {
            board[7] += board[i];
            board[i] = 0;
        }
        for (int i = 8; i <= 13; ++i) {
            board[0] += board[i];
            board[i] = 0;
        }
    }

    int checkWinner() {
        if (sumInProgress) {
            if (board[0] > board[7]) return 1;
            else if (board[7] > board[0]) return 2;
            else return 0;
        } else {
            return 0;
        }
    }

    int getCurrentPlayer() const {
        return currentPlayer;
    }
};

int main() {
    MangalaGame game;
    int move;
    while (true) {
        cout << "=====================================" << endl;
        cout << "Oyuncu " << game.getCurrentPlayer() << "'in Sırası." << endl;
        game.printBoard();
        cout << "Oynamak İstediğiniz Kuyuyu Seçiniz: ";
        cin >> move;
        if (move < 1 || move > 6) {
            cout << "Yanlış Tuşa Bastınız! Tekrar Deneyiniz." << endl;
            continue;
        }
        game.makeMove(move);
        int winner = game.checkWinner();
        if (winner != 0) {
            game.printBoard();
            if (winner == 1)
                cout << "Oyuncu 1 Kazandı!" << endl;
            else
                cout << "Oyuncu 2 Kazandı!" << endl;
            break;
        }
    }
    return 0;
}

