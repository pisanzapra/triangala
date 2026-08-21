// Mangala (Mancala/Kalah) - 3 KISILIK Konsol Oyunu
// -----------------------------------------------------------------
// BOARD DUZENI (21 pozisyon, indeks 0-20):
//   1-6   : Oyuncu 1'in kuyulari       7  : Oyuncu 1'in hazinesi
//   8-13  : Oyuncu 2'nin kuyulari      14 : Oyuncu 2'nin hazinesi
//   15-20 : Oyuncu 3'un kuyulari       0  : Oyuncu 3'un hazinesi
// (2 kisilik versiyondaki "0 = son oyuncunun hazinesi, dongu ondan
//  once biter" yapisi burada da aynen korunuyor, sadece 3 oyuncuya
//  genisletildi.)
//
// KURAL FARKLARI (2 kisilikten 3 kisiye genisletirken alinan kararlar):
//
// 1) TAS KAPMA (capture) - "karsi kuyu" kavrami: 18 kuyuyu (hazineler
//    haric) bir cember gibi dusunup, bir kuyunun "tam karsisini" 9 kuyu
//    ileride tanimliyoruz (18/2=9). Bu, her kuyuyu iki rakipten TAM
//    OLARAK BIRINE ait belirli bir kuyuyla eslestiriyor - matematiksel
//    olarak net tanimli, ama simetrik hissetmeyebilir (bir oyuncunun
//    kuyularinin bir kismi bir rakibe, kalani digerine karsi dusuyor).
//
// 2) OYUN BITISI - ELEME SISTEMI: 2 kisilikte kural basitti (bir
//    tarafin kuyulari bosaldi mi oyun biter). 3 kisilikte bunun yerine:
//    bir oyuncunun TUM kuyulari boşaldiginda o oyuncu ELENIR (sirasi
//    atlanir, kuyulari artik hicbir sekilde tas almaz - taşlar zaten
//    hep 0 oldugu icin bu, "kuyularini oyunun geri kalaninda cemberden
//    cikarmak" ile ayni anlama gelir). Oyun, SADECE BIR oyuncu tas
//    sahibi kalana kadar devam eder; o oyuncu kendi kuyularindaki
//    taslari kendi hazinesine supurur ve oyun biter. Kazanan = en
//    yuksek hazineye sahip oyuncu.
// -----------------------------------------------------------------

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int NUM_POS = 21;
const int STONES_PER_PIT = 4;

int ownStoreIdx(int player) {
    if (player == 1) return 7;
    if (player == 2) return 14;
    return 0; // player 3
}

int pitStart(int player) {
    if (player == 1) return 1;
    if (player == 2) return 8;
    return 15;
}

bool isOwnPit(int player, int idx) {
    int s = pitStart(player);
    return idx >= s && idx <= s + 5;
}

int pitOwner(int idx) {
    // Yalnizca gercek bir kuyu (store degil) icin cagrilmali.
    if (idx >= 1 && idx <= 6) return 1;
    if (idx >= 8 && idx <= 13) return 2;
    if (idx >= 15 && idx <= 20) return 3;
    return -1; // store
}

bool isStore(int idx) { return idx == 0 || idx == 7 || idx == 14; }

int nextInRotation(int player) { return (player % 3) + 1; }

// Bir kuyunun 18-kuyuluk cember uzerindeki "tam karsisi"ni bulur (9 kuyu ileride).
int oppositePit(int idx) {
    int owner = pitOwner(idx);
    int localIdx = idx - pitStart(owner);           // 0-5
    int pitOnly = (owner - 1) * 6 + localIdx;        // 0-17
    int oppPitOnly = (pitOnly + 9) % 18;
    int oppOwner = oppPitOnly / 6 + 1;
    int oppLocal = oppPitOnly % 6;
    return pitStart(oppOwner) + oppLocal;
}

// Bir oyuncunun tum kuyulari bos mu? (dinamik olarak hesaplaniyor - ayri bir
// "elenmis" bayragina gerek yok, cunku bir oyuncu bir kez 0'a dustugunde
// sowing sirasinda atlanacagi icin bir daha asla tas alamiyor, yani bu kontrol
// kalici bir "eleme" ile ayni sonucu veriyor.)
bool isPlayerOut(const vector<int>& board, int player) {
    int s = pitStart(player);
    for (int i = s; i <= s + 5; ++i) if (board[i] > 0) return false;
    return true;
}

int countActivePlayers(const vector<int>& board) {
    int c = 0;
    for (int p = 1; p <= 3; ++p) if (!isPlayerOut(board, p)) c++;
    return c;
}

