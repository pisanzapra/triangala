// Mangala - Ozellik Tabanli TD-Ogrenme Ajani (SON SURUM)
// -----------------------------------------------------------------
// KULLANIM:
//   g++ -std=c++17 -O2 -o mangala_td mangala_td.cpp
//
//   ./mangala_td train 300000     -> 300.000 self-play oyunuyla egitir,
//                                     weights.txt varsa YUKLEYIP devam eder,
//                                     sonunda tekrar weights.txt'e kaydeder.
//   ./mangala_td play             -> weights.txt'i yukler, sana karsi oynar.
//
// NASIL CALISIYOR (ozet):
//   - Board, "sirasi gelen oyuncunun bakis acisiyla" 12 sayilik bir durum
//     olarak goruluyor (once ham tabloyu denedik, durum uzayi cok buyuk
//     oldugu icin ogrenemedi - bkz. asagidaki not). Bu durumdan 17 sayilik
//     bir "ozellik vektoru" (phi) cikariyoruz.
//   - V(durum) = w . phi(durum)  (dogrusal deger fonksiyonu)
//   - Her hamlede: olasi her secenegi 1 adim simule edip hangisinin en
//     iyi V sonucunu verdigine bakarak hamle seciliyor (kucuk ihtimalle
//     rastgele = kesif).
//   - TD guncellemesi: w += alpha * (hedef - V(durum)) * phi(durum)
//     hedef = bu hamledeki anlik odul + (sira hala bendeyse) gamma*V(sonraki)
//                                       (sira rakipteyse) - gamma*V(sonraki)
//     (Sıra rakibe geçtiğinde eksi işaret kullanılır çünkü rakibin en iyi
//      hamlesi bizim için en kötüsüdür - sıfır toplamlı oyun, negamax mantığı.)
// -----------------------------------------------------------------

#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <climits>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace std;

// ---------------------- Oyun kurallari ----------------------

int ownStoreIdx(int player) { return player == 1 ? 7 : 0; }
int oppStoreIdx(int player) { return player == 1 ? 0 : 7; }
bool isOwnPit(int player, int idx) { return player == 1 ? (idx >= 1 && idx <= 6) : (idx >= 8 && idx <= 13); }
int oppositePit(int idx) { return 14 - idx; }
int otherPlayer(int player) { return player == 1 ? 2 : 1; }

struct MoveResult { int nextPlayer; bool gameOver; };

MoveResult applyMove(vector<int>& board, int player, int index) {
    int stones = board[index];
    board[index] = 0;
    int oppStore = oppStoreIdx(player);
    int ownSt = ownStoreIdx(player);
    int idx = index, lastIndex = index;
    while (stones > 0) {
        idx = (idx + 1) % 14;
        if (idx == oppStore) continue;
        board[idx]++; stones--; lastIndex = idx;
    }
    bool extraTurn = false;
    if (lastIndex == ownSt) extraTurn = true;
    else if (isOwnPit(player, lastIndex) && board[lastIndex] == 1) {
        int opp = oppositePit(lastIndex);
        if (board[opp] > 0) {
            board[ownSt] += board[opp] + board[lastIndex];
            board[opp] = 0; board[lastIndex] = 0;
        }
    }
    bool p1Empty = true; for (int i=1;i<=6;++i) if (board[i]>0){p1Empty=false;break;}
    bool p2Empty = true; for (int i=8;i<=13;++i) if (board[i]>0){p2Empty=false;break;}
    bool over = false;
    if (p1Empty || p2Empty) {
        for (int i=1;i<=6;++i){board[7]+=board[i];board[i]=0;}
        for (int i=8;i<=13;++i){board[0]+=board[i];board[i]=0;}
        over = true;
    }
    int nextPlayer = (extraTurn && !over) ? player : otherPlayer(player);
    return {nextPlayer, over};
}

vector<int> getValidMoves(const vector<int>& board, int player) {
    vector<int> moves;
    int lo = player==1?1:8, hi = player==1?6:13;
    for (int i=lo;i<=hi;++i) if (board[i]>0) moves.push_back(i);
    return moves;
}

void printBoard(const vector<int>& board) {
    cout << "\n      ";
    for (int i = 13; i >= 8; --i) cout << board[i] << "   ";
    cout << "\n " << board[0] << "                             " << board[7] << "\n      ";
    for (int i = 1; i <= 6; ++i) cout << board[i] << "   ";
    cout << "\n\n";
}

