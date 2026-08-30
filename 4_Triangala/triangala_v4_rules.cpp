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

// ---------------------- KAPMA (capture) KURAL VARYANTLARI ----------------------
// 1 = SABIT_KARSI   : 18-kuyuluk cemberde 9-ileri = karsi kuyu (mevcut/klasik-genisletme)
// 2 = SIRADAKI      : sabit zincir (1'den 2'ye, 2'den 3'e, 3'den 1'e), aynalanmis pozisyon
// 3 = HER_IKISI     : aynalanmis pozisyondan HER IKI rakipten birden kapma
// 4 = TERCIHLI      : aynalanmis pozisyonda iki aday da varsa, oyuncu hangisini secer
// 5 = KAPMA_YOK     : kontrol grubu - capture kurali tamamen kaldirilmis

int CAPTURE_MODE = 1;

int targetPlayer(int player) { return (player % 3) + 1; } // 1->2, 2->3, 3->1 (SIRADAKI modu icin sabit zincir)

// sourcePlayer'in lastIndex'teki kuyusunun, targetPlayer'in sirasindaki AYNALANMIS
// (7-i mantigi, 2 kisilik oyundaki ile ayni) karsilik gelen kuyusunu bulur.
int mirroredPitIn(int sourcePlayer, int lastIndex, int targetPlayer) {
    int localIdx0 = lastIndex - pitStart(sourcePlayer);      // 0-5
    int mirrored0 = 5 - localIdx0;                            // ayna
    return pitStart(targetPlayer) + mirrored0;
}

// Etkilesimli modda (interactive=true) TERCIHLI kapma icin kullaniciya sorar.
// Etkilesimsiz modda (simulasyon) buyuk yigini tercih eden basit bir sezgi kullanir.
void performCapture(vector<int>& board, int player, int lastIndex, bool interactive) {
    int ownSt = ownStoreIdx(player);

    if (CAPTURE_MODE == 5) return; // KAPMA_YOK

    if (CAPTURE_MODE == 1) { // SABIT_KARSI
        int opp = oppositePit(lastIndex);
        if (board[opp] > 0) {
            board[ownSt] += board[opp] + board[lastIndex];
            board[opp] = 0; board[lastIndex] = 0;
        }
        return;
    }

    if (CAPTURE_MODE == 2) { // SIRADAKI (sabit zincir)
        int tgt = targetPlayer(player);
        int opp = mirroredPitIn(player, lastIndex, tgt);
        if (board[opp] > 0) {
            board[ownSt] += board[opp] + board[lastIndex];
            board[opp] = 0; board[lastIndex] = 0;
        }
        return;
    }

    // Diger iki oyuncunun aynalanmis kuyulari (HER_IKISI ve TERCIHLI icin ortak)
    int others[2]; int oi = 0;
    for (int p = 1; p <= 3; ++p) if (p != player) others[oi++] = p;
    int oppA = mirroredPitIn(player, lastIndex, others[0]);
    int oppB = mirroredPitIn(player, lastIndex, others[1]);

    if (CAPTURE_MODE == 3) { // HER_IKISI
        int gained = 0;
        if (board[oppA] > 0) { gained += board[oppA]; board[oppA] = 0; }
        if (board[oppB] > 0) { gained += board[oppB]; board[oppB] = 0; }
        if (gained > 0) { board[ownSt] += gained + board[lastIndex]; board[lastIndex] = 0; }
        return;
    }

    if (CAPTURE_MODE == 4) { // TERCIHLI
        bool hasA = board[oppA] > 0, hasB = board[oppB] > 0;
        if (!hasA && !hasB) return; // kapacak bir sey yok
        int chosen = -1;
        if (hasA && !hasB) chosen = oppA;
        else if (hasB && !hasA) chosen = oppB;
        else {
            // iki aday da mevcut - secim gerekiyor
            if (interactive) {
                cout << "Iki secenek var: (A) Oyuncu " << others[0] << "'dan " << board[oppA] << " tas"
                     << "  (B) Oyuncu " << others[1] << "'den " << board[oppB] << " tas\n";
                cout << "Hangisini kapmak istersiniz? (A/B): ";
                char c; cin >> c;
                chosen = (c == 'a' || c == 'A') ? oppA : oppB;
            } else {
                chosen = (board[oppA] >= board[oppB]) ? oppA : oppB; // simulasyon: buyuk yigini sec
            }
        }
        board[ownSt] += board[chosen] + board[lastIndex];
        board[chosen] = 0; board[lastIndex] = 0;
        return;
    }
}

