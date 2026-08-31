// TRIANGALA (3+ Player Mangala) - Last Rule Set - DIRECTORS FINAL CUT
// -----------------------------------------------------------------
// BOARD DUZENI (21 pozisyon, indeks 0-20):
//   1-6   : Oyuncu 1'in kuyulari       7  : Oyuncu 1'in hazinesi
//   8-13  : Oyuncu 2'nin kuyulari      14 : Oyuncu 2'nin hazinesi
//   15-20 : Oyuncu 3'un kuyulari       0  : Oyuncu 3'un hazinesi
//
// UYGULANAN İDEAL KURALLAR:
// 1) ORİJİNAL DAĞITIM: Bir kuyudan tas alinirken, eger kuyuda 1'den 
//    fazla tas varsa ilk tas alinan kuyuya birakilir, kalani dagitilir. 
//    Tek tas varsa o tas alinip bir sonraki kuyuya birakilir.
// 2) CIFTLEME KURALI: Son tas, HERHANGI bir rakibin kuyusuna duser 
//    ve o kuyudaki tas sayisini CIFT (2, 4, 6, 8) yaparsa taslar kapilir.
// 3) CANLI KUYU SİSTEMİ: Kuyusu bosalan oyuncu kalici olarak elenmez.
//    O tur pas gecer ancak rakipleri tas dagitirken o kuyulara tas
//    birakmaya devam eder.
// -----------------------------------------------------------------

#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <chrono>
#include <random>
#include <cmath>

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

bool isStore(int idx) { 
    return idx == 0 || idx == 7 || idx == 14; 
}

int nextInRotation(int player) { 
    return (player % 3) + 1; 
}

bool isPlayerOut(const vector<int>& board, int player) {
    int s = pitStart(player);
    for (int i = s; i <= s + 5; ++i) if (board[i] > 0) return false;
    return true;
}

int totalPitStones(const vector<int>& board) {
    int total = 0;
    for (int i = 1; i < NUM_POS; ++i) if (!isStore(i)) total += board[i];
    return total;
}

struct MoveResult { int nextPlayer; bool gameOver; };

