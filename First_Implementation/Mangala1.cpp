//when a move made to play, player has to enter a number to make a move from that hole. and if player choses a number bigger than 6 and/or a input different from a number, this code has to print "wrong input. try again"
// and if there is n stones inside a hole, there has to be 1 stone left in that hole. and the next holes has to be add by 1. for example, there is 6 stones in the 3rd hole. if i press 3, 1 stone has to be left in the 3th hole. and by the equation "n-1", there has to be next 6-1=5 holes has to be add by 1.

//Players turn do not going as they should. If the last stone goes to the main hole, the next turn is again that player's turn. If the last stone(last number) does not go the main hole, the turn is the other player's turn.



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
    
    //************** printing the game board **********************
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
    //**************************************************************
    //-*-*-*-*-*-*-*-* arrange holes from here -*-*-*-*-*-*-*-*-*-*-*
    // 
    void makeMove(int index) {
        int stones = board[index];
        board[index] = +1;

        while (stones > 0) {
            index = (index + 1) % 14;
            if (index == 0 && currentPlayer == 2)
                continue;
            board[index]++;
            stones--;
        }

	//if a move made on a hole that has no stones, it is not allowed.
	

        if (index != 0 && index != 7) {
            if (board[index] == 1 && board[14 - index] > 0 && index <= 7) {
                board[7] += board[index] + board[14 - index];
                board[index] = 0;
                board[14 - index] = 0;
            }
            switchPlayer();
        }
        if (isPlayerEmpty()) { //has to be corrected
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
        endGame(); 
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
        cout << "Oynamak İstediğiniz Kuyuyu Seçiniz: " /*<< endl*/;
        cin >> move;
        
        if (cin.fail() || move < 1 || move > 6) {
            cout << "Yanlış Tuşa Bastınız! Tekrar Deneyiniz." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
        } else {
            game.makeMove(move);
        }
        
        int winner = game.checkWinner();
        if (winner != 0) {
            game.printBoard();
            cout << "Oyuncu " << winner << " Kazandı!" << endl;
            break;
        }
    }
    return 0;
}

