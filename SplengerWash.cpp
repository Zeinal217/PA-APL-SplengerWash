#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <cctype>
#include <algorithm>

using namespace std;

// ═══════════════════════════════════════════════════════════
//  WARNA ANSI
// ═══════════════════════════════════════════════════════════
const string R  = "\033[0m";
const string B  = "\033[1m";
const string DM = "\033[2m";
const string IT = "\033[3m";
const string RD = "\033[31m";
const string GR = "\033[32m";
const string YL = "\033[33m";
const string BL = "\033[34m";
const string MG = "\033[35m";
const string CY = "\033[36m";
const string WH = "\033[37m";
const string BGC = "\033[46m";   // background cyan
const string BGY = "\033[43m";   // background yellow
const string BGR = "\033[41m";   // background red
const string BGG = "\033[42m";   // background green

// ═══════════════════════════════════════════════════════════
//  STRUCT DATA
// ═══════════════════════════════════════════════════════════
struct Layanan {
    int    idLayanan;
    string namaLayanan;
    string jenis;       // "Kiloan" / "Satuan"
    float  harga;       // per kg atau per pcs
    string estimasi;    // "1 Hari", "2 Hari", dll
    string tambahan;    // "Tanpa Setrika" / "Setrika + Lipat"
};

struct Pelanggan {
    int    idPelanggan;
    string nama;
    string username;
    string password;
};

struct Admin {
    string username;
    string password;
};

struct Transaksi {
    int    idTransaksi;
    int    noAntrian;
    int    idPelanggan;
    int    idLayanan;
    float  berat;        // 0 = belum ditimbang
    int    hargaTotal;   // 0 = belum dihitung
    string status;       // "Menunggu" / "Diproses" / "Selesai" / "Dibatalkan"
    string statusBayar;  // "Belum Bayar" / "Lunas"
    string tglOrder;
    string tglSelesai;
};

// ═══════════════════════════════════════════════════════════
//  KONSTANTA & DATA GLOBAL
// ═══════════════════════════════════════════════════════════
const int MAKS_LAYANAN    = 20;
const int MAKS_PELANGGAN  = 50;
const int MAKS_TRANSAKSI  = 200;

Layanan    daftarLayanan[MAKS_LAYANAN];
Pelanggan  daftarPelanggan[MAKS_PELANGGAN];
Transaksi  daftarTransaksi[MAKS_TRANSAKSI];
Admin      dataAdmin = {"julpa", "julpa123"};

int jumlahLayanan   = 0;
int jumlahPelanggan = 0;
int jumlahTransaksi = 0;
int nextIdLayanan   = 1;
int nextIdPelanggan = 1;
int nextIdTransaksi = 1001;
int nextNoAntrian   = 1;

// Sesi aktif
bool      loginSebagaiAdmin    = false;
int       idxPelangganAktif    = -1;

// ═══════════════════════════════════════════════════════════
//  DEKLARASI PROSEDUR & FUNGSI
// ═══════════════════════════════════════════════════════════
void        bersihkanLayar();
void        jedaLayar();
void        garis(char c = '-', int n = 62);
string      tanggalHariIni();
int         inputAngka(const string& pesan);
int         inputAngkaBatas(const string& pesan, int minVal, int maxVal);
float       inputDesimal(const string& pesan);
float       inputDesimalPositif(const string& pesan);
string      inputTeks(const string& pesan);
string      inputTeksBolehKosong(const string& pesan);
string      inputPassword(const string& pesan);
bool        konfirmasi(const string& pesan);
void        muatData();
void        simpanData();
void        tampilkanHeader();
void        tampilkanStruk(int idxTrx);
bool        loginAdmin();
bool        loginPelanggan();
void        daftarPelanggan_();
void        menuUtama();
void        menuAdmin();
void        menuKelolalayanan();
void        lihatSemuaLayanan();
void        lihatLayananTersedia();
void        tambahLayananBaru();
void        editLayanan();
void        hapusLayanan();
void        menuKelolaTransaksi();
void        lihatSemuaOrder();
void        tambahTransaksiManual();
void        updateStatusOrder();
void        hapusCancelOrder();
void        menuSortingSearching();
void        sortingData();
void        cariTransaksi();
void        menuPelanggan();
void        buatOrderBaru();
void        lihatOrderStrukSaya();
void        batalkanOrder();
void        mergeSortNama(Transaksi* arr, int kiri, int kanan, Pelanggan* pel, int jmlPel);
void        merge_(Transaksi* arr, int kiri, int tengah, int kanan, Pelanggan* pel, int jmlPel);
int         partisiQuick(Transaksi* arr, int lo, int hi);
void        quickSortHarga(Transaksi* arr, int lo, int hi);
void        selectionSortAntrian(Transaksi* arr, int n);
int         binarySearchId(int targetId);
void        linearSearchNama(const string& nama);
string      getNamaPelanggan(int idPel);
string      getNamaLayanan(int idLay);
int         cariIdxLayanan(int idLay);
int         cariIdxPelanggan(int idPel);
int         cariIdxTransaksi(int idTrx);
string      warnaStatus(const string& s);
void        tampilkanPesan(const string& tipe, const string& pesan);
void        animasiLoading(const string& pesan, int durasi);

// ═══════════════════════════════════════════════════════════
//  UTILITAS TAMPILAN
// ═══════════════════════════════════════════════════════════
void tampilkanPesan(const string& tipe, const string& pesan) {
    if (tipe == "sukses") {
        cout << "\n  " << BGG << B << WH << " ✓ BERHASIL " << R << GR << " " << pesan << R << "\n\n";
    } else if (tipe == "error") {
        cout << "\n  " << BGR << B << WH << " ✗ ERROR " << R << RD << " " << pesan << R << "\n\n";
    } else if (tipe == "info") {
        cout << "\n  " << BGC << B << WH << " ℹ INFO " << R << CY << " " << pesan << R << "\n\n";
    } else if (tipe == "peringatan") {
        cout << "\n  " << BGY << B << " ⚠ PERINGATAN " << R << YL << " " << pesan << R << "\n\n";
    }
}

void animasiLoading(const string& pesan, int langkah) {
    const string spinner[] = {"⠋","⠙","⠹","⠸","⠼","⠴","⠦","⠧","⠇","⠏"};
    for (int i = 0; i < langkah; i++) {
        cout << "\r  " << CY << spinner[i % 10] << R << " " << pesan << "   " << flush;
        // Simulasi delay ringan
        for (volatile long j = 0; j < 8000000L; j++);
    }
    cout << "\r  " << GR << "✓" << R << " " << pesan << " " << GR << "[Selesai]" << R << "   \n";
}

void bersihkanLayar() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void jedaLayar() {
    cout << "\n" << DM << "  ──────────────────────────────────────────────────────────\n";
    cout << "  Tekan Enter untuk melanjutkan..." << R;
    cin.get();
}

void garis(char c, int n) {
    cout << DM << string(n, c) << R << "\n";
}

string tanggalHariIni() {
    time_t t = time(nullptr);
    tm* lt   = localtime(&t);
    char buf[12];
    strftime(buf, sizeof(buf), "%d-%m-%Y", lt);
    return string(buf);
}

// ═══════════════════════════════════════════════════════════
//  INPUT DENGAN ERROR HANDLING LENGKAP
// ═══════════════════════════════════════════════════════════
int inputAngka(const string& pesan) {
    int v;
    while (true) {
        cout << CY << pesan << R;
        string baris;
        getline(cin, baris);
        // Cek apakah kosong
        if (baris.empty()) {
            tampilkanPesan("error", "Input tidak boleh kosong! Masukkan angka.");
            continue;
        }
        // Cek apakah semua karakter adalah digit (boleh ada tanda minus di depan)
        bool valid = true;
        int mulai = 0;
        if (baris[0] == '-') mulai = 1;
        if (mulai == 1 && baris.size() == 1) valid = false;
        for (size_t i = mulai; i < baris.size(); i++) {
            if (!isdigit(baris[i])) { valid = false; break; }
        }
        if (!valid) {
            tampilkanPesan("error", "Input tidak valid! Hanya boleh angka bulat.");
            continue;
        }
        try {
            v = stoi(baris);
            return v;
        } catch (const out_of_range&) {
            tampilkanPesan("error", "Angka terlalu besar! Masukkan angka yang lebih kecil.");
        }
    }
}

