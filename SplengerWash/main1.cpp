#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <stdexcept>
using namespace std;

// ── Warna ANSI ────────────────────────────────────────────
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
const string BGR = "\033[41m";
const string BGG = "\033[42m";
const string BGY = "\033[43m";
const string BGC = "\033[46m";

// STRUCT

struct Layanan {
    int idLayanan;
    string namaLayanan;
    string jenis;
    float harga;
    string estimasi;
    string tambahan;
};

struct Pelanggan {
    int idPelanggan;
    string nama;
    string username;
    string password;
};

struct Admin {
    string username;
    string password;
};

struct Transaksi {
    int idTransaksi;
    int noAntrian;
    int idPelanggan;
    int idLayanan;
    float berat;
    int hargaTotal;
    string status;
    string statusBayar;
    string tglOrder;
    string tglSelesai;
};

// DEKLARASI


const int MAKS_LAYANAN = 20;
const int MAKS_PELANGGAN = 50;
const int MAKS_TRANSAKSI = 200;

Layanan daftarLayanan[MAKS_LAYANAN];
Pelanggan daftarPelanggan[MAKS_PELANGGAN];
Transaksi daftarTransaksi[MAKS_TRANSAKSI];

Admin dataAdmin = {"julpa", "julpa123"};

int jumlahLayanan = 0;
int jumlahPelanggan = 0;
int jumlahTransaksi = 0;

int nextIdLayanan = 1;
int nextIdPelanggan = 1;
int nextIdTransaksi = 1001;
int nextNoAntrian = 1;

bool loginSebagaiAdmin = false;
int idxPelangganAktif = -1;

// FUNCTION TAMBAHAN BIAR PROGRAM BISA RUN

void garis(char c = '=', int n = 50) {
    for (int i = 0; i < n; i++) cout << c;
    cout << endl;
}

void bersihkanLayar() {
    system("cls||clear");
}

void tampilkanHeader() {
    cout << CY << B << "\n=== SISTEM LAUNDRY ===\n" << R;
}

void jedaLayar() {
    cout << "\nTekan enter...";
    cin.ignore();
    cin.get();
}

void tampilkanPesan(string tipe, string pesan) {
    cout << pesan << endl;
}

bool konfirmasi(string text) {
    char pilih;
    cout << text << " (y/n): ";
    cin >> pilih;
    cin.ignore();
    return pilih == 'y' || pilih == 'Y';
}

string inputTeks(string text) {
    string x;
    cout << text;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, x);
    return x;
}

string inputTeksBolehKosong(string text) {
    string x;
    cout << text;
    getline(cin, x);
    return x;
}

int inputAngka(string text) {
    int x;
    cout << text;
    cin >> x;
    cin.ignore();
    return x;
}

int inputAngkaBatas(string text, int min, int max) {
    int x;
    do {
        cout << text;
        cin >> x;
        cin.ignore();
    } while (x < min || x > max);

    return x;
}

float inputDesimalPositif(string text) {
    float x;
    do {
        cout << text;
        cin >> x;
        cin.ignore();
    } while (x <= 0);

    return x;
}

int cariIdxLayanan(int id) {
    for (int i = 0; i < jumlahLayanan; i++) {
        if (daftarLayanan[i].idLayanan == id)
            return i;
    }
    return -1;
}

int cariIdxPelanggan(int id) {
    for (int i = 0; i < jumlahPelanggan; i++) {
        if (daftarPelanggan[i].idPelanggan == id)
            return i;
    }
    return -1;
}

int cariIdxTransaksi(int id) {
    for (int i = 0; i < jumlahTransaksi; i++) {
        if (daftarTransaksi[i].idTransaksi == id)
            return i;
    }
    return -1;
}

string getNamaPelanggan(int id) {
    for (int i = 0; i < jumlahPelanggan; i++) {
        if (daftarPelanggan[i].idPelanggan == id)
            return daftarPelanggan[i].nama;
    }
    return "-";
}

