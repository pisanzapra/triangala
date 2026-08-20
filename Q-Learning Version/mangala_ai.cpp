// Mangala (Mancala/Kalah) - 2 Kisilik + Bilgisayara Karsi (Minimax/Alpha-Beta AI)
// -----------------------------------------------------------------
// Board duzeni (Mangala0.cpp ile ayni):
//  - board[1..6]  : Oyuncu 1'in kuyulari
//  - board[8..13] : Oyuncu 2'nin kuyulari
//  - board[7]     : Oyuncu 1'in hazinesi
//  - board[0]     : Oyuncu 2'nin hazinesi
//
// AI KISMI (Minimax + Alpha-Beta Budama):
//  - Bilgisayar, olasi tum hamleleri belli bir "derinlige" (kac hamle ileriye
//    bakacagi) kadar simule eder, her sonuc icin bir puan hesaplar
//    (kendi hazinesi - rakip hazinesi) ve kendisi icin en iyi, rakip icin en
//    kotu sonucu veren hamleyi secer (minimax).
//  - Alpha-beta budama, zaten daha kotu oldugu kanitlanmis dallari erken
//    keserek aramayi hizlandirir (sonuc degismez, sadece daha hizli bulunur).
//  - Ekstra tur kazanma kurali (son tas kendi hazinesine duserse tekrar oyna)
//    aramada da doğru şekilde temsil edilir: bir sonraki "hamle sahibi"
//    applyMove()'un dondurdugu nextPlayer'a gore belirlenir, illa sira
//    degismez.
// -----------------------------------------------------------------

#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <limits>

using namespace std;

// ---------------------- Ortak yardimci fonksiyonlar ----------------------

int ownStoreIdx(int player) { return player == 1 ? 7 : 0; }
int oppStoreIdx(int player) { return player == 1 ? 0 : 7; }

bool isOwnPit(int player, int idx) {
    return player == 1 ? (idx >= 1 && idx <= 6) : (idx >= 8 && idx <= 13);
}

int oppositePit(int idx) { return 14 - idx; }

struct MoveResult {
    int nextPlayer;
    bool gameOver;
};

// Tek bir hamleyi verilen board uzerinde uygular (taslari dagitir, capture ve
// ekstra tur kurallarini isletir, oyun bitti mi kontrol eder). Hem gercek
// oyun hem de AI aramasi bu AYNI fonksiyonu kullanir; boylece kurallar iki
// yerde de birebir tutarli olur.
MoveResult applyMove(vector<int>& board, int player, int index) {
    int stones = board[index];
    board[index] = 0;
    int oppStore = oppStoreIdx(player);
    int ownSt = ownStoreIdx(player);
    int idx = index;
    int lastIndex = index;

    while (stones > 0) {
        idx = (idx + 1) % 14;
        if (idx == oppStore) continue; // rakibin hazinesi atlanir
        board[idx]++;
        stones--;
        lastIndex = idx;
    }

    bool extraTurn = false;
    if (lastIndex == ownSt) {
        extraTurn = true;
    } else if (isOwnPit(player, lastIndex) && board[lastIndex] == 1) {
        int opp = oppositePit(lastIndex);
        if (board[opp] > 0) {
            board[ownSt] += board[opp] + board[lastIndex];
            board[opp] = 0;
            board[lastIndex] = 0;
        }
    }

    bool p1Empty = true;
    for (int i = 1; i <= 6; ++i) if (board[i] > 0) { p1Empty = false; break; }
    bool p2Empty = true;
    for (int i = 8; i <= 13; ++i) if (board[i] > 0) { p2Empty = false; break; }

    bool over = false;
    if (p1Empty || p2Empty) {
        for (int i = 1; i <= 6; ++i) { board[7] += board[i]; board[i] = 0; }
        for (int i = 8; i <= 13; ++i) { board[0] += board[i]; board[i] = 0; }
        over = true;
    }

    int nextPlayer = (extraTurn && !over) ? player : (player == 1 ? 2 : 1);
    return {nextPlayer, over};
}

vector<int> getValidMoves(const vector<int>& board, int player) {
    vector<int> moves;
    int lo = player == 1 ? 1 : 8, hi = player == 1 ? 6 : 13;
    for (int i = lo; i <= hi; ++i) if (board[i] > 0) moves.push_back(i);
    return moves;
}

// ------------------------------- AI (Minimax) -------------------------------

// Basit degerlendirme fonksiyonu: AI'nin hazinesi - rakibin hazinesi.
// Oyun bitmemisse bile "su an bu board AI icin ne kadar iyi" sorusuna cevap verir.
int evaluate(const vector<int>& board, int aiPlayer) {
    int other = (aiPlayer == 1) ? 2 : 1;
    return board[ownStoreIdx(aiPlayer)] - board[ownStoreIdx(other)];
}