int inputAngkaBatas(const string& pesan, int minVal, int maxVal) {
    while (true) {
        int v = inputAngka(pesan);
        if (v >= minVal && v <= maxVal) return v;
        tampilkanPesan("error", "Angka harus antara " + to_string(minVal) + " dan " + to_string(maxVal) + "!");
    }
}

float inputDesimal(const string& pesan) {
    float v;
    while (true) {
        cout << CY << pesan << R;
        string baris;
        getline(cin, baris);
        if (baris.empty()) {
            tampilkanPesan("error", "Input tidak boleh kosong! Masukkan angka.");
            continue;
        }
        // Validasi format desimal
        bool valid = true, adaTitik = false;
        int mulai = 0;
        if (baris[0] == '-') mulai = 1;
        if (mulai == 1 && baris.size() == 1) valid = false;
        for (size_t i = mulai; i < baris.size(); i++) {
            if (baris[i] == '.') {
                if (adaTitik) { valid = false; break; }
                adaTitik = true;
            } else if (!isdigit(baris[i])) {
                valid = false; break;
            }
        }
        if (!valid) {
            tampilkanPesan("error", "Input tidak valid! Masukkan angka (contoh: 2.5 atau 3).");
            continue;
        }
        try {
            v = stof(baris);
            return v;
        } catch (...) {
            tampilkanPesan("error", "Angka tidak valid, coba lagi.");
        }
    }
}

float inputDesimalPositif(const string& pesan) {
    while (true) {
        float v = inputDesimal(pesan);
        if (v > 0) return v;
        tampilkanPesan("error", "Nilai harus lebih dari 0!");
    }
}

string inputTeks(const string& pesan) {
    string v;
    while (true) {
        cout << CY << pesan << R;
        getline(cin, v);
        // Trim spasi di awal dan akhir
        size_t awal = v.find_first_not_of(" \t");
        size_t akhir = v.find_last_not_of(" \t");
        if (awal == string::npos) {
            tampilkanPesan("error", "Input tidak boleh kosong atau hanya spasi!");
            continue;
        }
        v = v.substr(awal, akhir - awal + 1);
        if (v.length() > 100) {
            tampilkanPesan("error", "Input terlalu panjang! Maksimal 100 karakter.");
            continue;
        }
        return v;
    }
}

string inputTeksBolehKosong(const string& pesan) {
    cout << CY << pesan << R;
    string v;
    getline(cin, v);
    return v;
}

string inputPassword(const string& pesan) {
    while (true) {
        cout << CY << pesan << R;
        string v;
        getline(cin, v);
        if (v.empty()) {
            tampilkanPesan("error", "Password tidak boleh kosong!");
            continue;
        }
        if (v.length() < 6) {
            tampilkanPesan("error", "Password minimal 6 karakter!");
            continue;
        }
        if (v.length() > 50) {
            tampilkanPesan("error", "Password maksimal 50 karakter!");
            continue;
        }
        return v;
    }
}

bool konfirmasi(const string& pesan) {
    while (true) {
        cout << YL << pesan << " (y/n): " << R;
        string k;
        getline(cin, k);
        if (k == "y" || k == "Y") return true;
        if (k == "n" || k == "N") return false;
        tampilkanPesan("error", "Masukkan 'y' untuk Ya atau 'n' untuk Tidak!");
    }
}

// ═══════════════════════════════════════════════════════════
//  HELPER
// ═══════════════════════════════════════════════════════════
string getNamaPelanggan(int idPel) {
    for (int i = 0; i < jumlahPelanggan; i++)
        if (daftarPelanggan[i].idPelanggan == idPel)
            return daftarPelanggan[i].nama;
    return "?";
}

string getNamaLayanan(int idLay) {
    for (int i = 0; i < jumlahLayanan; i++)
        if (daftarLayanan[i].idLayanan == idLay)
            return daftarLayanan[i].namaLayanan;
    return "?";
}

int cariIdxLayanan(int idLay) {
    for (int i = 0; i < jumlahLayanan; i++)
        if (daftarLayanan[i].idLayanan == idLay) return i;
    return -1;
}

int cariIdxPelanggan(int idPel) {
    for (int i = 0; i < jumlahPelanggan; i++)
        if (daftarPelanggan[i].idPelanggan == idPel) return i;
    return -1;
}

int cariIdxTransaksi(int idTrx) {
    for (int i = 0; i < jumlahTransaksi; i++)
        if (daftarTransaksi[i].idTransaksi == idTrx) return i;
    return -1;
}

string warnaStatus(const string& s) {
    if (s == "Selesai")     return GR + B + s + R;
    if (s == "Diproses")    return YL + B + s + R;
    if (s == "Menunggu")    return CY + s + R;
    if (s == "Dibatalkan")  return RD + B + s + R;
    if (s == "Lunas")       return GR + B + s + R;
    if (s == "Belum Bayar") return YL + s + R;
    return s;
}


// ═══════════════════════════════════════════════════════════
//  HEADER UTAMA
// ═══════════════════════════════════════════════════════════
void tampilkanHeader() {
    cout << "\n";
    cout << CY << B;
    cout << "  ╔══════════════════════════════════════════════════════════╗\n";
    cout << "  ║                                                          ║\n";
    cout << "  ║    ███████╗██████╗ ██╗     ███████╗███╗   ██╗ ██████╗   ║\n";
    cout << "  ║    ██╔════╝██╔══██╗██║     ██╔════╝████╗  ██║██╔════╝   ║\n";
    cout << "  ║    ███████╗██████╔╝██║     █████╗  ██╔██╗ ██║██║  ███╗  ║\n";
    cout << "  ║    ╚════██║██╔═══╝ ██║     ██╔══╝  ██║╚██╗██║██║   ██║  ║\n";
    cout << "  ║    ███████║██║     ███████╗███████╗██║ ╚████║╚██████╔╝  ║\n";
    cout << "  ║    ╚══════╝╚═╝     ╚══════╝╚══════╝╚═╝  ╚═══╝ ╚═════╝   ║\n";
    cout << R;
    cout << CY;
    cout << "  ║                                                          ║\n";
    cout << "  ║" << R << MG << B << "          W A S H  ·  Sistem Manajemen Laundry          " << R << CY << "║\n";
    cout << "  ║" << R << DM << "          Bersih · Cepat · Terpercaya  |  v2.0            " << R << CY << "║\n";
    cout << "  ║                                                          ║\n";
    cout << "  ╚══════════════════════════════════════════════════════════╝\n";
    cout << R << "\n";
}

// ═══════════════════════════════════════════════════════════
//  STRUK DINAMIS
// ═══════════════════════════════════════════════════════════
void tampilkanStruk(int idxTrx) {
    Transaksi& t = daftarTransaksi[idxTrx];
    int idxL     = cariIdxLayanan(t.idLayanan);
    string nama  = getNamaPelanggan(t.idPelanggan);

    ostringstream oss;
    oss << "TRX-2026-" << setw(3) << setfill('0') << t.idTransaksi - 1000;
    string noOrder = oss.str();

    ostringstream ossA;
    ossA << "#" << setw(3) << setfill('0') << t.noAntrian;
    string noAnt = ossA.str();

    cout << "\n";
    cout << CY << "  ╔══════════════════════════════════════╗\n";
    cout << "  ║" << R << B << "        SplengerWash  Receipt         " << R << CY << "║\n";
    cout << "  ╠══════════════════════════════════════╣\n" << R;
    cout << "  ║  No Antrian  : " << YL << B << left << setw(22) << noAnt << R << CY << "║\n" << R;
    cout << "  ║  No Order    : " << left << setw(22) << noOrder << CY << "║\n" << R;
    cout << "  ║  Status      : " << left << setw(22) << warnaStatus(t.status) << CY << "║\n" << R;
    cout << CY << "  ╠══════════════════════════════════════╣\n" << R;
    cout << "  ║  Nama        : " << left << setw(22) << nama.substr(0,22) << CY << "║\n" << R;
    if (idxL >= 0) {
        cout << "  ║  Layanan     : " << left << setw(22) << daftarLayanan[idxL].namaLayanan.substr(0,22) << CY << "║\n" << R;
        cout << "  ║  Jenis       : " << left << setw(22) << daftarLayanan[idxL].jenis << CY << "║\n" << R;
        cout << "  ║  Tambahan    : " << left << setw(22) << daftarLayanan[idxL].tambahan.substr(0,22) << CY << "║\n" << R;
        cout << "  ║  Estimasi    : " << left << setw(22) << daftarLayanan[idxL].estimasi << CY << "║\n" << R;
    }
    if (t.berat == 0) {
        cout << "  ║  Berat       : " << DM << left << setw(22) << "(menunggu admin)" << R << CY << "║\n" << R;
        cout << "  ║  Harga       : " << DM << left << setw(22) << "(menunggu admin)" << R << CY << "║\n" << R;
    } else {
        string satuanStr = (idxL >= 0 && daftarLayanan[idxL].jenis == "Kiloan") ? " kg" : " pcs";
        ostringstream beratOss;
        beratOss << fixed << setprecision(1) << t.berat << satuanStr;
        cout << "  ║  Berat       : " << left << setw(22) << beratOss.str() << CY << "║\n" << R;
        string hargaStr = "Rp " + to_string(t.hargaTotal);
        cout << "  ║  Harga       : " << GR << B << left << setw(22) << hargaStr << R << CY << "║\n" << R;
    }
    cout << CY << "  ╠══════════════════════════════════════╣\n" << R;
    cout << "  ║  Status Bayar: " << left << setw(22) << warnaStatus(t.statusBayar) << CY << "║\n" << R;
    cout << "  ║  Tgl Order   : " << left << setw(22) << t.tglOrder << CY << "║\n" << R;
    if (!t.tglSelesai.empty())
        cout << "  ║  Tgl Selesai : " << left << setw(22) << t.tglSelesai << CY << "║\n" << R;
    cout << CY << "  ╚══════════════════════════════════════╝\n" << R;
    cout << "\n";
}