struct MoveResult { int nextPlayer; bool gameOver; };

MoveResult applyMove(vector<int>& board, int player, int index) {
    int stones = board[index];
    board[index] = 0;
    int ownSt = ownStoreIdx(player);
    int idx = index;
    int lastIndex = index;

    while (stones > 0) {
        idx = (idx + 1) % NUM_POS;
        if (isStore(idx) && idx != ownSt) continue;            // baskasinin hazinesi - atla
        if (!isStore(idx) && isPlayerOut(board, pitOwner(idx))) continue; // elenmis oyuncunun kuyusu - atla
        board[idx]++;
        stones--;
        lastIndex = idx;
    }

    bool extraTurn = (lastIndex == ownSt);
    if (!extraTurn && isOwnPit(player, lastIndex) && board[lastIndex] == 1) {
        int opp = oppositePit(lastIndex);
        if (board[opp] > 0) {
            board[ownSt] += board[opp] + board[lastIndex];
            board[opp] = 0;
            board[lastIndex] = 0;
        }
    }

    int active = countActivePlayers(board);
    bool over = (active <= 1);
    if (over) {
        for (int p = 1; p <= 3; ++p) {
            if (!isPlayerOut(board, p)) {
                int s = pitStart(p);
                for (int i = s; i <= s + 5; ++i) { board[ownStoreIdx(p)] += board[i]; board[i] = 0; }
            }
        }
    }

    int nextPlayer = player;
    if (!over) {
        int candidate = extraTurn ? player : nextInRotation(player);
        while (isPlayerOut(board, candidate)) candidate = nextInRotation(candidate);
        nextPlayer = candidate;
    }
    return {nextPlayer, over};
}

vector<int> getValidMoves(const vector<int>& board, int player) {
    vector<int> moves;
    int s = pitStart(player);
    for (int i = s; i <= s + 5; ++i) if (board[i] > 0) moves.push_back(i);
    return moves;
}

void printBoard(const vector<int>& board) {
    cout << "\n";
    cout << "  Oyuncu 3: ";
    for (int i = 15; i <= 20; ++i) cout << board[i] << " ";
    cout << " | Hazine3: " << board[0] << "\n";
    cout << "  Oyuncu 2: ";
    for (int i = 8; i <= 13; ++i) cout << board[i] << " ";
    cout << " | Hazine2: " << board[14] << "\n";
    cout << "  Oyuncu 1: ";
    for (int i = 1; i <= 6; ++i) cout << board[i] << " ";
    cout << " | Hazine1: " << board[7] << "\n\n";
}

int main() {
    cout.setf(ios::unitbuf);
    cout << "======================================\n";
    cout << "     MANGALA - 3 Kisilik (Local)\n";
    cout << "======================================\n";
    cout << "Bir oyuncunun tum kuyulari bosalirsa o oyuncu elenir (sirasi\n";
    cout << "atlanir). Oyun, sadece bir oyuncu tas kalana kadar devam eder.\n";

    vector<int> board(NUM_POS, STONES_PER_PIT);
    board[0] = board[7] = board[14] = 0;
    int player = 1;
    bool gameOver = false;

    while (!gameOver) {
        cout << "--------------------------------------\n";
        cout << "Oyuncu " << player << "'in sirasi.\n";
        printBoard(board);

        int lo = pitStart(player), hi = lo + 5;
        cout << "Oynamak istediginiz kuyuyu seciniz (" << lo << "-" << hi << "): ";
        int move;
        cin >> move;

        if (cin.fail()) {
            cin.clear(); cin.ignore(10000, '\n');
            cout << "Gecersiz giris! Tekrar deneyin.\n";
            continue;
        }
        if (move < lo || move > hi || board[move] == 0) {
            cout << "Gecersiz hamle! Kendi kuyularinizdan dolu birini seciniz.\n";
            continue;
        }

        MoveResult res = applyMove(board, player, move);
        player = res.nextPlayer;
        gameOver = res.gameOver;
    }

    printBoard(board);
    int scores[4] = {0, board[7], board[14], board[0]};
    int best = max({scores[1], scores[2], scores[3]});
    cout << "======================================\n";
    cout << "OYUN BITTI!\n";
    for (int p = 1; p <= 3; ++p) {
        cout << "Oyuncu " << p << ": " << scores[p] << " tas" << (scores[p] == best ? "  <-- KAZANAN" : "") << "\n";
    }
    cout << "======================================\n";
    return 0;
}
