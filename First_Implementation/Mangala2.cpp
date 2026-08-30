#include <iostream>
#include <vector>

using namespace std;

class MangalaGame {
private:
    vector<int> board;
    int currentPlayer;
    bool sumInProgress;

public:
    MangalaGame() : board(14, 0), currentPlayer(1), sumInProgress(false) {
        // Initialize the holes with 6 stones each.
        for (int i = 1; i <= 6; i++) {
            board[i] = 6;
            board[i + 7] = 6;
        }
    }

    // Function to print the game board.
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

    // Function to make a move.
    void makeMove(int index) {
        if (board[index] == 0) {
            // You can't make a move on an empty hole.
            cout << "Invalid move. Try again." << endl;
            return;
        }

        int stones = board[index];
        board[index] = 1; // Leave one stone in the selected hole.

        while (stones > 0) {
            index = (index + 1) % 14;
            board[index]++;
            stones--;
        }

        // Check if the last stone landed in the player's main hole.
        if (index != 0 && index != 7) {
            if (board[index] == 1 && board[14 - index] > 0) {
                board[7] += board[index] + board[14 - index];
                board[index] = 0;
                board[14 - index] = 0;
            } else {
                switchPlayer();
            }
        }

        // Check if the current player's side is empty.
        if (isPlayerEmpty()) {
            sumInProgress = true;
            takeOpponentStones();
        }
    }

    // Function to switch the current player.
    void switchPlayer() {
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }

    // Function to check if the current player's side is empty.
    bool isPlayerEmpty() const {
        int start = (currentPlayer == 1) ? 1 : 8;
        int end = start + 5;
        for (int i = start; i <= end; ++i) {
            if (board[i] > 0) return false;
        }
        return true;
    }

    // Function to collect opponent's stones when the game ends.
    void takeOpponentStones() {
        int start = (currentPlayer == 1) ? 8 : 1;
        int end = start + 5;
        for (int i = start; i <= end; ++i) {
            board[7] += board[i];
            board[i] = 0;
        }
        endGame();
    }

    // Function to end the game.
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

    // Function to check the winner.
    int checkWinner() {
        if (sumInProgress) {
            if (board[0] > board[7]) return 1;
            else if (board[7] > board[0]) return 2;
            else return 0; // It's a tie.
        } else {
            return 0; // The game is still in progress.
        }
    }

    // Function to get the current player.
    int getCurrentPlayer() const {
        return currentPlayer;
    }
};

int main() {
    MangalaGame game;
    int move;
    while (true) {
        cout << "=====================================" << endl;
        cout << "Player " << game.getCurrentPlayer() << "'s Turn." << endl;
        game.printBoard();
        cout << "Select a hole to move from (1-6): ";
        cin >> move;

        if (cin.fail() || move < 1 || move > 6) {
            cout << "Invalid input. Try again." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
        } else {
            game.makeMove(move);
        }

        int winner = game.checkWinner();
        if (winner != 0) {
            game.printBoard();
            if (winner == 1 || winner == 2) {
                cout << "Player " << winner << " wins!" << endl;
            } else {
                cout << "It's a tie!" << endl;
            }
            break;
        }
    }
    return 0;
}