// ═══════════════════════════════════════════════════════════
//  FILE I/O
// ═══════════════════════════════════════════════════════════
void muatData() {
    ifstream fL("sw_layanan.txt");
    if (fL.is_open()) {
        fL >> jumlahLayanan >> nextIdLayanan;
        fL.ignore();
        for (int i = 0; i < jumlahLayanan; i++) {
            fL >> daftarLayanan[i].idLayanan >> daftarLayanan[i].harga;
            fL.ignore();
            getline(fL, daftarLayanan[i].namaLayanan);
            getline(fL, daftarLayanan[i].jenis);
            getline(fL, daftarLayanan[i].estimasi);
            getline(fL, daftarLayanan[i].tambahan);
        }
        fL.close();
    } else {
        daftarLayanan[0] = {1, "Paket Hemat (Cuci + Setrika)", "Kiloan", 6000, "2 Hari", "Setrika + Lipat"};
        daftarLayanan[1] = {2, "Paket Express",                "Kiloan", 10000,"1 Hari", "Setrika + Lipat"};
        daftarLayanan[2] = {3, "Cuci Kering",                  "Kiloan", 4000, "2 Hari", "Tanpa Setrika"};
        daftarLayanan[3] = {4, "Setrika Saja",                 "Kiloan", 3000, "1 Hari", "Setrika + Lipat"};
        daftarLayanan[4] = {5, "Cuci Selimut / Bedcover",      "Satuan", 15000,"3 Hari", "Tanpa Setrika"};
        daftarLayanan[5] = {6, "Cuci Sepatu",                  "Satuan", 20000,"2 Hari", "Tanpa Setrika"};
        jumlahLayanan    = 6;
        nextIdLayanan    = 7;
    }

    ifstream fP("sw_pelanggan.txt");
    if (fP.is_open()) {
        fP >> jumlahPelanggan >> nextIdPelanggan;
        fP.ignore();
        for (int i = 0; i < jumlahPelanggan; i++) {
            fP >> daftarPelanggan[i].idPelanggan;
            fP.ignore();
            getline(fP, daftarPelanggan[i].nama);
            getline(fP, daftarPelanggan[i].username);
            getline(fP, daftarPelanggan[i].password);
        }
        fP.close();
    } else {
        daftarPelanggan[0] = {1, "Zeinal Abidin", "zaenal", "zaenal123"};
        daftarPelanggan[1] = {2, "Nuril Akmal",   "akmal",  "akmal123"};
        jumlahPelanggan    = 2;
        nextIdPelanggan    = 3;
    }

    ifstream fT("sw_transaksi.txt");
    if (fT.is_open()) {
        fT >> jumlahTransaksi >> nextIdTransaksi >> nextNoAntrian;
        fT.ignore();
        for (int i = 0; i < jumlahTransaksi; i++) {
            fT >> daftarTransaksi[i].idTransaksi
               >> daftarTransaksi[i].noAntrian
               >> daftarTransaksi[i].idPelanggan
               >> daftarTransaksi[i].idLayanan
               >> daftarTransaksi[i].berat
               >> daftarTransaksi[i].hargaTotal;
            fT.ignore();
            getline(fT, daftarTransaksi[i].status);
            getline(fT, daftarTransaksi[i].statusBayar);
            getline(fT, daftarTransaksi[i].tglOrder);
            getline(fT, daftarTransaksi[i].tglSelesai);
        }
        fT.close();
    }
}

void simpanData() {
    ofstream fL("sw_layanan.txt");
    if (fL.is_open()) {
        fL << jumlahLayanan << " " << nextIdLayanan << "\n";
        for (int i = 0; i < jumlahLayanan; i++) {
            fL << daftarLayanan[i].idLayanan << " " << daftarLayanan[i].harga << "\n";
            fL << daftarLayanan[i].namaLayanan << "\n";
            fL << daftarLayanan[i].jenis << "\n";
            fL << daftarLayanan[i].estimasi << "\n";
            fL << daftarLayanan[i].tambahan << "\n";
        }
        fL.close();
    }

    ofstream fP("sw_pelanggan.txt");
    if (fP.is_open()) {
        fP << jumlahPelanggan << " " << nextIdPelanggan << "\n";
        for (int i = 0; i < jumlahPelanggan; i++) {
            fP << daftarPelanggan[i].idPelanggan << "\n";
            fP << daftarPelanggan[i].nama << "\n";
            fP << daftarPelanggan[i].username << "\n";
            fP << daftarPelanggan[i].password << "\n";
        }
        fP.close();
    }

    ofstream fT("sw_transaksi.txt");
    if (fT.is_open()) {
        fT << jumlahTransaksi << " " << nextIdTransaksi << " " << nextNoAntrian << "\n";
        for (int i = 0; i < jumlahTransaksi; i++) {
            fT << daftarTransaksi[i].idTransaksi << " "
               << daftarTransaksi[i].noAntrian   << " "
               << daftarTransaksi[i].idPelanggan << " "
               << daftarTransaksi[i].idLayanan   << " "
               << daftarTransaksi[i].berat       << " "
               << daftarTransaksi[i].hargaTotal  << "\n";
            fT << daftarTransaksi[i].status      << "\n";
            fT << daftarTransaksi[i].statusBayar << "\n";
            fT << daftarTransaksi[i].tglOrder    << "\n";
            fT << daftarTransaksi[i].tglSelesai  << "\n";
        }
        fT.close();
    }
}


// ═══════════════════════════════════════════════════════════
//  AUTENTIKASI
// ═══════════════════════════════════════════════════════════
bool loginAdmin() {
    bersihkanLayar(); tampilkanHeader();
    cout << YL << B << "  ┌─────────────────────────────┐\n";
    cout << "  │       LOGIN ADMIN           │\n";
    cout << "  └─────────────────────────────┘\n" << R;
    garis();

    int percobaan = 0;
    const int MAKS_PERCOBAAN = 3;
    while (percobaan < MAKS_PERCOBAAN) {
        cout << DM << "  Percobaan " << (percobaan + 1) << "/" << MAKS_PERCOBAAN << R << "\n";
        string u = inputTeks("  Username : ");
        string p = inputTeks("  Password : ");
        if (u == dataAdmin.username && p == dataAdmin.password) {
            animasiLoading("Memverifikasi kredensial", 8);
            tampilkanPesan("sukses", "Login berhasil! Selamat datang, " + dataAdmin.username + ".");
            jedaLayar();
            return true;
        }
        percobaan++;
        int sisa = MAKS_PERCOBAAN - percobaan;
        if (sisa > 0) {
            tampilkanPesan("error", "Username atau password salah! Sisa percobaan: " + to_string(sisa));
        }
    }
    tampilkanPesan("peringatan", "Akun dikunci sementara. 3x percobaan gagal.");
    jedaLayar();
    return false;
}