// --- Referans rakip (test/egitim degerlendirmesi icin): onceki minimax ---
int evaluateMM(const vector<int>& board, int aiPlayer) {
    int other = otherPlayer(aiPlayer);
    return board[ownStoreIdx(aiPlayer)] - board[ownStoreIdx(other)];
}
int minimax(vector<int> board, int player, int depth, int alpha, int beta, int aiPlayer) {
    vector<int> moves = getValidMoves(board, player);
    bool p1Empty = true; for (int i=1;i<=6;++i) if (board[i]>0){p1Empty=false;break;}
    bool p2Empty = true; for (int i=8;i<=13;++i) if (board[i]>0){p2Empty=false;break;}
    if (depth == 0 || moves.empty() || p1Empty || p2Empty) return evaluateMM(board, aiPlayer);
    bool maximizing = (player == aiPlayer);
    int best = maximizing ? INT_MIN : INT_MAX;
    for (int m : moves) {
        vector<int> nb = board;
        MoveResult res = applyMove(nb, player, m);
        int val = res.gameOver ? evaluateMM(nb, aiPlayer) : minimax(nb, res.nextPlayer, depth-1, alpha, beta, aiPlayer);
        if (maximizing) { best = max(best,val); alpha = max(alpha,best); }
        else { best = min(best,val); beta = min(beta,best); }
        if (beta <= alpha) break;
    }
    return best;
}
int chooseMinimaxMove(const vector<int>& board, int aiPlayer, int depth) {
    vector<int> moves = getValidMoves(board, aiPlayer);
    int bestMove = moves.front(), bestVal = INT_MIN;
    int alpha = INT_MIN, beta = INT_MAX;
    for (int m : moves) {
        vector<int> nb = board;
        MoveResult res = applyMove(nb, aiPlayer, m);
        int val = res.gameOver ? evaluateMM(nb, aiPlayer) : minimax(nb, res.nextPlayer, depth-1, alpha, beta, aiPlayer);
        if (val > bestVal) { bestVal = val; bestMove = m; }
        alpha = max(alpha, bestVal);
    }
    return bestMove;
}

// ---------------------------- OGRENILEN DEGER FONKSIYONU ----------------------------

const int NUM_FEATURES = 17;

array<int,12> canonicalPits(const vector<int>& board, int player) {
    array<int,12> p;
    if (player == 1) {
        for (int i = 0; i < 6; ++i) p[i] = board[1+i];
        for (int i = 0; i < 6; ++i) p[6+i] = board[8+i];
    } else {
        for (int i = 0; i < 6; ++i) p[i] = board[8+i];
        for (int i = 0; i < 6; ++i) p[6+i] = board[1+i];
    }
    return p;
}

array<double, NUM_FEATURES> extractFeatures(const array<int,12>& p) {
    array<double, NUM_FEATURES> f{};
    f[0] = 1.0;
    for (int i = 0; i < 6; ++i) f[1+i] = p[i];
    for (int i = 0; i < 6; ++i) f[7+i] = p[6+i];
    int myEmpty = 0, oppEmpty = 0;
    double myThreat = 0, oppThreat = 0;
    for (int k = 0; k < 6; ++k) {
        int mirror = 5 - k;
        if (p[k] == 0) { myEmpty++; myThreat += p[6 + mirror]; }
        if (p[6+k] == 0) { oppEmpty++; oppThreat += p[mirror]; }
    }
    f[13] = myEmpty; f[14] = oppEmpty; f[15] = myThreat; f[16] = oppThreat;
    return f;
}

array<double, NUM_FEATURES> weights{};

double V(const array<double, NUM_FEATURES>& phi) {
    double s = 0;
    for (int i = 0; i < NUM_FEATURES; ++i) s += weights[i] * phi[i];
    return s;
}

double ALPHA = 0.001;
double GAMMA = 0.98;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

struct MoveChoice { int index; array<double, NUM_FEATURES> phiState; double target; };