string getNamaLayanan(int id) {
    for (int i = 0; i < jumlahLayanan; i++) {
        if (daftarLayanan[i].idLayanan == id)
            return daftarLayanan[i].namaLayanan;
    }
    return "-";
}

string warnaStatus(string s) {
    return s;
}

string tanggalHariIni() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    stringstream ss;
    ss << 1900 + ltm->tm_year << "-"
       << 1 + ltm->tm_mon << "-"
       << ltm->tm_mday;

    return ss.str();
}

// deklarasi fungsi
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
        daftarLayanan[0] = {1, "Paket Hemat (Cuci + Setrika)", "Kiloan", 6000,  "2 Hari", "Setrika + Lipat"};
        daftarLayanan[1] = {2, "Paket Express",                "Kiloan", 10000, "1 Hari", "Setrika + Lipat"};
        daftarLayanan[2] = {3, "Cuci Kering",                  "Kiloan", 4000,  "2 Hari", "Tanpa Setrika"};
        daftarLayanan[3] = {4, "Setrika Saja",                 "Kiloan", 3000,  "1 Hari", "Setrika + Lipat"};
        daftarLayanan[4] = {5, "Cuci Selimut / Bedcover",      "Satuan", 15000, "3 Hari", "Tanpa Setrika"};
        daftarLayanan[5] = {6, "Cuci Sepatu",                  "Satuan", 20000, "2 Hari", "Tanpa Setrika"};
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

void tampilkanStruk(int idx) {
    if (idx < 0 || idx >= jumlahTransaksi) return;

    cout << "\n=== STRUK ===\n";
    cout << "ID : " << daftarTransaksi[idx].idTransaksi << endl;
    cout << "Pelanggan : "
         << getNamaPelanggan(daftarTransaksi[idx].idPelanggan) << endl;
    cout << "Layanan : "
         << getNamaLayanan(daftarTransaksi[idx].idLayanan) << endl;
    cout << "Status : "
         << daftarTransaksi[idx].status << endl;
}

void lihatSemuaLayanan() {
    cout << MG << "  DAFTAR LAYANAN\n" << R;
    garis();
    if (jumlahLayanan == 0) { cout << DM << "  Belum ada layanan.\n" << R; return; }
    cout << B << left
         << setw(5)  << "  ID"
         << setw(32) << "Nama Layanan"
         << setw(9)  << "Jenis"
         << setw(10) << "Harga"
         << setw(9)  << "Estimasi"
         << "Tambahan" << R << "\n";
    garis();
    for (int i = 0; i < jumlahLayanan; i++) {
        Layanan& l = daftarLayanan[i];
        cout << "  " << YL << setw(3) << l.idLayanan << R << "  "
             << left << setw(32) << l.namaLayanan
             << setw(9)  << l.jenis
             << GR << setw(10) << ("Rp " + to_string((int)l.harga)) << R
             << setw(9)  << l.estimasi
             << l.tambahan << "\n";
    }
    garis();
}

// LOGIN ADMIN

bool loginAdmin() {
    cout << "=== LOGIN ADMIN ===\n";

    loginSebagaiAdmin = false;
    idxPelangganAktif = -1;

    int percobaan = 0;

    while (percobaan < 3) {
        string u, p;

        cout << "Username : ";
        cin >> u;

        cout << "Password : ";
        cin >> p;

        if (u == dataAdmin.username && p == dataAdmin.password) {
            loginSebagaiAdmin = true;

            cout << "\nLogin berhasil! Selamat datang, "
                 << dataAdmin.username << ".\n";
            return true;
        }

        percobaan++;
        cout << "Login gagal! Sisa percobaan: "
             << (3 - percobaan) << "\n";
    }

    cout << "3x gagal. Kembali ke menu utama.\n";
    return false;
}

// LOGIN PELANGGAN