bool loginPelanggan() {
    bersihkanLayar(); tampilkanHeader();
    cout << BL << B << "  ┌─────────────────────────────┐\n";
    cout << "  │      LOGIN PELANGGAN        │\n";
    cout << "  └─────────────────────────────┘\n" << R;
    garis();

    int percobaan = 0;
    const int MAKS_PERCOBAAN = 3;
    while (percobaan < MAKS_PERCOBAAN) {
        cout << DM << "  Percobaan " << (percobaan + 1) << "/" << MAKS_PERCOBAAN << R << "\n";
        string u = inputTeks("  Username : ");
        string p = inputTeks("  Password : ");
        for (int i = 0; i < jumlahPelanggan; i++) {
            if (daftarPelanggan[i].username == u && daftarPelanggan[i].password == p) {
                idxPelangganAktif = i;
                animasiLoading("Memverifikasi akun", 8);
                tampilkanPesan("sukses", "Selamat datang, " + daftarPelanggan[i].nama + "!");
                jedaLayar();
                return true;
            }
        }
        percobaan++;
        int sisa = MAKS_PERCOBAAN - percobaan;
        if (sisa > 0) {
            tampilkanPesan("error", "Username atau password salah! Sisa percobaan: " + to_string(sisa));
        }
    }
    tampilkanPesan("peringatan", "Akun dikunci sementara. 3x percobaan gagal.");
    jedaLayar();
    return false;
}

void daftarPelanggan_() {
    bersihkanLayar(); tampilkanHeader();
    cout << CY << B << "  ┌─────────────────────────────┐\n";
    cout << "  │    DAFTAR PELANGGAN BARU    │\n";
    cout << "  └─────────────────────────────┘\n" << R;
    garis();

    if (jumlahPelanggan >= MAKS_PELANGGAN) {
        tampilkanPesan("error", "Kapasitas pengguna penuh! Hubungi admin.");
        jedaLayar(); return;
    }

    // Input nama
    string nama = inputTeks("  Nama lengkap  : ");
    if (nama.length() < 3) {
        tampilkanPesan("error", "Nama minimal 3 karakter!");
        jedaLayar(); return;
    }

    // Input username unik
    string user;
    while (true) {
        user = inputTeks("  Username      : ");
        if (user.length() < 4) {
            tampilkanPesan("error", "Username minimal 4 karakter!");
            continue;
        }
        // Cek karakter valid (hanya huruf, angka, underscore)
        bool valid = true;
        for (char c : user) {
            if (!isalnum(c) && c != '_') { valid = false; break; }
        }
        if (!valid) {
            tampilkanPesan("error", "Username hanya boleh huruf, angka, dan underscore (_)!");
            continue;
        }
        bool ada = false;
        for (int i = 0; i < jumlahPelanggan; i++)
            if (daftarPelanggan[i].username == user) { ada = true; break; }
        if (ada) {
            tampilkanPesan("error", "Username '" + user + "' sudah digunakan, coba yang lain.");
            continue;
        }
        break;
    }

    // Input password dengan konfirmasi
    string pass, passKonfirm;
    while (true) {
        pass = inputPassword("  Password      : ");
        passKonfirm = inputPassword("  Konfirm Pass  : ");
        if (pass == passKonfirm) break;
        tampilkanPesan("error", "Password tidak cocok! Coba lagi.");
    }

    animasiLoading("Mendaftarkan akun", 6);
    daftarPelanggan[jumlahPelanggan] = {nextIdPelanggan++, nama, user, pass};
    jumlahPelanggan++;
    simpanData();
    tampilkanPesan("sukses", "Registrasi berhasil! Silakan login dengan akun Anda.");
    jedaLayar();
}

// ═══════════════════════════════════════════════════════════
//  TAMPILAN TABEL LAYANAN
// ═══════════════════════════════════════════════════════════
void lihatSemuaLayanan() {
    cout << MG << B << "  DAFTAR LAYANAN\n" << R;
    garis('═', 62);
    if (jumlahLayanan == 0) {
        tampilkanPesan("info", "Belum ada layanan yang tersedia.");
        return;
    }
    cout << B << "  " << left
         << setw(5)  << "ID"
         << setw(30) << "Nama Layanan"
         << setw(9)  << "Jenis"
         << setw(12) << "Harga"
         << setw(9)  << "Estimasi"
         << "Tambahan" << R << "\n";
    garis('─', 62);
    for (int i = 0; i < jumlahLayanan; i++) {
        Layanan& l = daftarLayanan[i];
        cout << "  " << YL << B << setw(3) << l.idLayanan << R << "  "
             << left << setw(30) << l.namaLayanan.substr(0, 28)
             << setw(9)  << l.jenis
             << GR << setw(12) << ("Rp " + to_string((int)l.harga)) << R
             << setw(9)  << l.estimasi
             << DM << l.tambahan << R << "\n";
    }
    garis('═', 62);
    cout << DM << "  Total: " << jumlahLayanan << " layanan\n" << R;
}

void lihatLayananTersedia() {
    lihatSemuaLayanan();
}

// ═══════════════════════════════════════════════════════════
//  KELOLA LAYANAN (ADMIN)
// ═══════════════════════════════════════════════════════════
void tambahLayananBaru() {
    if (jumlahLayanan >= MAKS_LAYANAN) {
        tampilkanPesan("error", "Kapasitas layanan penuh! Maksimal " + to_string(MAKS_LAYANAN) + " layanan.");
        return;
    }

    cout << MG << B << "  TAMBAH LAYANAN BARU\n" << R;
    garis();

    string nama = inputTeks("  Nama layanan         : ");

    string jenis;
    while (true) {
        cout << "  Jenis layanan:\n";
        cout << "    " << CY << "1" << R << ". Kiloan\n";
        cout << "    " << CY << "2" << R << ". Satuan\n";
        int pj = inputAngkaBatas("  Pilih (1/2): ", 1, 2);
        jenis = (pj == 1) ? "Kiloan" : "Satuan";
        break;
    }

    float harga = inputDesimalPositif("  Harga (per kg/pcs)   : Rp ");

    string estimasi = inputTeks("  Estimasi (cth: 1 Hari): ");

    cout << "  Layanan tambahan:\n";
    cout << "    " << CY << "1" << R << ". Tanpa Setrika\n";
    cout << "    " << CY << "2" << R << ". Setrika + Lipat\n";
    int pt = inputAngkaBatas("  Pilih (1/2): ", 1, 2);
    string tambahan = (pt == 1) ? "Tanpa Setrika" : "Setrika + Lipat";

    // Konfirmasi sebelum simpan
    cout << "\n" << DM << "  Ringkasan:\n" << R;
    cout << "  Nama     : " << nama << "\n";
    cout << "  Jenis    : " << jenis << "\n";
    cout << "  Harga    : Rp " << (int)harga << "\n";
    cout << "  Estimasi : " << estimasi << "\n";
    cout << "  Tambahan : " << tambahan << "\n\n";

    if (!konfirmasi("  Simpan layanan ini?")) {
        tampilkanPesan("info", "Penambahan layanan dibatalkan.");
        return;
    }

    daftarLayanan[jumlahLayanan] = {nextIdLayanan++, nama, jenis, harga, estimasi, tambahan};
    jumlahLayanan++;
    simpanData();
    tampilkanPesan("sukses", "Layanan berhasil ditambahkan.");
}