MoveChoice pickMove(const vector<int>& board, int player, double epsilon, bool training) {
    vector<int> moves = getValidMoves(board, player);
    array<double, NUM_FEATURES> phiState = extractFeatures(canonicalPits(board, player));

    uniform_real_distribution<double> coin(0.0, 1.0);
    if (training && coin(rng) < epsilon) {
        uniform_int_distribution<int> d(0, (int)moves.size()-1);
        int chosen = moves[d(rng)];
        vector<int> nb = board;
        int myBefore = nb[ownStoreIdx(player)], oppBefore = nb[ownStoreIdx(otherPlayer(player))];
        MoveResult res = applyMove(nb, player, chosen);
        double reward = (nb[ownStoreIdx(player)]-myBefore) - (nb[ownStoreIdx(otherPlayer(player))]-oppBefore);
        double target;
        if (res.gameOver) target = reward;
        else {
            array<double,NUM_FEATURES> phiNext = extractFeatures(canonicalPits(nb, res.nextPlayer));
            double vNext = V(phiNext);
            target = (res.nextPlayer == player) ? reward + GAMMA*vNext : reward - GAMMA*vNext;
        }
        return {chosen, phiState, target};
    }

    int bestMove = moves[0];
    double bestVal = -1e18, bestTarget = 0;
    for (int m : moves) {
        vector<int> nb = board;
        int myBefore = nb[ownStoreIdx(player)], oppBefore = nb[ownStoreIdx(otherPlayer(player))];
        MoveResult res = applyMove(nb, player, m);
        double reward = (nb[ownStoreIdx(player)]-myBefore) - (nb[ownStoreIdx(otherPlayer(player))]-oppBefore);
        double target;
        if (res.gameOver) target = reward;
        else {
            array<double,NUM_FEATURES> phiNext = extractFeatures(canonicalPits(nb, res.nextPlayer));
            double vNext = V(phiNext);
            target = (res.nextPlayer == player) ? reward + GAMMA*vNext : reward - GAMMA*vNext;
        }
        if (target > bestVal) { bestVal = target; bestMove = m; bestTarget = target; }
    }
    return {bestMove, phiState, bestTarget};
}

int playTrainingEpisode(double epsilon) {
    vector<int> board(14, 4); board[0] = board[7] = 0;
    int player = 1;
    static bool flip = false;
    flip = !flip;
    if (flip) player = 2;

    while (true) {
        vector<int> moves = getValidMoves(board, player);
        if (moves.empty()) break;
        MoveChoice mc = pickMove(board, player, epsilon, true);
        double err = mc.target - V(mc.phiState);
        for (int i = 0; i < NUM_FEATURES; ++i) weights[i] += ALPHA * err * mc.phiState[i];
        MoveResult res = applyMove(board, player, mc.index);
        if (res.gameOver) {
            if (board[7] > board[0]) return 1;
            if (board[0] > board[7]) return 2;
            return 3;
        }
        player = res.nextPlayer;
    }
    return 3;
}

int playEvalGame(int agentPlayer, int opponentType, int opponentDepth, int randomOpeningPlies) {
    vector<int> board(14,4); board[0]=board[7]=0;
    int player = 1;
    for (int i = 0; i < randomOpeningPlies; ++i) {
        vector<int> moves = getValidMoves(board, player);
        bool p1E=true; for(int k=1;k<=6;++k) if(board[k]>0){p1E=false;break;}
        bool p2E=true; for(int k=8;k<=13;++k) if(board[k]>0){p2E=false;break;}
        if (moves.empty() || p1E || p2E) break;
        uniform_int_distribution<int> d(0,(int)moves.size()-1);
        MoveResult res = applyMove(board, player, moves[d(rng)]);
        player = res.nextPlayer;
    }
    while (true) {
        vector<int> moves = getValidMoves(board, player);
        bool p1Empty=true; for(int i=1;i<=6;++i) if(board[i]>0){p1Empty=false;break;}
        bool p2Empty=true; for(int i=8;i<=13;++i) if(board[i]>0){p2Empty=false;break;}
        if (moves.empty() || p1Empty || p2Empty) {
            for (int i=1;i<=6;++i){board[7]+=board[i];board[i]=0;}
            for (int i=8;i<=13;++i){board[0]+=board[i];board[i]=0;}
            break;
        }
        int index;
        if (player == agentPlayer) index = pickMove(board, player, 0.0, false).index;
        else if (opponentType == 0) { uniform_int_distribution<int> d(0,(int)moves.size()-1); index = moves[d(rng)]; }
        else index = chooseMinimaxMove(board, player, opponentDepth);
        MoveResult res = applyMove(board, player, index);
        player = res.nextPlayer;
    }
    if (board[7] > board[0]) return 1;
    if (board[0] > board[7]) return 2;
    return 3;
}