bool loginPelanggan() {
    cout << "=== LOGIN PELANGGAN ===\n";

    loginSebagaiAdmin = false;
    idxPelangganAktif = -1;

    int percobaan = 0;

    while (percobaan < 3) {
        string u, p;

        cout << "Username : ";
        cin >> u;

        cout << "Password : ";
        cin >> p;

        for (int i = 0; i < jumlahPelanggan; i++) {
            if (daftarPelanggan[i].username == u &&
                daftarPelanggan[i].password == p) {

                idxPelangganAktif = i;

                cout << "\nSelamat datang, "
                     << daftarPelanggan[i].nama << "!\n";
                return true;
            }
        }

        percobaan++;
        cout << "Login gagal! Sisa percobaan: "
             << (3 - percobaan) << "\n";
    }

    cout << "3x gagal. Kembali ke menu utama.\n";
    return false;
}

// DAFTAR PELANGGAN

void daftarPelanggan_() {
    cout << "=== DAFTAR PELANGGAN BARU ===\n";

    if (jumlahPelanggan >= MAKS_PELANGGAN) {
        cout << "Kapasitas pengguna penuh.\n";
        return;
    }

    string nama, user, pass;

    cout << "Nama lengkap : ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, nama);

    while (true) {
        cout << "Username : ";
        cin >> user;

        bool ada = false;
        for (int i = 0; i < jumlahPelanggan; i++) {
            if (daftarPelanggan[i].username == user) {
                ada = true;
                break;
            }
        }

        if (!ada) break;

        cout << "Username sudah digunakan, coba lain.\n";
    }

    cout << "Password : ";
    cin >> pass;

    daftarPelanggan[jumlahPelanggan] = {
        nextIdPelanggan++, nama, user, pass
    };

    jumlahPelanggan++;

    cout << "\nRegistrasi berhasil! Silakan login.\n";
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
    garis('=', 62);
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
    garis('-', 62);
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
    garis('=', 62);
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
    garis('-', 40);
    for (int i = 0; i < jumlahPelanggan; i++)
        cout << "  " << YL << daftarPelanggan[i].idPelanggan << R
             << ". " << daftarPelanggan[i].nama << " (" << DM << daftarPelanggan[i].username << R << ")\n";
    garis('-', 40);

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

// ============================================================
//  BAGIAN SORTING
// ============================================================

void merge_(Transaksi* arr, int kiri, int tengah, int kanan,
            Pelanggan* pel, int jmlPel) {
    int n1 = tengah - kiri + 1;
    int n2 = kanan - tengah;

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
        for (char& c : nL) c = tolower(c);
        for (char& c : nR) c = tolower(c);

        if (nL <= nR) arr[k++] = L[i++];
        else          arr[k++] = R_[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R_[j++];

    delete[] L;
    delete[] R_;
}

void mergeSortNama(Transaksi* arr, int kiri, int kanan, Pelanggan* pel, int jmlPel) {
    if (kiri < kanan) {
        int tengah = kiri + (kanan - kiri) / 2;
        mergeSortNama(arr, kiri,      tengah, pel, jmlPel);
        mergeSortNama(arr, tengah + 1, kanan, pel, jmlPel);
        merge_(arr, kiri, tengah, kanan, pel, jmlPel);
    }
}

int partisiQuick(Transaksi* arr, int lo, int hi) {
    int pivot = arr[hi].hargaTotal;
    int i     = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j].hargaTotal >= pivot) {
            i++;
            Transaksi tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        }
    }
    Transaksi tmp = arr[i + 1]; arr[i + 1] = arr[hi]; arr[hi] = tmp;
    return i + 1;
}

void quickSortHarga(Transaksi* arr, int lo, int hi) {
    if (lo < hi) {
        int p = partisiQuick(arr, lo, hi);
        quickSortHarga(arr, lo,    p - 1);
        quickSortHarga(arr, p + 1, hi);
    }
}

void tampilkanRingkasTransaksi(Transaksi* arr, int jumlah) {
    cout << B << left
         << setw(7)  << "  Antri"
         << setw(7)  << "ID"
         << setw(18) << "Nama Pelanggan"
         << setw(14) << "Harga"
         << "Status" << R << "\n";
    garis();
    for (int i = 0; i < jumlah; i++) {
        string hargaStr = (arr[i].hargaTotal == 0)
                          ? DM + string("-") + R
                          : GR + "Rp " + to_string(arr[i].hargaTotal) + R;
        cout << "  " << YL << setw(5) << ("#" + to_string(arr[i].noAntrian)) << R << "  "
             << left
             << setw(7)  << arr[i].idTransaksi
             << setw(18) << getNamaPelanggan(arr[i].idPelanggan)
             << setw(14) << hargaStr
             << warnaStatus(arr[i].status) << "\n";
    }
    garis();
}