void editLayanan() {
    if (jumlahLayanan == 0) {
        tampilkanPesan("info", "Belum ada layanan untuk diedit.");
        return;
    }
    lihatSemuaLayanan();
    int id = inputAngka("  Masukkan ID layanan yang diedit: ");
    int idx = cariIdxLayanan(id);
    if (idx == -1) {
        tampilkanPesan("error", "ID layanan " + to_string(id) + " tidak ditemukan!");
        return;
    }

    Layanan& l = daftarLayanan[idx];
    cout << "\n" << DM << "  Data saat ini:\n" << R;
    cout << "  Nama     : " << l.namaLayanan << "\n";
    cout << "  Jenis    : " << l.jenis << "\n";
    cout << "  Harga    : Rp " << (int)l.harga << "\n";
    cout << "  Estimasi : " << l.estimasi << "\n";
    cout << "  Tambahan : " << l.tambahan << "\n\n";
    cout << DM << IT << "  (Tekan Enter untuk melewati / tidak mengubah)\n\n" << R;

    string tmp;
    tmp = inputTeksBolehKosong("  Nama baru     : ");
    if (!tmp.empty()) {
        if (tmp.length() > 100) tampilkanPesan("peringatan", "Nama terlalu panjang, dilewati.");
        else l.namaLayanan = tmp;
    }

    tmp = inputTeksBolehKosong("  Jenis baru (Kiloan/Satuan): ");
    if (!tmp.empty()) {
        if (tmp == "Kiloan" || tmp == "Satuan") l.jenis = tmp;
        else tampilkanPesan("peringatan", "Jenis tidak valid (harus 'Kiloan' atau 'Satuan'), dilewati.");
    }

    tmp = inputTeksBolehKosong("  Harga baru (Enter=skip)  : Rp ");
    if (!tmp.empty()) {
        try {
            float h = stof(tmp);
            if (h > 0) l.harga = h;
            else tampilkanPesan("peringatan", "Harga harus > 0, dilewati.");
        } catch (...) {
            tampilkanPesan("peringatan", "Format harga tidak valid, dilewati.");
        }
    }

    tmp = inputTeksBolehKosong("  Estimasi baru : ");
    if (!tmp.empty()) l.estimasi = tmp;

    cout << "  Tambahan baru:\n";
    cout << "    " << CY << "0" << R << ". Lewati\n";
    cout << "    " << CY << "1" << R << ". Tanpa Setrika\n";
    cout << "    " << CY << "2" << R << ". Setrika + Lipat\n";
    int p = inputAngkaBatas("  Pilih (0/1/2): ", 0, 2);
    if (p == 1) l.tambahan = "Tanpa Setrika";
    else if (p == 2) l.tambahan = "Setrika + Lipat";

    simpanData();
    tampilkanPesan("sukses", "Layanan berhasil diperbarui.");
}

void hapusLayanan() {
    if (jumlahLayanan == 0) {
        tampilkanPesan("info", "Belum ada layanan untuk dihapus.");
        return;
    }
    lihatSemuaLayanan();
    int id = inputAngka("  Masukkan ID layanan yang dihapus: ");
    int idx = cariIdxLayanan(id);
    if (idx == -1) {
        tampilkanPesan("error", "ID layanan " + to_string(id) + " tidak ditemukan!");
        return;
    }

    cout << "\n  Layanan yang akan dihapus: " << RD << B << daftarLayanan[idx].namaLayanan << R << "\n";
    if (!konfirmasi("  Yakin ingin menghapus layanan ini?")) {
        tampilkanPesan("info", "Penghapusan dibatalkan.");
        return;
    }

    for (int i = idx; i < jumlahLayanan - 1; i++)
        daftarLayanan[i] = daftarLayanan[i + 1];
    jumlahLayanan--;
    simpanData();
    tampilkanPesan("sukses", "Layanan berhasil dihapus.");
}

void menuKelolalayanan() {
    bool aktif = true;
    while (aktif) {
        bersihkanLayar(); tampilkanHeader();
        cout << MG << B << "  ╔══════════════════════════╗\n";
        cout << "  ║     KELOLA LAYANAN       ║\n";
        cout << "  ╚══════════════════════════╝\n" << R;
        garis();
        cout << "  " << CY << "1" << R << ". Lihat Semua Layanan\n";
        cout << "  " << CY << "2" << R << ". Tambah Layanan Baru\n";
        cout << "  " << CY << "3" << R << ". Edit Layanan\n";
        cout << "  " << CY << "4" << R << ". Hapus Layanan\n";
        cout << "  " << RD << "0" << R << ". Kembali\n";
        garis();
        int p = inputAngka("  Pilih: ");
        switch (p) {
            case 1: bersihkanLayar(); tampilkanHeader(); lihatSemuaLayanan(); jedaLayar(); break;
            case 2: bersihkanLayar(); tampilkanHeader(); tambahLayananBaru(); jedaLayar(); break;
            case 3: bersihkanLayar(); tampilkanHeader(); editLayanan();       jedaLayar(); break;
            case 4: bersihkanLayar(); tampilkanHeader(); hapusLayanan();      jedaLayar(); break;
            case 0: aktif = false; break;
            default:
                tampilkanPesan("error", "Pilihan tidak valid! Masukkan angka 0-4.");
                jedaLayar();
        }
    }
}


// ═══════════════════════════════════════════════════════════
//  TAMPILAN TABEL TRANSAKSI
// ═══════════════════════════════════════════════════════════
void lihatSemuaOrder() {
    cout << CY << B << "  SEMUA ORDER\n" << R;
    garis('═', 62);
    if (jumlahTransaksi == 0) {
        tampilkanPesan("info", "Belum ada order masuk.");
        return;
    }
    cout << B << "  " << left
         << setw(7)  << "Antri"
         << setw(7)  << "ID"
         << setw(14) << "Pelanggan"
         << setw(22) << "Layanan"
         << setw(8)  << "Berat"
         << setw(12) << "Harga"
         << setw(12) << "Status"
         << "Bayar" << R << "\n";
    garis('─', 62);
    for (int i = 0; i < jumlahTransaksi; i++) {
        Transaksi& t = daftarTransaksi[i];
        string beratStr  = (t.berat == 0)      ? DM + string("-") + R : to_string(t.berat).substr(0,4);
        string hargaStr  = (t.hargaTotal == 0) ? DM + string("-") + R : "Rp " + to_string(t.hargaTotal);
        cout << "  " << YL << setw(5) << ("#" + to_string(t.noAntrian)) << R << "  "
             << left << setw(7)  << t.idTransaksi
             << setw(14) << getNamaPelanggan(t.idPelanggan).substr(0,12)
             << setw(22) << getNamaLayanan(t.idLayanan).substr(0, 20)
             << setw(8)  << beratStr
             << setw(12) << hargaStr
             << setw(12) << warnaStatus(t.status)
             << warnaStatus(t.statusBayar) << "\n";
    }
    garis('═', 62);
    cout << DM << "  Total: " << jumlahTransaksi << " order\n" << R;
}

// ═══════════════════════════════════════════════════════════
//  KELOLA TRANSAKSI (ADMIN)
// ═══════════════════════════════════════════════════════════
void tambahTransaksiManual() {
    if (jumlahTransaksi >= MAKS_TRANSAKSI) {
        tampilkanPesan("error", "Kapasitas transaksi penuh! Maksimal " + to_string(MAKS_TRANSAKSI) + " transaksi.");
        return;
    }
    if (jumlahPelanggan == 0) {
        tampilkanPesan("error", "Belum ada pelanggan terdaftar!");
        return;
    }

    cout << B << "  DAFTAR PELANGGAN:\n" << R;
    garis('─', 40);
    for (int i = 0; i < jumlahPelanggan; i++)
        cout << "  " << YL << daftarPelanggan[i].idPelanggan << R
             << ". " << daftarPelanggan[i].nama << " (" << DM << daftarPelanggan[i].username << R << ")\n";
    garis('─', 40);

    int idPel, idxPel = -1;
    while (idxPel == -1) {
        idPel  = inputAngka("  ID Pelanggan: ");
        idxPel = cariIdxPelanggan(idPel);
        if (idxPel == -1)
            tampilkanPesan("error", "Pelanggan dengan ID " + to_string(idPel) + " tidak ditemukan!");
    }

    lihatSemuaLayanan();
    int idLay, idxLay = -1;
    while (idxLay == -1) {
        idLay  = inputAngka("  ID Layanan: ");
        idxLay = cariIdxLayanan(idLay);
        if (idxLay == -1)
            tampilkanPesan("error", "Layanan dengan ID " + to_string(idLay) + " tidak ditemukan!");
    }

    float berat = inputDesimalPositif("  Berat (kg/pcs): ");
    int harga = (int)(berat * daftarLayanan[idxLay].harga);

    cout << "\n" << DM << "  Ringkasan transaksi:\n" << R;
    cout << "  Pelanggan : " << daftarPelanggan[idxPel].nama << "\n";
    cout << "  Layanan   : " << daftarLayanan[idxLay].namaLayanan << "\n";
    cout << "  Berat     : " << berat << "\n";
    cout << "  Total     : " << GR << B << "Rp " << harga << R << "\n\n";

    if (!konfirmasi("  Konfirmasi tambah transaksi?")) {
        tampilkanPesan("info", "Transaksi dibatalkan.");
        return;
    }

    Transaksi trx;
    trx.idTransaksi  = nextIdTransaksi++;
    trx.noAntrian    = nextNoAntrian++;
    trx.idPelanggan  = idPel;
    trx.idLayanan    = idLay;
    trx.berat        = berat;
    trx.hargaTotal   = harga;
    trx.status       = "Diproses";
    trx.statusBayar  = "Belum Bayar";
    trx.tglOrder     = tanggalHariIni();
    trx.tglSelesai   = "";

    daftarTransaksi[jumlahTransaksi++] = trx;
    simpanData();
    tampilkanPesan("sukses", "Transaksi berhasil ditambahkan!");
    tampilkanStruk(jumlahTransaksi - 1);
}