double evalWinRateVsRandom(int numGames) {
    int wins = 0;
    for (int i = 0; i < numGames; ++i) { int seat=(i%2==0)?1:2; if (playEvalGame(seat,0,0,2)==seat) wins++; }
    return (double)wins / numGames;
}
double evalWinRateVsMinimax(int numGames, int depth) {
    int wins = 0;
    for (int i = 0; i < numGames; ++i) { int seat=(i%2==0)?1:2; if (playEvalGame(seat,1,depth,2)==seat) wins++; }
    return (double)wins / numGames;
}

bool loadWeights(const string& path) {
    ifstream in(path);
    if (!in) return false;
    for (int i = 0; i < NUM_FEATURES; ++i) if (!(in >> weights[i])) return false;
    return true;
}
void saveWeights(const string& path) {
    ofstream out(path);
    for (double w : weights) out << w << " ";
}

// ---------------------------- MODLAR ----------------------------

void runTraining(long long episodes) {
    bool loaded = loadWeights("weights.txt");
    cout << (loaded ? "Onceki agirliklar (weights.txt) yuklendi, egitime devam ediliyor.\n"
                     : "Sifirdan baslaniyor (weights.txt bulunamadi).\n");
    cout << episodes << " self-play oyunuyla egitim basliyor...\n";

    auto t0 = chrono::high_resolution_clock::now();
    double epsilonStart = 0.3, epsilonEnd = 0.02;
    long long checkpoint = max((long long)1000, episodes / 10);

    for (long long ep = 1; ep <= episodes; ++ep) {
        double frac = (double)ep / episodes;
        double epsilon = epsilonStart + (epsilonEnd - epsilonStart) * frac;
        playTrainingEpisode(epsilon);
        if (ep % checkpoint == 0) {
            double wrRandom = evalWinRateVsRandom(200);
            double wrMM3 = evalWinRateVsMinimax(60, 3);
            auto now = chrono::high_resolution_clock::now();
            double sec = chrono::duration<double>(now - t0).count();
            cout << "Episode " << ep << "/" << episodes
                 << " | Rastgele: " << (wrRandom*100) << "%"
                 << " | Minimax(d3): " << (wrMM3*100) << "%"
                 << " | sure: " << sec << "s\n";
            saveWeights("weights.txt"); // ilerlemeyi kaybetmemek icin periyodik kaydet
        }
    }
    saveWeights("weights.txt");
    cout << "\nEgitim bitti. Agirliklar weights.txt dosyasina kaydedildi.\n";
    cout << "Programi tekrar calistirip 2) secersen (weights.txt kaliciysa) veya\n";
    cout << "3) ile ayni oturumda devam edebilirsin.\n";
}

void runPlay() {
    if (!loadWeights("weights.txt")) {
        cout << "weights.txt bulunamadi! Once egitmen lazim: ./mangala_td train 300000\n";
        return;
    }
    cout << "Egitilmis ajana karsi oynuyorsun. Sen Oyuncu 1'sin.\n";
    vector<int> board(14, 4); board[0] = board[7] = 0;
    int player = 1;
    int aiPlayer = 2;

    while (true) {
        bool p1Empty=true; for(int i=1;i<=6;++i) if(board[i]>0){p1Empty=false;break;}
        bool p2Empty=true; for(int i=8;i<=13;++i) if(board[i]>0){p2Empty=false;break;}
        if (p1Empty || p2Empty) {
            for (int i=1;i<=6;++i){board[7]+=board[i];board[i]=0;}
            for (int i=8;i<=13;++i){board[0]+=board[i];board[i]=0;}
            break;
        }
        cout << "--------------------------------------\n";
        cout << "Oyuncu " << player << "'in sirasi.\n";
        printBoard(board);

        int index;
        if (player == aiPlayer) {
            cout << "Ajan dusunuyor...\n";
            index = pickMove(board, player, 0.0, false).index;
            cout << "Ajan " << index << " numarali kuyuyu oynadi.\n";
        } else {
            int lo = player==1?1:8, hi = player==1?6:13;
            cout << "Oynamak istediginiz kuyuyu seciniz (" << lo << "-" << hi << "): ";
            cin >> index;
            if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); cout << "Gecersiz giris!\n"; continue; }
            vector<int> valid = getValidMoves(board, player);
            if (find(valid.begin(), valid.end(), index) == valid.end()) {
                cout << "Gecersiz hamle!\n"; continue;
            }
        }
        MoveResult res = applyMove(board, player, index);
        player = res.nextPlayer;
        if (res.gameOver) break;
    }

    printBoard(board);
    cout << "======================================\n";
    if (board[7] > board[0]) cout << "Sen kazandin! (" << board[7] << " - " << board[0] << ")\n";
    else if (board[0] > board[7]) cout << "Ajan kazandi. (" << board[0] << " - " << board[7] << ")\n";
    else cout << "Berabere!\n";
    cout << "======================================\n";
}