void sortingData() {
    if (jumlahTransaksi == 0) {
        cout << DM << "  Belum ada data transaksi.\n" << R;
        return;
    }

    cout << MG << "  SORTING DATA\n" << R;
    garis();
    cout << "  1. Nama Pelanggan  (Merge Sort  - Ascending A-Z)\n";
    cout << "  2. Total Bayar     (Quick Sort  - Descending)\n";
    garis();

    int p;
    while (true) {
        p = inputAngka("  Pilih (1-2): ");
        if (p == 1 || p == 2) break;
        cout << RD << "  Pilihan tidak valid!\n" << R;
    }

    cout << "\n" << YL << "  DATA SEBELUM SORTING:\n" << R;
    tampilkanRingkasTransaksi(daftarTransaksi, jumlahTransaksi);

    if (p == 1) {
        mergeSortNama(daftarTransaksi, 0, jumlahTransaksi - 1,
                      daftarPelanggan, jumlahPelanggan);
        cout << GR << "\n  Merge Sort Nama Pelanggan (A-Z) selesai.\n" << R;
    } else {
        quickSortHarga(daftarTransaksi, 0, jumlahTransaksi - 1);
        cout << GR << "\n  Quick Sort Harga Total (Tertinggi ke Terendah) selesai.\n" << R;
    }

    cout << "\n" << GR << "  DATA SESUDAH SORTING:\n" << R;
    tampilkanRingkasTransaksi(daftarTransaksi, jumlahTransaksi);

    simpanData();
}


// ============================================================
//  BAGIAN SEARCHING
// ============================================================

int binarySearchId(int targetId) {
    Transaksi temp[MAKS_TRANSAKSI];
    for (int i = 0; i < jumlahTransaksi; i++) temp[i] = daftarTransaksi[i];

    for (int i = 0; i < jumlahTransaksi - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < jumlahTransaksi; j++)
            if (temp[j].idTransaksi < temp[minIdx].idTransaksi) minIdx = j;
        Transaksi t = temp[i]; temp[i] = temp[minIdx]; temp[minIdx] = t;
    }

    int lo = 0, hi = jumlahTransaksi - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (temp[mid].idTransaksi == targetId) {
            return cariIdxTransaksi(targetId);
        } else if (temp[mid].idTransaksi < targetId) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return -1;
}

void linearSearchNama(const string& nama) {
    string namaLow = nama;
    for (char& c : namaLow) c = tolower(c);

    bool ada = false;
    cout << "\n" << CY << "  Hasil pencarian nama '" << nama << "':\n" << R;
    garis();

    for (int i = 0; i < jumlahTransaksi; i++) {
        string np    = getNamaPelanggan(daftarTransaksi[i].idPelanggan);
        string npLow = np;
        for (char& c : npLow) c = tolower(c);

        if (npLow.find(namaLow) != string::npos) {
            tampilkanStruk(i);
            ada = true;
        }
    }

    if (!ada) cout << RD << "  Nama pelanggan tidak ditemukan.\n" << R;
}

void cariTransaksi() {
    cout << BL << "  CARI TRANSAKSI\n" << R;
    garis();
    cout << "  1. Cari berdasarkan ID Transaksi   (Binary Search)\n";
    cout << "  2. Cari berdasarkan Nama Pelanggan (Linear Search)\n";
    garis();

    int p;
    while (true) {
        p = inputAngka("  Pilih (1/2): ");
        if (p == 1 || p == 2) break;
        cout << RD << "  Pilihan tidak valid!\n" << R;
    }

    if (p == 1) {
        int id  = inputAngka("  Masukkan ID Transaksi: ");
        int idx = binarySearchId(id);
        if (idx == -1) {
            cout << RD << "  ID transaksi tidak ditemukan.\n" << R;
        } else {
            cout << GR << "  Transaksi ditemukan!\n" << R;
            tampilkanStruk(idx);
        }
    } else {
        string nama = inputTeks("  Masukkan nama pelanggan: ");
        linearSearchNama(nama);
    }
}

