// according to rules, stones goes right but when there are 1 stone left and when that hole is selected, tha hole becomes empty. but in this code it does not become empty. there are always 1 stone left which is we dont want.
// tur: kurallara göre taşlar doğru gidiyor ama eğer bir kuyuda 1 taş kaldıysa ve o kuyu oynandıysa normalde o kuyunun boş olması ve o son taşın diğer kuyuya gitmesi gerekir

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

        if (board[index] == 0) {
            // You can't make a move when an empty hole selected.
            cout << "Boş kuyuyu oynayamazsınız. Tekrar deneyin." << endl;
            return;
        }

        int stones = board[index];
        board[index] = 0;

        //this looks wrong. solve it
        while (stones > 0) {
            if (index == 0 && currentPlayer == 2)
                continue;
            board[index]++;
            stones--;
            index = index+1 % 14;
            cout << index << " " << stones << endl;
        }
	

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
        cout << "Oynamak İstediğiniz Kuyuyu Seçiniz: " ;
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