void updateStatusOrder() {
    if (jumlahTransaksi == 0) {
        tampilkanPesan("info", "Belum ada order.");
        return;
    }
    lihatSemuaOrder();

    int id  = inputAngka("  Masukkan ID Transaksi: ");
    int idx = cariIdxTransaksi(id);
    while (idx == -1) {
        tampilkanPesan("error", "ID transaksi " + to_string(id) + " tidak ditemukan!");
        id  = inputAngka("  Masukkan ID Transaksi (0=batal): ");
        if (id == 0) return;
        idx = cariIdxTransaksi(id);
    }

    Transaksi& t = daftarTransaksi[idx];
    cout << "\n  Status saat ini: " << warnaStatus(t.status) << "\n\n";

    if (t.status == "Menunggu") {
        int idxL = cariIdxLayanan(t.idLayanan);
        cout << "  Layanan : " << (idxL >= 0 ? daftarLayanan[idxL].namaLayanan : "-") << "\n";
        float berat = inputDesimalPositif("  Input berat (kg/pcs): ");
        t.berat      = berat;
        t.hargaTotal = (idxL >= 0) ? (int)(berat * daftarLayanan[idxL].harga) : 0;
        t.status     = "Diproses";
        simpanData();
        tampilkanPesan("sukses", "Order diproses. Total: Rp " + to_string(t.hargaTotal));

    } else if (t.status == "Diproses") {
        if (!konfirmasi("  Tandai order ini sebagai Selesai?")) return;
        t.status     = "Selesai";
        t.tglSelesai = tanggalHariIni();
        simpanData();
        tampilkanPesan("sukses", "Order ditandai selesai.");

    } else if (t.status == "Selesai") {
        if (!konfirmasi("  Konfirmasi pembayaran Lunas?")) return;
        t.statusBayar = "Lunas";
        simpanData();
        tampilkanPesan("sukses", "Pembayaran dikonfirmasi. Terima kasih!");

    } else if (t.status == "Dibatalkan") {
        tampilkanPesan("peringatan", "Order sudah dibatalkan, tidak bisa diupdate.");
    }
}

void hapusCancelOrder() {
    if (jumlahTransaksi == 0) {
        tampilkanPesan("info", "Belum ada order.");
        return;
    }
    lihatSemuaOrder();

    int id  = inputAngka("  Masukkan ID Transaksi (0=batal): ");
    if (id == 0) return;
    int idx = cariIdxTransaksi(id);
    while (idx == -1) {
        tampilkanPesan("error", "ID transaksi " + to_string(id) + " tidak ditemukan!");
        id  = inputAngka("  Masukkan ID Transaksi (0=batal): ");
        if (id == 0) return;
        idx = cariIdxTransaksi(id);
    }

    cout << "\n  Order yang akan dibatalkan:\n";
    cout << "  ID     : " << daftarTransaksi[idx].idTransaksi << "\n";
    cout << "  Status : " << warnaStatus(daftarTransaksi[idx].status) << "\n\n";

    if (!konfirmasi("  Yakin ingin membatalkan order ini?")) {
        tampilkanPesan("info", "Pembatalan dibatalkan.");
        return;
    }

    daftarTransaksi[idx].status = "Dibatalkan";
    simpanData();
    tampilkanPesan("sukses", "Order berhasil dibatalkan.");
}

void menuKelolaTransaksi() {
    bool aktif = true;
    while (aktif) {
        bersihkanLayar(); tampilkanHeader();
        cout << CY << B << "  ╔══════════════════════════╗\n";
        cout << "  ║    KELOLA TRANSAKSI      ║\n";
        cout << "  ╚══════════════════════════╝\n" << R;
        garis();
        cout << "  " << CY << "1" << R << ". Lihat Semua Order\n";
        cout << "  " << CY << "2" << R << ". Tambah Transaksi Manual (Walk-in)\n";
        cout << "  " << CY << "3" << R << ". Update Status Order\n";
        cout << "  " << CY << "4" << R << ". Hapus / Cancel Order\n";
        cout << "  " << RD << "0" << R << ". Kembali\n";
        garis();
        int p = inputAngka("  Pilih: ");
        switch (p) {
            case 1: bersihkanLayar(); tampilkanHeader(); lihatSemuaOrder();        jedaLayar(); break;
            case 2: bersihkanLayar(); tampilkanHeader(); tambahTransaksiManual();  jedaLayar(); break;
            case 3: bersihkanLayar(); tampilkanHeader(); updateStatusOrder();      jedaLayar(); break;
            case 4: bersihkanLayar(); tampilkanHeader(); hapusCancelOrder();       jedaLayar(); break;
            case 0: aktif = false; break;
            default:
                tampilkanPesan("error", "Pilihan tidak valid! Masukkan angka 0-4.");
                jedaLayar();
        }
    }
}


// ═══════════════════════════════════════════════════════════
//  SORTING
// ═══════════════════════════════════════════════════════════
void merge_(Transaksi* arr, int kiri, int tengah, int kanan, Pelanggan* pel, int jmlPel) {
    int n1 = tengah - kiri + 1, n2 = kanan - tengah;
    Transaksi* L  = new Transaksi[n1];
    Transaksi* R_ = new Transaksi[n2];
    for (int i = 0; i < n1; i++) L[i]  = arr[kiri + i];
    for (int j = 0; j < n2; j++) R_[j] = arr[tengah + 1 + j];
    int i = 0, j = 0, k = kiri;
    while (i < n1 && j < n2) {
        string nL = "", nR = "";
        for (int x = 0; x < jmlPel; x++) {
            if (pel[x].idPelanggan == L[i].idPelanggan)  nL = pel[x].nama;
            if (pel[x].idPelanggan == R_[j].idPelanggan) nR = pel[x].nama;
        }
        if (nL <= nR) arr[k++] = L[i++];
        else          arr[k++] = R_[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R_[j++];
    delete[] L; delete[] R_;
}

void mergeSortNama(Transaksi* arr, int kiri, int kanan, Pelanggan* pel, int jmlPel) {
    if (kiri < kanan) {
        int tengah = (kiri + kanan) / 2;
        mergeSortNama(arr, kiri, tengah, pel, jmlPel);
        mergeSortNama(arr, tengah + 1, kanan, pel, jmlPel);
        merge_(arr, kiri, tengah, kanan, pel, jmlPel);
    }
}

int partisiQuick(Transaksi* arr, int lo, int hi) {
    int pivot = arr[hi].hargaTotal;
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j].hargaTotal >= pivot) {
            i++;
            Transaksi tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        }
    }
    Transaksi tmp = arr[i+1]; arr[i+1] = arr[hi]; arr[hi] = tmp;
    return i + 1;
}

void quickSortHarga(Transaksi* arr, int lo, int hi) {
    if (lo < hi) {
        int p = partisiQuick(arr, lo, hi);
        quickSortHarga(arr, lo, p - 1);
        quickSortHarga(arr, p + 1, hi);
    }
}

void selectionSortAntrian(Transaksi* arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j].noAntrian < arr[minIdx].noAntrian) minIdx = j;
        if (minIdx != i) {
            Transaksi tmp = arr[i]; arr[i] = arr[minIdx]; arr[minIdx] = tmp;
        }
    }
}