// ============================================================
//  BAGIAN MENU PELANGGAN
// ============================================================

void lihatLayananTersedia() {
    cout << MG << "  DAFTAR LAYANAN TERSEDIA\n" << R;
    garis();
    if (jumlahLayanan == 0) {
        cout << DM << "  Belum ada layanan tersedia.\n" << R;
        return;
    }
    cout << B << left
         << setw(5)  << "  ID"
         << setw(32) << "Nama Layanan"
         << setw(9)  << "Jenis"
         << setw(12) << "Harga"
         << setw(9)  << "Estimasi"
         << "Tambahan" << R << "\n";
    garis();
    for (int i = 0; i < jumlahLayanan; i++) {
        Layanan& l = daftarLayanan[i];
        string satuanHarga = (l.jenis == "Kiloan") ? "/kg" : "/pcs";
        cout << "  " << YL << setw(3) << l.idLayanan << R << "  "
             << left
             << setw(32) << l.namaLayanan
             << setw(9)  << l.jenis
             << GR << setw(12) << ("Rp " + to_string((int)l.harga) + satuanHarga) << R
             << setw(9)  << l.estimasi
             << l.tambahan << "\n";
    }
    garis();
}

void buatOrderBaru() {
    if (jumlahTransaksi >= MAKS_TRANSAKSI) {
        cout << RD << "  Sistem penuh, hubungi admin.\n" << R;
        return;
    }
    if (jumlahLayanan == 0) {
        cout << RD << "  Belum ada layanan tersedia, hubungi admin.\n" << R;
        return;
    }

    lihatLayananTersedia();
    cout << "\n";

    int idLay;
    int idxLay = -1;
    while (idxLay == -1) {
        idLay  = inputAngka("  Pilih ID Layanan: ");
        idxLay = cariIdxLayanan(idLay);
        if (idxLay == -1) cout << RD << "  Layanan tidak ditemukan, ulangi.\n" << R;
    }

    Layanan& l = daftarLayanan[idxLay];
    cout << "\n  Detail layanan yang dipilih:\n";
    garis('-', 40);
    cout << "  Nama     : " << l.namaLayanan << "\n";
    cout << "  Jenis    : " << l.jenis << "\n";
    cout << "  Harga    : " << GR << "Rp " << (int)l.harga
         << R << " / " << (l.jenis == "Kiloan" ? "kg" : "pcs") << "\n";
    cout << "  Estimasi : " << l.estimasi << "\n";
    cout << "  Tambahan : " << l.tambahan << "\n";
    garis('-', 40);

    cout << CY << "  Lanjutkan order? (y/n): " << R;
    string k; getline(cin, k);
    if (k != "y" && k != "Y") {
        cout << DM << "  Order dibatalkan.\n" << R;
        return;
    }

    Transaksi trx;
    trx.idTransaksi = nextIdTransaksi++;
    trx.noAntrian   = nextNoAntrian++;
    trx.idPelanggan = daftarPelanggan[idxPelangganAktif].idPelanggan;
    trx.idLayanan   = idLay;
    trx.berat       = 0;
    trx.hargaTotal  = 0;
    trx.status      = "Menunggu";
    trx.statusBayar = "Belum Bayar";
    trx.tglOrder    = tanggalHariIni();
    trx.tglSelesai  = "";

    daftarTransaksi[jumlahTransaksi++] = trx;
    simpanData();

    cout << GR << "\n  Order berhasil dibuat!\n" << R;
    cout << "  Nomor Antrian : " << YL << B << "#" << trx.noAntrian << R << "\n";
    cout << "  ID Order      : " << trx.idTransaksi << "\n";
    cout << DM << "  Silakan antar pakaian ke toko.\n" << R;
}