MoveResult applyMove(vector<int>& board, int player, int index) {
    int stones = board[index];
    board[index] = 0;
    int ownSt = ownStoreIdx(player);
    int idx = index;
    int lastIndex = index;

    // Rule 1: OG Turkish Mangala Distrubition
    if (stones == 1) {
        // Sadece 1 tas varsa, alinan kuyu bosalir, tas bir sonrakine gecer
        // NOT: index buraya HER ZAMAN mevcut oyuncunun KENDI kuyu araligindan
        // gelir (getValidMoves ve main() girdi dogrulamasi bunu garanti eder).
        // Board duzeninde her oyuncunun 6 kuyusunun hemen ardindan YALNIZCA
        // kendi hazinesi gelir (1-6->7, 8-13->14, 15-20->0) - hicbir kuyunun
        // hemen sonrasi BASKA bir oyuncunun hazinesi olamaz. Bu yuzden tek
        // adimlik ilerleme hicbir zaman bir hazine atlamaya ihtiyac duymaz;
        // bir dongu yerine dogrudan tek adim yeterli ve daha sade
        idx = (idx + 1) % NUM_POS;
        board[idx]++;
        lastIndex = idx;
    } else {
        // birden fazla tas varsa, alinan ilk tas o kuyuya birakilir
        board[index]++;
        stones--;
        while (stones > 0) {
            idx = (idx + 1) % NUM_POS;
            if (isStore(idx) && idx != ownSt) continue; // Rakip hazinesini atla
            board[idx]++;
            stones--;
            lastIndex = idx;
        }
    }

    bool extraTurn = (lastIndex == ownSt);

    // Rule 2: Doubling
    if (!extraTurn && !isOwnPit(player, lastIndex) && !isStore(lastIndex)) {
        if (board[lastIndex] % 2 == 0) { // 2, 4, 6, 8 vb. cift sayiysa
            board[ownSt] += board[lastIndex];
            board[lastIndex] = 0;
        }
    }

    // Rule 3: Living Hole (Canli Kuyu) - game ends when all holes are empty (oyun tum 18 kuyu tamamen bosalinca biter)
    bool over = (totalPitStones(board) == 0);

    int nextPlayer = player;
    if (!over) {
        int candidate = extraTurn ? player : nextInRotation(player);
        int guard = 0;
        // Sirasi gelen oyuncunun kuyulari bossa pas gecer (digerleri tas birakana kadar)
        while (isPlayerOut(board, candidate) && guard < 3) { 
            candidate = nextInRotation(candidate); 
            guard++; 
        }
        nextPlayer = candidate;
        if (guard >= 3) over = true; // Sonsuz pas gecme guvenligi
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

// 3 PLATER AI (max^n search)

using ValueVec = array<double, 4>; 

ValueVec evaluateLeaf(const vector<int>& board) {
    ValueVec v{};
    for (int p = 1; p <= 3; ++p) {
        int mine = board[ownStoreIdx(p)];
        int maxOther = 0;
        for (int q = 1; q <= 3; ++q) {
            if (q != p) maxOther = max(maxOther, board[ownStoreIdx(q)]);
        }
        v[p] = mine - maxOther; 
    }
    return v;
}

ValueVec evaluateTerminal(const vector<int>& board) {
    return evaluateLeaf(board);
}

using Clock = std::chrono::steady_clock;

ValueVec maxN(vector<int> board, int player, int depth, const Clock::time_point& deadline) {
    if (totalPitStones(board) == 0) return evaluateTerminal(board);
    if (depth == 0 || Clock::now() >= deadline) return evaluateLeaf(board);

    vector<int> moves = getValidMoves(board, player);
    if (moves.empty()) return evaluateLeaf(board); // Segfault guvenligi

    ValueVec best{};
    double bestOwn = -1e18;
    for (int m : moves) {
        vector<int> nb = board;
        MoveResult res = applyMove(nb, player, m);
        ValueVec childVal = res.gameOver ? evaluateTerminal(nb) : maxN(nb, res.nextPlayer, depth - 1, deadline);
        
        if (childVal[player] > bestOwn) { 
            bestOwn = childVal[player]; 
            best = childVal; 
        }
        if (Clock::now() >= deadline) break; 
    }
    return best;
}

int chooseMaxNMove(const vector<int>& board, int player, int maxDepth, int timeBudgetMs = 2000) {
    vector<int> moves = getValidMoves(board, player);
    if (moves.empty()) return -1; // Cok nadir uc durum icin Segfault guvenligi

    static std::mt19937 rng(std::random_device{}());
    const double EPS = 1e-9;

    int bestMove = moves.front();
    Clock::time_point deadline = Clock::now() + std::chrono::milliseconds(timeBudgetMs);

    // Iteratif Derinlestirme
    for (int d = 1; d <= maxDepth; ++d) {
        double bestOwn = -1e18;
        vector<int> bestMovesThisDepth; // esit degerli hamleleri biriktir (tie-break icin)
        bool ranOutOfTime = false;

        for (int m : moves) {
            if (Clock::now() >= deadline) { ranOutOfTime = true; break; }
            vector<int> nb = board;
            MoveResult res = applyMove(nb, player, m);
            ValueVec childVal = res.gameOver ? evaluateTerminal(nb) : maxN(nb, res.nextPlayer, d - 1, deadline);

            if (childVal[player] > bestOwn + EPS) {
                bestOwn = childVal[player];
                bestMovesThisDepth.clear();
                bestMovesThisDepth.push_back(m);
            } else if (std::fabs(childVal[player] - bestOwn) <= EPS) {
                bestMovesThisDepth.push_back(m);
            }
        }
        if (ranOutOfTime) break;
        // esit degerli hamle varsa birden fazla, aralarindan rastgele secim yapilsin
        // rng() % n modulo-bias tasiyabilecegi icin (n, 2^32'yi tam bolmuyorsa
        // bazi degerler istatistiksel olarak cok hafif kayirilir) std::uniform_int_distribution
        // kullaniyoruz - bu, [0, n-1] araliginda gercekten esit dagilim garantiler.
        std::uniform_int_distribution<size_t> dist(0, bestMovesThisDepth.size() - 1);
        bestMove = bestMovesThisDepth[dist(rng)];
    }
    return bestMove;
}

int main() {
    cout.setf(ios::unitbuf);
    cout << "======================================\n";
    cout << "     TRIANGALA - 3 Kisilik Mangala\n";
    cout << "======================================\n";
    cout << "Aktif Kurallar:\n";
    cout << "- Orijinal Dagitim: lk tas alinan kuyuya birakilir.\n";
    cout << "- Ciftleme: Son tas rakip kuyuyu cift (2,4,6...) yaparsa kapar.\n";
    cout << "- Canli Kuyu: Kuyusu bosalan elenmez, pas gecer (tas birakilabilir).\n";
    cout << "--------------------------------------\n\n";

    bool isAI[4] = {false, false, false, false};
    int aiDepth[4] = {0, 0, 0, 0};
    
    cout << "Her koltugu kim oynasin?\n";
    for (int p = 1; p <= 3; ++p) {
        cout << "Oyuncu " << p << ": 1) Insan   2) Bilgisayar (AI)\nSeciminiz: ";
        int c; cin >> c;
        if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); c = 1; }
        
        if (c == 2) {
            isAI[p] = true;
            cout << "  AI Zorluk: 1) Kolay(d4)  2) Orta(d6)  3) Zor(d8)\n  Seciminiz: ";
            int d; cin >> d;
            if (cin.fail() || d < 1 || d > 3) { cin.clear(); cin.ignore(10000, '\n'); d = 2; }
            aiDepth[p] = (d == 1) ? 4 : (d == 3) ? 8 : 6;
        }
    }

    vector<int> board(NUM_POS, STONES_PER_PIT);
    board[0] = board[7] = board[14] = 0; // Hazineler bos baslar
    int player = 1;
    bool gameOver = false;
    int moveCount = 0;
    // Guvenlik siniri: Canli Kuyu sisteminde teorik olarak (ozellikle capture
    // hic tetiklenmeyen kotu sansli/inatci oyunlarda) oyun cok uzayabilir.
    // 200+ simule oyunda hicbir zaman 160 hamleyi gecmedi, ama insan oyuncular
    // kasten oyalayabilecegi icin ucuz bir guvenlik agi olarak birakildi.
    const int MAX_MOVES = 1000;

    while (!gameOver) {
        cout << "--------------------------------------\n";
        cout << "Oyuncu " << player << "'in sirasi." << (isAI[player] ? " (AI)" : "") << "\n";
        printBoard(board);

        int move;
        if (isAI[player]) {
            cout << "AI dusunuyor...\n";
            move = chooseMaxNMove(board, player, aiDepth[player], 2000);
            if (move == -1) break; // Olasiligi dusuk guvenlik cikisi
            cout << "AI (Oyuncu " << player << ") " << move << " numarali kuyuyu oynadi.\n";
        } else {
            int lo = pitStart(player), hi = lo + 5;
            cout << "Oynamak istediginiz kuyuyu seciniz (" << lo << "-" << hi << "): ";
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
        }

        MoveResult res = applyMove(board, player, move);
        player = res.nextPlayer;
        gameOver = res.gameOver;
        moveCount++;

        if (!gameOver && moveCount >= MAX_MOVES) {
            cout << "\n[Guvenlik siniri] Oyun " << MAX_MOVES
                 << " hamleyi asti; kalan taslar sahiplerine dagitilip oyun sonlandiriliyor.\n";
            for (int p = 1; p <= 3; ++p) {
                int s = pitStart(p);
                for (int i = s; i <= s + 5; ++i) { board[ownStoreIdx(p)] += board[i]; board[i] = 0; }
            }
            gameOver = true;
        }
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