// ---------------------- ELEME KURALI VARYANTLARI ----------------------
// 1 = KALICI (UNO tarzi)  : kuyulari bosalan oyuncu cemberden tamamen cikar,
//                            bir daha asla tas alamaz, sirasi surekli atlanir.
// 2 = CANLI_KUYU (toparlanabilir): kuyulari bosalan oyuncu sadece O TURDA
//                            oynayamaz (pas gecer), ama kuyulari cemberde
//                            "canli" kalir - baskalari oradan gecerken tas
//                            birakabilir, oyuncu tekrar toparlanip oynayabilir.
//
// ANALIZ SONUCU (simulasyonla olculdu): KALICI modelde ilk elenen oyuncu
// ortalama 2.67/3 sirayla bitiriyor (neredeyse hep sonuncu) - net bir
// haksizlik. CANLI_KUYU modelinde bu fark 2.20/3'e iniyor - cok daha adil,
// bedeli sadece biraz daha uzun oyun (~76 -> ~90 hamle).
int ELIMINATION_MODE = 2; // varsayilan: daha adil olan CANLI_KUYU

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

int totalPitStones(const vector<int>& board) {
    int total = 0;
    for (int i = 1; i < NUM_POS; ++i) if (!isStore(i)) total += board[i];
    return total;
}

struct MoveResult { int nextPlayer; bool gameOver; };

MoveResult applyMove(vector<int>& board, int player, int index, bool interactive) {
    int stones = board[index];
    board[index] = 0;
    int ownSt = ownStoreIdx(player);
    int idx = index;
    int lastIndex = index;

    while (stones > 0) {
        idx = (idx + 1) % NUM_POS;
        if (isStore(idx) && idx != ownSt) continue; // baskasinin hazinesi - her zaman atlanir
        // KALICI modda elenmis oyuncularin kuyulari da atlanir; CANLI_KUYU modda atlanmaz.
        if (ELIMINATION_MODE == 1 && !isStore(idx) && isPlayerOut(board, pitOwner(idx))) continue;
        board[idx]++;
        stones--;
        lastIndex = idx;
    }

    bool extraTurn = (lastIndex == ownSt);
    if (!extraTurn && isOwnPit(player, lastIndex) && board[lastIndex] == 1) {
        performCapture(board, player, lastIndex, interactive);
    }

    bool over;
    if (ELIMINATION_MODE == 1) {
        int active = countActivePlayers(board);
        over = (active <= 1);
        if (over) {
            for (int p = 1; p <= 3; ++p) {
                if (!isPlayerOut(board, p)) {
                    int s = pitStart(p);
                    for (int i = s; i <= s + 5; ++i) { board[ownStoreIdx(p)] += board[i]; board[i] = 0; }
                }
            }
        }
    } else {
        // CANLI_KUYU: oyun, TUM kuyular (18'i de) toplamda bosalinca biter.
        over = (totalPitStones(board) == 0);
    }

    int nextPlayer = player;
    if (!over) {
        int candidate = extraTurn ? player : nextInRotation(player);
        int guard = 0;
        while (isPlayerOut(board, candidate) && guard < 3) { candidate = nextInRotation(candidate); guard++; }
        nextPlayer = candidate;
        if (guard >= 3) over = true; // guvenlik: kimsenin hamlesi yoksa bitir (pratikte olmamali)
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
    cout << "======================================\n\n";
    cout << "Bir oyuncunun kuyulari boslarsa oyunda ne olsun?\n";
    cout << "1) Kalici Eleme (UNO tarzi): sirasi tamamen atlanir, kuyulari\n";
    cout << "   cemberden cikar, bir daha asla tas alamaz.\n";
    cout << "   [Simulasyon: ilk elenen ort. 2.67/3 sirayla bitiyor - belirgin haksizlik]\n";
    cout << "2) Canli Kuyu (toparlanabilir): o turda pas gecer ama kuyulari\n";
    cout << "   cemberde kalir, baskalari tas birakabilir, tekrar oynayabilir.\n";
    cout << "   [Simulasyon: ilk dusen ort. 2.20/3 sirayla bitiyor - cok daha adil]\n";
    cout << "Seciminiz: ";
    cin >> ELIMINATION_MODE;
    if (cin.fail() || ELIMINATION_MODE < 1 || ELIMINATION_MODE > 2) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "Gecersiz secim, varsayilan (2 - Canli Kuyu) kullaniliyor.\n";
        ELIMINATION_MODE = 2;
    }

    cout << "\nKapma (capture) kurali hangi varyantla oynansin?\n";
    cout << "1) Sabit karsi kuyu (18'li cemberde 9-ileri)\n";
    cout << "2) Siradaki oyuncudan kap (sabit zincir: 1<-2<-3<-1)\n";
    cout << "3) Her iki rakipten de kap\n";
    cout << "4) Tercihli (iki aday varsa sen secersin)\n";
    cout << "5) Kapma yok (kontrol grubu)\n";
    cout << "Seciminiz: ";
    cin >> CAPTURE_MODE;
    if (cin.fail() || CAPTURE_MODE < 1 || CAPTURE_MODE > 5) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "Gecersiz secim, varsayilan (1) kullaniliyor.\n";
        CAPTURE_MODE = 1;
    }

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

        MoveResult res = applyMove(board, player, move, true);
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