void lihatOrderStrukSaya() {
    int idPel = daftarPelanggan[idxPelangganAktif].idPelanggan;
    cout << BL << "  ORDER & STRUK SAYA\n" << R;
    garis();

    bool ada = false;
    cout << B << left
         << setw(7)  << "  ID"
         << setw(22) << "Layanan"
         << setw(14) << "Status"
         << "Bayar" << R << "\n";
    garis();
    for (int i = 0; i < jumlahTransaksi; i++) {
        if (daftarTransaksi[i].idPelanggan == idPel) {
            cout << "  " << YL << setw(5) << daftarTransaksi[i].idTransaksi << R << "  "
                 << left
                 << setw(22) << getNamaLayanan(daftarTransaksi[i].idLayanan).substr(0, 20)
                 << setw(14) << warnaStatus(daftarTransaksi[i].status)
                 << warnaStatus(daftarTransaksi[i].statusBayar) << "\n";
            ada = true;
        }
    }
    if (!ada) {
        cout << DM << "  Belum ada order.\n" << R;
        return;
    }
    garis();

    int id = inputAngka("  Masukkan ID untuk lihat struk (0=batal): ");
    if (id == 0) return;

    int idx = cariIdxTransaksi(id);
    if (idx == -1 || daftarTransaksi[idx].idPelanggan != idPel) {
        cout << RD << "  Transaksi tidak ditemukan.\n" << R;
        return;
    }
    tampilkanStruk(idx);
}

void batalkanOrder() {
    int idPel = daftarPelanggan[idxPelangganAktif].idPelanggan;
    cout << RD << "  BATALKAN ORDER\n" << R;
    garis();

    bool ada = false;
    cout << B << left
         << setw(7)  << "  ID"
         << setw(22) << "Layanan"
         << "Status" << R << "\n";
    garis();
    for (int i = 0; i < jumlahTransaksi; i++) {
        if (daftarTransaksi[i].idPelanggan == idPel) {
            cout << "  " << YL << setw(5) << daftarTransaksi[i].idTransaksi << R << "  "
                 << left
                 << setw(22) << getNamaLayanan(daftarTransaksi[i].idLayanan).substr(0, 20)
                 << warnaStatus(daftarTransaksi[i].status) << "\n";
            ada = true;
        }
    }
    if (!ada) {
        cout << DM << "  Tidak ada order yang bisa dibatalkan.\n" << R;
        return;
    }
    garis();

    int id = inputAngka("  Masukkan ID order yang dibatalkan (0=batal): ");
    if (id == 0) return;

    int idx = cariIdxTransaksi(id);
    if (idx == -1 || daftarTransaksi[idx].idPelanggan != idPel) {
        cout << RD << "  Transaksi tidak ditemukan.\n" << R;
        return;
    }

    if (daftarTransaksi[idx].status != "Menunggu") {
        cout << RD << "  Order sudah diproses, tidak bisa dibatalkan.\n" << R;
        return;
    }

    cout << CY << "  Yakin batalkan order ini? (y/n): " << R;
    string k; getline(cin, k);
    if (k != "y" && k != "Y") {
        cout << DM << "  Pembatalan dibatalkan.\n" << R;
        return;
    }

    daftarTransaksi[idx].status = "Dibatalkan";
    simpanData();
    cout << GR << "  Order berhasil dibatalkan.\n" << R;
}

// =========================================================
//  Menu utama admin
// =========================================================