void sortingData() {
    if (jumlahTransaksi == 0) {
        tampilkanPesan("info", "Belum ada data transaksi untuk diurutkan.");
        return;
    }

    cout << MG << B << "  SORTING DATA\n" << R;
    garis();
    cout << "  " << CY << "1" << R << ". Nama Pelanggan  (Merge Sort  - Ascending)\n";
    cout << "  " << CY << "2" << R << ". Total Bayar     (Quick Sort  - Descending)\n";
    cout << "  " << CY << "3" << R << ". Nomor Antrian   (Selection Sort - Ascending)\n";
    int p = inputAngkaBatas("  Pilih (1-3): ", 1, 3);

    Transaksi temp[MAKS_TRANSAKSI];
    for (int i = 0; i < jumlahTransaksi; i++) temp[i] = daftarTransaksi[i];

    cout << "\n" << YL << B << "  DATA SEBELUM SORTING:\n" << R;
    garis('─', 55);
    for (int i = 0; i < jumlahTransaksi; i++) {
        cout << "  " << DM << temp[i].idTransaksi << R << " │ "
             << left << setw(16) << getNamaPelanggan(temp[i].idPelanggan)
             << " │ Antri " << YL << "#" << temp[i].noAntrian << R
             << " │ " << GR << "Rp " << temp[i].hargaTotal << R << "\n";
    }

    animasiLoading("Mengurutkan data", 10);

    if (p == 1) mergeSortNama(temp, 0, jumlahTransaksi - 1, daftarPelanggan, jumlahPelanggan);
    else if (p == 2) quickSortHarga(temp, 0, jumlahTransaksi - 1);
    else selectionSortAntrian(temp, jumlahTransaksi);

    cout << "\n" << GR << B << "  DATA SESUDAH SORTING:\n" << R;
    garis('─', 55);
    for (int i = 0; i < jumlahTransaksi; i++) {
        cout << "  " << DM << temp[i].idTransaksi << R << " │ "
             << left << setw(16) << getNamaPelanggan(temp[i].idPelanggan)
             << " │ Antri " << YL << "#" << temp[i].noAntrian << R
             << " │ " << GR << "Rp " << temp[i].hargaTotal << R << "\n";
    }
    garis('─', 55);
}