int minimax(vector<int> board, int player, int depth, int alpha, int beta, int aiPlayer) {
    vector<int> moves = getValidMoves(board, player);

    bool p1Empty = true;
    for (int i = 1; i <= 6; ++i) if (board[i] > 0) { p1Empty = false; break; }
    bool p2Empty = true;
    for (int i = 8; i <= 13; ++i) if (board[i] > 0) { p2Empty = false; break; }

    if (depth == 0 || moves.empty() || p1Empty || p2Empty) {
        return evaluate(board, aiPlayer);
    }

    bool maximizing = (player == aiPlayer);
    int best = maximizing ? INT_MIN : INT_MAX;

    for (int m : moves) {
        vector<int> nb = board;
        MoveResult res = applyMove(nb, player, m);
        int val = res.gameOver ? evaluate(nb, aiPlayer)
                                : minimax(nb, res.nextPlayer, depth - 1, alpha, beta, aiPlayer);
        if (maximizing) {
            best = max(best, val);
            alpha = max(alpha, best);
        } else {
            best = min(best, val);
            beta = min(beta, best);
        }
        if (beta <= alpha) break; // alpha-beta budama
    }
    return best;
}

// Kok seviyesinde: her olasi hamleyi dener, en iyi puani veren kuyuyu dondurur.
int chooseAIMove(const vector<int>& board, int aiPlayer, int depth) {
    vector<int> moves = getValidMoves(board, aiPlayer);
    int bestMove = moves.front();
    int bestVal = INT_MIN;
    int alpha = INT_MIN, beta = INT_MAX;

    for (int m : moves) {
        vector<int> nb = board;
        MoveResult res = applyMove(nb, aiPlayer, m);
        int val = res.gameOver ? evaluate(nb, aiPlayer)
                                : minimax(nb, res.nextPlayer, depth - 1, alpha, beta, aiPlayer);
        if (val > bestVal) {
            bestVal = val;
            bestMove = m;
        }
        alpha = max(alpha, bestVal);
    }
    return bestMove;
}

// -------------------------------- Oyun Sinifi --------------------------------

class MangalaGame {
private:
    vector<int> board;
    int currentPlayer;
    bool gameOver;

public:
    MangalaGame() : board(14, 4), currentPlayer(1), gameOver(false) {
        board[0] = board[7] = 0;
    }

    void printBoard() const {
        cout << "\n      ";
        for (int i = 13; i >= 8; --i) cout << board[i] << "   ";
        cout << "\n " << board[0] << "                             " << board[7] << "\n      ";
        for (int i = 1; i <= 6; ++i) cout << board[i] << "   ";
        cout << "\n\n";
    }

    int getCurrentPlayer() const { return currentPlayer; }
    bool isOver() const { return gameOver; }
    int getStore(int player) const { return board[ownStoreIdx(player)]; }
    const vector<int>& getBoard() const { return board; }

    bool isValidMove(int index) const {
        if (!isOwnPit(currentPlayer, index)) return false;
        if (board[index] == 0) return false;
        return true;
    }

    bool makeMove(int index) {
        if (!isValidMove(index)) {
            cout << "Gecersiz hamle! Kendi kuyularinizdan dolu birini seciniz.\n";
            return false;
        }
        MoveResult res = applyMove(board, currentPlayer, index);
        currentPlayer = res.nextPlayer;
        gameOver = res.gameOver;
        return true;
    }

    int getWinner() const {
        if (!gameOver) return 0;
        if (board[7] > board[0]) return 1;
        if (board[0] > board[7]) return 2;
        return 3;
    }
};

// ----------------------------------- main -----------------------------------

int main() {
    cout.setf(ios::unitbuf); // her yazmadan sonra otomatik flush et
    cout << "======================================\n";
    cout << "        MANGALA\n";
    cout << "======================================\n";
    cout << "1) Iki Kisilik (Local)\n";
    cout << "2) Insan (Oyuncu 1) vs Bilgisayar (Oyuncu 2)\n";
    cout << "Seciminiz: ";

    int modeChoice;
    cin >> modeChoice;
    bool vsAI = (modeChoice == 2);
    int aiPlayer = 2;
    int aiDepth = 9; // varsayilan: Orta

    if (vsAI) {
        cout << "\nZorluk sec:\n";
        cout << "1) Kolay\n2) Orta\n3) Zor\n";
        cout << "Seciminiz: ";
        int diff;
        cin >> diff;
        if (diff == 1) aiDepth = 5;
        else if (diff == 3) aiDepth = 12;
        else aiDepth = 9;
    }

    MangalaGame game;

    while (!game.isOver()) {
        cout << "--------------------------------------\n";
        cout << "Oyuncu " << game.getCurrentPlayer() << "'in sirasi.\n";
        game.printBoard();

        bool isAITurn = vsAI && (game.getCurrentPlayer() == aiPlayer);

        if (isAITurn) {
            cout << "Bilgisayar dusunuyor...\n";
            int move = chooseAIMove(game.getBoard(), aiPlayer, aiDepth);
            cout << "Bilgisayar " << move << " numarali kuyuyu oynadi.\n";
            game.makeMove(move);
        } else {
            int rangeLow = (game.getCurrentPlayer() == 1) ? 1 : 8;
            int rangeHigh = (game.getCurrentPlayer() == 1) ? 6 : 13;

            cout << "Oynamak istediginiz kuyuyu seciniz (" << rangeLow << "-" << rangeHigh << "): ";
            int move;
            cin >> move;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Gecersiz giris! Tekrar deneyin.\n";
                continue;
            }
            game.makeMove(move);
        }
    }

    game.printBoard();
    int winner = game.getWinner();
    cout << "======================================\n";
    if (winner == 3) {
        cout << "Oyun berabere bitti!\n";
    } else {
        cout << "Oyuncu " << winner << " kazandi! (" << game.getStore(winner) << " tas)\n";
    }
    cout << "======================================\n";

    return 0;
}
