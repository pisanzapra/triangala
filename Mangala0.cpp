// when a hole selected, there have to be 1 stone remaining. in this code, all stones get taken when that hole selected. this have to be corrected.

//Players turn do not going as they should. If the last stone goes to the main hole, the next turn is again that player's turn. If the last stone(last number) does not go the main hole, the turn is the other player's turn.
// -*-*-*-*-*-*-*-* currentPlayer sorunlu -*-*-*-*-*-*-*-*

//if a player presses the wrong button (like a letter or a number bigger than 6) it has to give a warning like: "You Pressed The Wrong Button! Try Again."


#include <iostream>
#include <vector>

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
    
    //-*-*-*-*-*-*-*-* kuyuyu buradan düzenle -*-*-*-*-*-*-*-*-*-*-*
    void makeMove(int index) {
        int stones = board[index];
        board[index] = 0; // Empty the selected hole

        while (stones > 0) {
            index = (index + 1) % 14;
            if (index == 0 && currentPlayer == 2)
                continue;
            board[index]++;
            stones--;
        }

        if (index != 0 && index != 7) {
            if (board[index] == 1 && board[14 - index] > 0 && index <= 6) {
                board[7] += board[index] + board[14 - index];
                board[index] = 0;
                board[14 - index] = 0;
            }
            switchPlayer();
        }
        if (isPlayerEmpty()) {
            sumInProgress = true;
            takeOpponentStones();
        }
    }
    //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
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
        cout << "=====================================" << endl; ;
        cout << "Oyuncu " << game.getCurrentPlayer() << "'in Sırası." << endl;
        game.printBoard();
        cout << "Oynamak İstediğiniz Kuyuyu Seçiniz: " << endl;
        cin >> move;
        game.makeMove(move);
        int winner = game.checkWinner();
        if (winner != 0) {
            game.printBoard();
            cout << "Oyuncu " << winner << " Kazandı!" << endl;
            break;
        }
    }
    return 0;
}