// Interaktif oyun: weights.txt'ten YUKLEMEDEN, o an hafizadaki (yeni egitilmis)
// agirliklarla oynatir. Online derleyicilerde dosya kaliciligi olmayabilecegi
// icin, "hemen ayni oturumda oyna" secenegi dosyaya hic bagli degildir.
void runPlayInMemory() {
    cout << "\nSimdi az once egittigin ajana karsi oynayabilirsin. Sen Oyuncu 1'sin.\n";
    vector<int> board(14, 4); board[0] = board[7] = 0;
    int player = 1;
    int aiPlayer = 2;

    while (true) {
        bool p1Empty=true; for(int i=1;i<=6;++i) if(board[i]>0){p1Empty=false;break;}
        bool p2Empty=true; for(int i=8;i<=13;++i) if(board[i]>0){p2Empty=false;break;}
        if (p1Empty || p2Empty) {
            for (int i=1;i<=6;++i){board[7]+=board[i];board[i]=0;}
            for (int i=8;i<=13;++i){board[0]+=board[i];board[i]=0;}
            break;
        }
        cout << "--------------------------------------\n";
        cout << "Oyuncu " << player << "'in sirasi.\n";
        printBoard(board);

        int index;
        if (player == aiPlayer) {
            cout << "Ajan dusunuyor...\n";
            index = pickMove(board, player, 0.0, false).index;
            cout << "Ajan " << index << " numarali kuyuyu oynadi.\n";
        } else {
            int lo = player==1?1:8, hi = player==1?6:13;
            cout << "Oynamak istediginiz kuyuyu seciniz (" << lo << "-" << hi << "): ";
            cin >> index;
            if (cin.fail()) { cin.clear(); cin.ignore(10000,'\n'); cout << "Gecersiz giris!\n"; continue; }
            vector<int> valid = getValidMoves(board, player);
            if (find(valid.begin(), valid.end(), index) == valid.end()) {
                cout << "Gecersiz hamle!\n"; continue;
            }
        }
        MoveResult res = applyMove(board, player, index);
        player = res.nextPlayer;
        if (res.gameOver) break;
    }

    printBoard(board);
    cout << "======================================\n";
    if (board[7] > board[0]) cout << "Sen kazandin! (" << board[7] << " - " << board[0] << ")\n";
    else if (board[0] > board[7]) cout << "Ajan kazandi. (" << board[0] << " - " << board[7] << ")\n";
    else cout << "Berabere!\n";
    cout << "======================================\n";
}

int main() {
    cout.setf(ios::unitbuf); // her yazmadan sonra flush et (online derleyicilerde de duzgun akmasi icin)
    cout << "======================================\n";
    cout << "   MANGALA - TD-Ogrenme Ajani\n";
    cout << "======================================\n";
    cout << "Not: Komut satiri argumani GEREKMEZ - her sey asagidaki\n";
    cout << "menuden yonetiliyor (online derleyicilerde de calisir).\n\n";
    cout << "1) Egit (weights.txt varsa yukleyip devam eder, sonunda kaydeder)\n";
    cout << "2) Kayitli agirliklarla oyna (weights.txt GEREKIR)\n";
    cout << "3) Egit VE hemen ayni oturumda oyna (dosya kaliciligi gerekmez - online derleyiciler icin ONERILEN)\n";
    cout << "Seciminiz: ";

    int choice;
    cin >> choice;
    if (cin.fail()) { cout << "Gecersiz secim.\n"; return 1; }

    if (choice == 2) {
        runPlay();
        return 0;
    }

    cout << "Kac bolum (episode) egitilsin?\n";
    cout << "(Hizli test icin 20000-50000 yeterli. Guclu bir ajan icin 300000+ onerilir,\n";
    cout << " ama optimizasyonsuz/online derleyicilerde bu suresi uzun surebilir.)\n";
    cout << "Sayi giriniz: ";
    long long episodes;
    cin >> episodes;
    if (cin.fail() || episodes <= 0) { cout << "Gecersiz sayi, varsayilan 20000 kullaniliyor.\n"; cin.clear(); cin.ignore(10000,'\n'); episodes = 20000; }

    runTraining(episodes);

    if (choice == 3) {
        runPlayInMemory();
    }
    return 0;
}