void menuAdmin() {
    bool aktif = true;
    while (aktif) {
        bersihkanLayar(); tampilkanHeader();
        cout << BL << B << "  ╔══════════════════════════╗\n";
        cout << "  ║     MENU ADMIN           ║\n";
        cout << "  ║  " << setw(20) << left << dataAdmin.username << " ║\n";
        cout << "  ╚══════════════════════════╝\n" << R;
        garis();
        cout << "  " << CY << "1" << R << ". Kelola Layanan\n";
        cout << "  " << CY << "2" << R << ". Kelola Transaksi\n";
        cout << "  " << CY << "3" << R << ". Sorting Data\n";
        cout << "  " << CY << "4" << R << ". Cari Transaksi\n";
        cout << "  " << RD << "5" << R << ". Logout\n";
        garis();
        int p = inputAngka("  Pilih: ");
        switch (p) {
            case 1:
                bersihkanLayar(); tampilkanHeader();
                menuKelolalayanan();
                break;
            case 2:
                bersihkanLayar(); tampilkanHeader();
                menuKelolaTransaksi();
                break;
            case 3:
                bersihkanLayar(); tampilkanHeader();
                sortingData();
                jedaLayar();
                break;
            case 4:
                bersihkanLayar(); tampilkanHeader();
                cariTransaksi();
                jedaLayar();
                break;
            case 5:
                aktif = false;
                loginSebagaiAdmin = false;
                cout << YL << "\n  Logout berhasil. Sampai jumpa!\n" << R;
                jedaLayar();
                break;
            default:
                cout << RD << "  Pilihan tidak valid! Masukkan angka 1-5.\n" << R;
                jedaLayar();
        }
    }
}

// =========================================================
//  Menu utama pelanggan
// =========================================================
void menuPelanggan() {
    bool aktif = true;
    while (aktif) {
        bersihkanLayar(); tampilkanHeader();
        cout << BL << "  MENU PELANGGAN  |  "
             << daftarPelanggan[idxPelangganAktif].nama << "\n" << R;
        garis();
        cout << "  1. Lihat Layanan Tersedia\n";
        cout << "  2. Buat Order Baru\n";
        cout << "  3. Lihat Order & Struk Saya\n";
        cout << "  4. Batalkan Order\n";
        cout << "  5. Logout\n";
        garis();
        int p = inputAngka("  Pilih: ");
        switch (p) {
            case 1: bersihkanLayar(); tampilkanHeader(); lihatLayananTersedia(); jedaLayar(); break;
            case 2: bersihkanLayar(); tampilkanHeader(); buatOrderBaru();        jedaLayar(); break;
            case 3: bersihkanLayar(); tampilkanHeader(); lihatOrderStrukSaya();  jedaLayar(); break;
            case 4: bersihkanLayar(); tampilkanHeader(); batalkanOrder();        jedaLayar(); break;
            case 5:
                aktif = false;
                idxPelangganAktif = -1;
                cout << YL << "\n  Logout berhasil. Sampai jumpa!\n" << R;
                jedaLayar();
                break;
            default:
                cout << RD << "  Pilihan tidak valid.\n" << R;
                jedaLayar();
        }
    }
}

// MAIN

int main() {
    system("chcp 65001 > nul");
    muatData();
    int pilihan;

    do {
        bersihkanLayar(); tampilkanHeader();
        cout << MG << B << "  ╔══════════════════════════╗\n";
        cout << "  ║     MENU UTAMA           ║\n";
        cout << "  ╚══════════════════════════╝\n" << R;
        garis();
        cout << "  " << CY << "1" << R << ". Login Admin\n";
        cout << "  " << CY << "2" << R << ". Login Pelanggan\n";
        cout << "  " << CY << "3" << R << ". Daftar Pelanggan\n";
        cout << "  " << RD << "0" << R << ". Keluar\n";
        garis();
        pilihan = inputAngka("  Pilih: ");

        switch (pilihan) {
            case 1:
                bersihkanLayar(); tampilkanHeader();
                if (loginAdmin()) {
                    menuAdmin();
                }
                break;

            case 2:
                bersihkanLayar(); tampilkanHeader();
                if (loginPelanggan()) {
                    menuPelanggan();
                }
                break;

            case 3:
                bersihkanLayar(); tampilkanHeader();
                daftarPelanggan_();
                jedaLayar();
                break;

            case 0:
                cout << GR << "\n  Terima kasih!\n" << R;
                break;

            default:
                cout << RD << "  Pilihan tidak valid! Masukkan angka 0-3.\n" << R;
                jedaLayar();
        }

    } while (pilihan != 0);

    return 0;
}