// ═══════════════════════════════════════════════════════════
//  SEARCHING
// ═══════════════════════════════════════════════════════════
int binarySearchId(int targetId) {
    Transaksi temp[MAKS_TRANSAKSI];
    for (int i = 0; i < jumlahTransaksi; i++) temp[i] = daftarTransaksi[i];
    for (int i = 0; i < jumlahTransaksi - 1; i++) {
        int minIdx = i;
        for (int j = i+1; j < jumlahTransaksi; j++)
            if (temp[j].idTransaksi < temp[minIdx].idTransaksi) minIdx = j;
        Transaksi t = temp[i]; temp[i] = temp[minIdx]; temp[minIdx] = t;
    }
    int lo = 0, hi = jumlahTransaksi - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (temp[mid].idTransaksi == targetId)
            return cariIdxTransaksi(targetId);
        else if (temp[mid].idTransaksi < targetId) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

void linearSearchNama(const string& nama) {
    bool ada = false;
    cout << "\n" << GR << B << "  Hasil pencarian: '" << nama << "'\n" << R;
    garis();
    for (int i = 0; i < jumlahTransaksi; i++) {
        string np = getNamaPelanggan(daftarTransaksi[i].idPelanggan);
        string npLow = np, namaLow = nama;
        for (char& c : npLow)   c = tolower(c);
        for (char& c : namaLow) c = tolower(c);
        if (npLow.find(namaLow) != string::npos) {
            tampilkanStruk(i);
            ada = true;
        }
    }
    if (!ada) tampilkanPesan("info", "Tidak ada transaksi dengan nama pelanggan '" + nama + "'.");
}

void cariTransaksi() {
    cout << BL << B << "  CARI TRANSAKSI\n" << R;
    garis();
    cout << "  " << CY << "1" << R << ". Cari berdasarkan ID Transaksi (Binary Search)\n";
    cout << "  " << CY << "2" << R << ". Cari berdasarkan Nama Pelanggan (Linear Search)\n";
    int p = inputAngkaBatas("  Pilih (1/2): ", 1, 2);

    if (p == 1) {
        int id  = inputAngka("  Masukkan ID Transaksi: ");
        if (id <= 0) {
            tampilkanPesan("error", "ID transaksi harus lebih dari 0!");
            return;
        }
        animasiLoading("Mencari transaksi", 6);
        int idx = binarySearchId(id);
        if (idx == -1) tampilkanPesan("info", "ID transaksi " + to_string(id) + " tidak ditemukan.");
        else { tampilkanPesan("sukses", "Transaksi ditemukan!"); tampilkanStruk(idx); }
    } else {
        string nama = inputTeks("  Masukkan nama pelanggan: ");
        animasiLoading("Mencari transaksi", 6);
        linearSearchNama(nama);
    }
}

void menuSortingSearching() {
    bool aktif = true;
    while (aktif) {
        bersihkanLayar(); tampilkanHeader();
        cout << MG << B << "  ╔══════════════════════════╗\n";
        cout << "  ║   SORTING & SEARCHING    ║\n";
        cout << "  ╚══════════════════════════╝\n" << R;
        garis();
        cout << "  " << CY << "1" << R << ". Sorting Data\n";
        cout << "  " << CY << "2" << R << ". Cari Transaksi\n";
        cout << "  " << RD << "0" << R << ". Kembali\n";
        garis();
        int p = inputAngka("  Pilih: ");
        switch (p) {
            case 1: bersihkanLayar(); tampilkanHeader(); sortingData();    jedaLayar(); break;
            case 2: bersihkanLayar(); tampilkanHeader(); cariTransaksi();  jedaLayar(); break;
            case 0: aktif = false; break;
            default:
                tampilkanPesan("error", "Pilihan tidak valid! Masukkan 0, 1, atau 2.");
                jedaLayar();
        }
    }
}

// ═══════════════════════════════════════════════════════════
//  MENU PELANGGAN
// ═══════════════════════════════════════════════════════════
void buatOrderBaru() {
    if (jumlahTransaksi >= MAKS_TRANSAKSI) {
        tampilkanPesan("error", "Sistem penuh, hubungi admin.");
        return;
    }
    if (jumlahLayanan == 0) {
        tampilkanPesan("error", "Belum ada layanan tersedia, hubungi admin.");
        return;
    }

    lihatLayananTersedia();
    cout << "\n";

    int idLay, idxLay = -1;
    while (idxLay == -1) {
        idLay  = inputAngka("  Pilih ID Layanan: ");
        idxLay = cariIdxLayanan(idLay);
        if (idxLay == -1)
            tampilkanPesan("error", "Layanan dengan ID " + to_string(idLay) + " tidak ditemukan!");
    }

    Layanan& l = daftarLayanan[idxLay];
    cout << "\n" << DM << "  Detail layanan yang dipilih:\n" << R;
    garis('─', 40);
    cout << "  Nama     : " << B << l.namaLayanan << R << "\n";
    cout << "  Jenis    : " << l.jenis << "\n";
    cout << "  Harga    : " << GR << "Rp " << (int)l.harga << R << " / " << (l.jenis == "Kiloan" ? "kg" : "pcs") << "\n";
    cout << "  Estimasi : " << l.estimasi << "\n";
    cout << "  Tambahan : " << l.tambahan << "\n";
    garis('─', 40);

    if (!konfirmasi("  Lanjutkan order ini?")) {
        tampilkanPesan("info", "Order dibatalkan.");
        return;
    }

    Transaksi trx;
    trx.idTransaksi  = nextIdTransaksi++;
    trx.noAntrian    = nextNoAntrian++;
    trx.idPelanggan  = daftarPelanggan[idxPelangganAktif].idPelanggan;
    trx.idLayanan    = idLay;
    trx.berat        = 0;
    trx.hargaTotal   = 0;
    trx.status       = "Menunggu";
    trx.statusBayar  = "Belum Bayar";
    trx.tglOrder     = tanggalHariIni();
    trx.tglSelesai   = "";

    daftarTransaksi[jumlahTransaksi++] = trx;
    simpanData();

    tampilkanPesan("sukses", "Order berhasil dibuat!");
    cout << "  Nomor Antrian : " << YL << B << "#" << trx.noAntrian << R << "\n";
    cout << "  ID Order      : " << trx.idTransaksi << "\n";
    cout << DM << IT << "  Silakan antar pakaian ke toko.\n" << R;
}

void lihatOrderStrukSaya() {
    int idPel = daftarPelanggan[idxPelangganAktif].idPelanggan;
    cout << BL << B << "  ORDER & STRUK SAYA\n" << R;
    garis('═', 55);

    bool ada = false;
    cout << B << "  " << left << setw(7) << "ID" << setw(24) << "Layanan"
         << setw(14) << "Status" << "Bayar" << R << "\n";
    garis('─', 55);
    for (int i = 0; i < jumlahTransaksi; i++) {
        if (daftarTransaksi[i].idPelanggan == idPel) {
            cout << "  " << YL << setw(5) << daftarTransaksi[i].idTransaksi << R << "  "
                 << left << setw(24) << getNamaLayanan(daftarTransaksi[i].idLayanan).substr(0,22)
                 << setw(14) << warnaStatus(daftarTransaksi[i].status)
                 << warnaStatus(daftarTransaksi[i].statusBayar) << "\n";
            ada = true;
        }
    }
    if (!ada) {
        tampilkanPesan("info", "Anda belum memiliki order.");
        return;
    }
    garis('═', 55);

    int id = inputAngka("  Masukkan ID untuk lihat struk (0=batal): ");
    if (id == 0) return;
    int idx = cariIdxTransaksi(id);
    if (idx == -1 || daftarTransaksi[idx].idPelanggan != idPel) {
        tampilkanPesan("error", "Transaksi tidak ditemukan atau bukan milik Anda!");
        return;
    }
    tampilkanStruk(idx);
}

void batalkanOrder() {
    int idPel = daftarPelanggan[idxPelangganAktif].idPelanggan;
    cout << RD << B << "  BATALKAN ORDER\n" << R;
    garis('═', 55);

    bool ada = false;
    cout << B << "  " << left << setw(7) << "ID" << setw(24) << "Layanan" << "Status" << R << "\n";
    garis('─', 55);
    for (int i = 0; i < jumlahTransaksi; i++) {
        if (daftarTransaksi[i].idPelanggan == idPel) {
            cout << "  " << YL << setw(5) << daftarTransaksi[i].idTransaksi << R << "  "
                 << left << setw(24) << getNamaLayanan(daftarTransaksi[i].idLayanan).substr(0,22)
                 << warnaStatus(daftarTransaksi[i].status) << "\n";
            ada = true;
        }
    }
    if (!ada) {
        tampilkanPesan("info", "Tidak ada order yang bisa dibatalkan.");
        return;
    }
    garis('═', 55);

    int id = inputAngka("  Masukkan ID order yang dibatalkan (0=batal): ");
    if (id == 0) return;
    int idx = cariIdxTransaksi(id);
    if (idx == -1 || daftarTransaksi[idx].idPelanggan != idPel) {
        tampilkanPesan("error", "Transaksi tidak ditemukan atau bukan milik Anda!");
        return;
    }

    if (daftarTransaksi[idx].status != "Menunggu") {
        tampilkanPesan("peringatan", "Order sudah diproses/selesai, tidak bisa dibatalkan!");
        return;
    }

    if (!konfirmasi("  Yakin ingin membatalkan order ini?")) {
        tampilkanPesan("info", "Pembatalan dibatalkan.");
        return;
    }

    daftarTransaksi[idx].status = "Dibatalkan";
    simpanData();
    tampilkanPesan("sukses", "Order berhasil dibatalkan.");
}

void menuPelanggan() {
    bool aktif = true;
    while (aktif) {
        bersihkanLayar(); tampilkanHeader();
        cout << BL << B << "  ╔══════════════════════════════════════╗\n";
        cout << "  ║  MENU PELANGGAN  │  " << R << left << setw(17)
             << daftarPelanggan[idxPelangganAktif].nama.substr(0,15) << BL << B << "║\n";
        cout << "  ╚══════════════════════════════════════╝\n" << R;
        garis();
        cout << "  " << CY << "1" << R << ". Lihat Layanan Tersedia\n";
        cout << "  " << CY << "2" << R << ". Buat Order Baru\n";
        cout << "  " << CY << "3" << R << ". Lihat Order & Struk Saya\n";
        cout << "  " << CY << "4" << R << ". Batalkan Order\n";
        cout << "  " << RD << "5" << R << ". Logout\n";
        garis();
        int p = inputAngka("  Pilih: ");
        switch (p) {
            case 1: bersihkanLayar(); tampilkanHeader(); lihatLayananTersedia(); jedaLayar(); break;
            case 2: bersihkanLayar(); tampilkanHeader(); buatOrderBaru();        jedaLayar(); break;
            case 3: bersihkanLayar(); tampilkanHeader(); lihatOrderStrukSaya();  jedaLayar(); break;
            case 4: bersihkanLayar(); tampilkanHeader(); batalkanOrder();        jedaLayar(); break;
            case 5: aktif = false; idxPelangganAktif = -1; break;
            default:
                tampilkanPesan("error", "Pilihan tidak valid! Masukkan angka 1-5.");
                jedaLayar();
        }
    }
}

// ═══════════════════════════════════════════════════════════
//  MENU ADMIN
// ═══════════════════════════════════════════════════════════
void menuAdmin() {
    bool aktif = true;
    while (aktif) {
        bersihkanLayar(); tampilkanHeader();
        cout << YL << B << "  ╔══════════════════════════════════════╗\n";
        cout << "  ║  DASHBOARD ADMIN  │  " << R << left << setw(16)
             << dataAdmin.username << YL << B << "║\n";
        cout << "  ╚══════════════════════════════════════╝\n" << R;
        garis();
        cout << "  " << CY << "1" << R << ". Kelola Layanan\n";
        cout << "  " << CY << "2" << R << ". Kelola Transaksi\n";
        cout << "  " << CY << "3" << R << ". Sorting & Searching\n";
        cout << "  " << RD << "4" << R << ". Logout\n";
        garis();
        int p = inputAngka("  Pilih: ");
        switch (p) {
            case 1: menuKelolalayanan();    break;
            case 2: menuKelolaTransaksi();  break;
            case 3: menuSortingSearching(); break;
            case 4: aktif = false; break;
            default:
                tampilkanPesan("error", "Pilihan tidak valid! Masukkan angka 1-4.");
                jedaLayar();
        }
    }
}

// ═══════════════════════════════════════════════════════════
//  MENU UTAMA
// ═══════════════════════════════════════════════════════════
void menuUtama() {
    bool berjalan = true;
    while (berjalan) {
        bersihkanLayar(); tampilkanHeader();
        cout << CY << "  Selamat Datang di SplengerWash!\n" << R;
        cout << DM << IT << "  Solusi laundry terbaik untuk Anda.\n\n" << R;
        garis();
        cout << "  " << CY << "1" << R << ". Login sebagai Admin\n";
        cout << "  " << CY << "2" << R << ". Login sebagai Pelanggan\n";
        cout << "  " << CY << "3" << R << ". Daftar sebagai Pelanggan Baru\n";
        cout << "  " << RD << "4" << R << ". Keluar\n";
        garis();
        int p = inputAngka("  Pilih: ");
        switch (p) {
            case 1:
                if (loginAdmin()) menuAdmin();
                break;
            case 2:
                if (loginPelanggan()) menuPelanggan();
                break;
            case 3:
                daftarPelanggan_();
                break;
            case 4:
                simpanData();
                bersihkanLayar();
                cout << "\n\n";
                cout << CY << "  ╔══════════════════════════════════════════════════════════╗\n";
                cout << "  ║                                                          ║\n";
                cout << "  ║   " << R << GR << B << "Terima kasih telah menggunakan SplengerWash!" << R << CY << "       ║\n";
                cout << "  ║   " << R << DM << "Sampai jumpa dan selamat beraktivitas!            " << R << CY << "║\n";
                cout << "  ║                                                          ║\n";
                cout << "  ╚══════════════════════════════════════════════════════════╝\n" << R;
                cout << "\n\n";
                berjalan = false;
                break;
            default:
                tampilkanPesan("error", "Pilihan tidak valid! Masukkan angka 1-4.");
                jedaLayar();
        }
    }
}


