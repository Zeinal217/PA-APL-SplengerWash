#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cctype>
#include <stdexcept>
#include <thread>
#include <chrono> 
#ifdef _WIN32
#include <windows.h>
#endif
using namespace std;

const string R  = "\033[0m"; // Reset
const string B  = "\033[1m"; // Bold
const string DM = "\033[2m"; //Dim (redup)
const string IT = "\033[3m"; // Italic miring)
const string RD = "\033[31m"; // Red
const string GR = "\033[32m"; // Green
const string YL = "\033[33m"; // Yellow
const string BL = "\033[34m"; // Blue
const string MG = "\033[35m"; // Magenta
const string CYN = "\033[36m"; // Cyan

struct Layanan {
    int idLayanan;
    string namaLayanan;
    string jenis;
    double harga;
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
    double berat;
    long long hargaTotal;
    string status;
    string statusBayar;
    string tglOrder;
    string tglSelesai;
    string namaWalkin;
    string namaPelanggan;
};

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

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool hasLeadingTrailingSpace(const string& str) {
    if (str.empty()) return false;
    return str.front() == ' ' || str.back() == ' ' || str.front() == '\t' || str.back() == '\t';
}

void garis(char c, int n) {
    for (int i = 0; i < n; ++i) cout << c;
    cout << '\n';
}

void garis(int n = 50) { garis('=', n); }

void bersihkanLayar() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

void tampilkanHeader() {
    cout << CYN << B;
    cout << "\n";
    cout << "  ╔═════════════════════════════════════════════════╗\n";
    cout << "  ║      SplengerWash - Sistem Manajemen Laundry    ║\n";
    cout << "  ╚═════════════════════════════════════════════════╝\n";
    cout << R;
}

void jedaLayar() {
    cout << "\nTekan enter...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void tampilkanLoading() {
    const int lebarBar = 40;
    const int durasiStep = 40;
    cout << "\n" << GR << "  Memuat data sistem " << R;
    cout.flush();
    for (int i = 0; i <= lebarBar; ++i) {
        cout << "\r  Memuat data sistem " << GR << "[";
        for (int j = 0; j < lebarBar; ++j) cout << (j < i ? "█" : " ");
        cout << "]" << R; cout.flush();
        this_thread::sleep_for(chrono::milliseconds(durasiStep));
    }
    cout << " Selesai!\n";
}

void tampilkanPesan(const string& tipe, const string& pesan) {
    if (tipe == "error")     cout << RD << "  [!] " << pesan << R << "\n";
    else if (tipe == "sukses") cout << GR << "  [v] " << pesan << R << "\n";
    else if (tipe == "info")   cout << CYN << "  [i] " << pesan << R << "\n";
    else if (tipe == "peringatan") cout << YL << "  [~] " << pesan << R << "\n";
    else cout << pesan << "\n";
}

bool konfirmasi(const string& text) {
    string pilih;
    while (true) {
        cout << text << " (y/n): ";
        getline(cin, pilih);
        pilih = trim(pilih);
        if (pilih == "y" || pilih == "Y") return true;
        if (pilih == "n" || pilih == "N") return false;
        cout << RD << "  Input tidak valid! Ketik y atau n.\n" << R;
    }
}

bool hanyaHuruf(const string& s) {
    for (char c : s)
        if (!isalpha(static_cast<unsigned char>(c)) && c != ' ') return false;
    return true;
}

bool hanyaHurufAngka(const string& s) {
    for (char c : s)
        if (!isalnum(static_cast<unsigned char>(c))) return false;
    return true;
}

bool formatDesimal(const string& s) {
    int titik = 0;
    for (char c : s) {
        if (c == '.') { ++titik; if (titik > 1) return false; }
        else if (!isdigit(static_cast<unsigned char>(c))) return false;
    }
    return !s.empty();
}

string inputTeks(const string& text) {
    string x;
    while (true) {
        cout << text;
        getline(cin, x);
        x = trim(x);
        if (!x.empty()) return x;
        cout << RD << "  Input tidak boleh kosong!\n" << R;
    }
}

string inputNama(const string& text) {
    string x;
    while (true) {
        cout << text;
        getline(cin, x);
        if (x == "0") return "0";
        if (hasLeadingTrailingSpace(x)) { cout << RD << "  Nama tidak boleh memiliki spasi di awal/akhir!\n" << R; continue; }
        x = trim(x);
        if (x.empty()) { cout << RD << "  Nama tidak boleh kosong!\n" << R; continue; }
        if (!hanyaHuruf(x)) { cout << RD << "  Nama hanya boleh huruf dan spasi!\n" << R; continue; }
        if (x.length() < 3)  { cout << RD << "  Nama minimal 3 karakter!\n" << R; continue; }
        if (x.length() > 50) { cout << RD << "  Nama maksimal 50 karakter!\n" << R; continue; }
        return x;
    }
}

string inputTeksBolehKosong(const string& text) {
    string x;
    cout << text;
    getline(cin, x);
    return trim(x);
}

int inputAngka(const string& text) {
    string s;
    while (true) {
        try {
            cout << text;
            getline(cin, s);
            if (s.find_first_of(" \t") != string::npos) {
                cout << RD << "  Input tidak boleh mengandung spasi!\n" << R;
                continue;
            }
            s = trim(s);
            if (s.empty()) { cout << RD << "  Input tidak boleh kosong!\n" << R; continue; }
            if (s[0] == '-') {
                cout << RD << "  Input tidak boleh negatif!\n" << R;
                continue;
            }
            size_t pos;
            long long val = stoll(s, &pos);
            if (pos != s.length()) throw invalid_argument("");
            if (val < INT_MIN || val > INT_MAX) throw out_of_range("");
            return static_cast<int>(val);
        } catch (const invalid_argument&) {
            cout << RD << "  Input harus angka bulat!\n" << R;
        } catch (const out_of_range&) {
            cout << RD << "  Angka terlalu besar!\n" << R;
        } catch (...) {
            cout << RD << "  Error input angka!\n" << R;
        }
    }
}

int inputAngkaBatas(const string& text, int minVal, int maxVal) {
    while (true) {
        try {
            int x = inputAngka(text);
            if (x >= minVal && x <= maxVal) return x;
            cout << RD << "  Pilihan harus antara " << minVal << " dan " << maxVal << "!\n" << R;
        } catch (...) {
            cout << RD << "  Error validasi angka!\n" << R;
        }
    }
}

double inputHarga(const string& text) {
    string s;
    while (true) {
        try {
            cout << text;
            getline(cin, s);
            s = trim(s);
            if (s.empty()) { cout << RD << "  Harga tidak boleh kosong!\n" << R; continue; }
            if (!formatDesimal(s)) { cout << RD << "  Harga hanya boleh angka! Contoh: 5000\n" << R; continue; }
            double v = stod(s);
            if (v < 2000)       { cout << RD << "  Harga minimal Rp 2.000!\n" << R; continue; }
            if (v > 1000000)  { cout << RD << "  Harga maksimal Rp 1.000.000!\n" << R; continue; }
            return v;
        } catch (const invalid_argument&) {
            cout << RD << "  Format harga tidak valid!\n" << R;
        } catch (const out_of_range&) {
            cout << RD << "  Harga terlalu besar!\n" << R;
        } catch (...) {
            cout << RD << "  Error input harga!\n" << R;
        }
    }
}

double inputBerat(const string& text) {
    string s;
    while (true) {
        try {
            cout << text;
            getline(cin, s);
            s = trim(s);
            if (s.empty()) { cout << RD << "  Berat tidak boleh kosong!\n" << R; continue; }
            if (!formatDesimal(s)) { cout << RD << "  Berat hanya boleh angka! Contoh: 2.5\n" << R; continue; }
            double v = stod(s);
            if (v < 0.1)   { cout << RD << "  Berat minimal 0.1 kg!\n" << R; continue; }
            if (v > 50)  { cout << RD << "  Berat maksimal 50 kg!\n" << R; continue; }
            return v;
        } catch (const invalid_argument&) {
            cout << RD << "  Format berat tidak valid!\n" << R;
        } catch (const out_of_range&) {
            cout << RD << "  Berat terlalu besar!\n" << R;
        } catch (...) {
            cout << RD << "  Error input berat!\n" << R;
        }
    }
}

int cariIdxLayanan(int id) {
    for (int i = 0; i < jumlahLayanan; i++)
        if (daftarLayanan[i].idLayanan == id) return i;
    return -1;
}

int cariIdxPelanggan(int id) {
    for (int i = 0; i < jumlahPelanggan; i++)
        if (daftarPelanggan[i].idPelanggan == id) return i;
    return -1;
}

int cariIdxTransaksi(int id) {
    for (int i = 0; i < jumlahTransaksi; i++)
        if (daftarTransaksi[i].idTransaksi == id) return i;
    return -1;
}

string getNamaPelanggan(int id) {
    for (int i = 0; i < jumlahPelanggan; i++)
        if (daftarPelanggan[i].idPelanggan == id) return daftarPelanggan[i].nama;
    return "-";
}

string getNamaLayanan(int id) {
    for (int i = 0; i < jumlahLayanan; i++)
        if (daftarLayanan[i].idLayanan == id) return daftarLayanan[i].namaLayanan;
    return "-";
}

string getNamaAktual(Transaksi trx) {
    if (!trx.namaPelanggan.empty()) {
        return trx.namaPelanggan;
    }
    if (!trx.namaWalkin.empty()) {
        return trx.namaWalkin;
    }
    for (int i = 0; i < jumlahPelanggan; i++) {
        if (daftarPelanggan[i].idPelanggan == trx.idPelanggan) {
            return daftarPelanggan[i].nama;
        }
    }
    return "-";
}

string tanggalHariIni() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    stringstream ss;
    ss << 1900 + ltm->tm_year << "-" << 1 + ltm->tm_mon << "-" << ltm->tm_mday;
    return ss.str();
}

void simpanData() {
    try {
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
                fT << daftarTransaksi[i].namaWalkin  << "\n";
                fT << daftarTransaksi[i].namaPelanggan << "\n";
            }
            fT.close();
        }
    } catch (const exception& e) {
        tampilkanPesan("error", "Gagal menyimpan data: " + string(e.what()));
    }
}

void muatData() {
    try {
        ifstream fL("sw_layanan.txt");
        if (fL.is_open()) {
            fL >> jumlahLayanan >> nextIdLayanan;
            fL.ignore(numeric_limits<streamsize>::max(), '\n');
            if (jumlahLayanan > MAKS_LAYANAN) {
                tampilkanPesan("peringatan", "Data layanan melebihi kapasitas! Hanya " + to_string(MAKS_LAYANAN) + " data pertama yang dimuat.");
                jumlahLayanan = MAKS_LAYANAN;
            }
            for (int i = 0; i < jumlahLayanan; i++) {
                fL >> daftarLayanan[i].idLayanan >> daftarLayanan[i].harga;
                fL.ignore(numeric_limits<streamsize>::max(), '\n');
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
            fP.ignore(numeric_limits<streamsize>::max(), '\n');
            if (jumlahPelanggan > MAKS_PELANGGAN) {
                tampilkanPesan("peringatan", "Data pelanggan melebihi kapasitas! Hanya " + to_string(MAKS_PELANGGAN) + " data pertama yang dimuat.");
                jumlahPelanggan = MAKS_PELANGGAN;
            }
            for (int i = 0; i < jumlahPelanggan; i++) {
                fP >> daftarPelanggan[i].idPelanggan;
                fP.ignore(numeric_limits<streamsize>::max(), '\n');
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
            fT.ignore(numeric_limits<streamsize>::max(), '\n');
            if (jumlahTransaksi > MAKS_TRANSAKSI) {
                tampilkanPesan("peringatan", "Data transaksi melebihi kapasitas! Hanya " + to_string(MAKS_TRANSAKSI) + " data pertama yang dimuat.");
                jumlahTransaksi = MAKS_TRANSAKSI;
            }
            for (int i = 0; i < jumlahTransaksi; i++) {
                fT >> daftarTransaksi[i].idTransaksi
                    >> daftarTransaksi[i].noAntrian
                    >> daftarTransaksi[i].idPelanggan
                    >> daftarTransaksi[i].idLayanan
                    >> daftarTransaksi[i].berat
                    >> daftarTransaksi[i].hargaTotal;
                fT.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(fT, daftarTransaksi[i].status);
                getline(fT, daftarTransaksi[i].statusBayar);
                getline(fT, daftarTransaksi[i].tglOrder);
                getline(fT, daftarTransaksi[i].tglSelesai);
                if (fT.peek() != EOF) getline(fT, daftarTransaksi[i].namaWalkin); else daftarTransaksi[i].namaWalkin = "";
                if (fT.peek() != EOF) getline(fT, daftarTransaksi[i].namaPelanggan); else daftarTransaksi[i].namaPelanggan = "";
            }
            fT.close();
        }
    } catch (const exception& e) {
        tampilkanPesan("error", "Gagal memuat data: " + string(e.what()));
    }
}

void tampilkanStruk(int idx) {
    if (idx < 0 || idx >= jumlahTransaksi) return;
    const Transaksi& trx = daftarTransaksi[idx];
    
    cout << "\n";
    cout << B << "  ╔══════════════════════════════════════╗\n" << R;
    cout << B << "  ║" << R << "  SplengerWash - Struk Transaksi      " << B << "║\n" << R;
    cout << B << "  ╚══════════════════════════════════════╝\n" << R;
    cout << "\n";
    
    cout << left;
    cout << "  " << B << "ID Transaksi" << R << " : " << trx.idTransaksi << "\n";
    cout << "  " << B << "No. Antrian" << R << "  : #" << trx.noAntrian << "\n";
    cout << "  " << B << "Pelanggan" << R << "    : " << getNamaAktual(trx) << "\n";
    cout << "  " << B << "Layanan" << R << "      : " << getNamaLayanan(trx.idLayanan) << "\n";
    cout << "  " << B << "Berat" << R << "        : " << fixed << setprecision(2) << trx.berat << " kg\n";
    cout << "  " << B << "Harga Total" << R << "  : " << GR << B << "Rp " << trx.hargaTotal << R << "\n";
    cout << "  " << B << "Status" << R << "       : " << trx.status << "\n";
    cout << "  " << B << "Status Bayar" << R << " : " << trx.statusBayar << "\n";
    cout << "  " << B << "Tgl Order" << R << "    : " << trx.tglOrder << "\n";
    if (!trx.tglSelesai.empty()) {
        cout << "  " << B << "Tgl Selesai" << R << "  : " << trx.tglSelesai << "\n";
    }
    cout << "\n";
    cout << B << "  ═══════════════════════════════════════\n" << R;
}

void lihatSemuaLayanan() {
    cout << MG << "  DAFTAR LAYANAN\n" << R;
    garis();
    if (jumlahLayanan == 0) { cout << DM << "  Belum ada layanan.\n" << R; return; }
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
        cout << "  " << YL << setw(3) << l.idLayanan << R << "  "
            << left << setw(32) << l.namaLayanan
            << setw(9)  << l.jenis
            << GR << setw(12) << ("Rp " + to_string(static_cast<int>(l.harga))) << R
            << setw(9)  << l.estimasi
            << l.tambahan << "\n";
    }
    garis();
}

bool loginAdmin() {
    cout << "" << endl;
    cout << MG << B << "  LOGIN ADMIN\n" << R;
    cout << RD << "  (Ketik 0 untuk batal)\n" << R;
    cout << "" << endl;
    loginSebagaiAdmin = false;
    idxPelangganAktif = -1;
    int percobaan = 0;
    while (percobaan < 3) {
        string u, p;
        cout << "Username : ";
        getline(cin, u);
        if (u == "0") { tampilkanPesan("info", "Login dibatalkan."); return false; }
        if (hasLeadingTrailingSpace(u)) { tampilkanPesan("error", "Username tidak boleh memiliki spasi di awal/akhir!"); continue; }
        u = trim(u);
        cout << "Password : ";
        getline(cin, p);
        if (p == "0") { tampilkanPesan("info", "Login dibatalkan."); return false; }
        if (hasLeadingTrailingSpace(p)) { tampilkanPesan("error", "Password tidak boleh memiliki spasi di awal/akhir!"); continue; }
        p = trim(p);
        if (u == dataAdmin.username && p == dataAdmin.password) {
            loginSebagaiAdmin = true;
            return true;
        }
        ++percobaan;
        tampilkanPesan("error", "Login gagal! Sisa percobaan: " + to_string(3 - percobaan));
    }
    cout << "" << endl;
    tampilkanPesan("error", "3x gagal. Kembali ke menu utama.");
    jedaLayar();
    return false;
}

bool loginPelanggan() {
    cout << "" << endl;
    cout << MG << B << "  LOGIN PELANGGAN\n" << R;
    cout << RD << "  (Ketik 0 untuk batal)\n" << R;
    cout << "" << endl;
    loginSebagaiAdmin = false;
    idxPelangganAktif = -1;
    int percobaan = 0;
    while (percobaan < 3) {
        string u, p;
        cout << "Username : ";
        getline(cin, u);
        if (u == "0") { tampilkanPesan("info", "Login dibatalkan."); return false; }
        if (hasLeadingTrailingSpace(u)) { tampilkanPesan("error", "Username tidak boleh memiliki spasi di awal/akhir!"); continue; }
        u = trim(u);
        cout << "Password : ";
        getline(cin, p);
        if (p == "0") { tampilkanPesan("info", "Login dibatalkan."); return false; }
        if (hasLeadingTrailingSpace(p)) { tampilkanPesan("error", "Password tidak boleh memiliki spasi di awal/akhir!"); continue; }
        p = trim(p);
        for (int i = 0; i < jumlahPelanggan; i++) {
            if (daftarPelanggan[i].username == u && daftarPelanggan[i].password == p) {
                idxPelangganAktif = i;
                return true;
            }
        }
        ++percobaan;
        tampilkanPesan("error", "Login gagal! Sisa percobaan: " + to_string(3 - percobaan));
    }
    cout << "" << endl;
    tampilkanPesan("error", "3x gagal. Kembali ke menu utama.");
    jedaLayar();
    return false;
}

void daftarPelanggan_() {
    cout << "" << endl;
    cout << MG << B << "  DAFTAR PELANGGAN BARU\n" << R;
    cout << RD << "  (Ketik 0 untuk batal)\n" << R;
    cout << "" << endl;
    if (jumlahPelanggan >= MAKS_PELANGGAN) {
        cout << RD << "  Kapasitas pengguna penuh.\n" << R;
        return;
    }
    string nama = inputNama("  Nama lengkap  : ");
    if (nama == "0") { tampilkanPesan("info", "Pendaftaran dibatalkan."); return; }

    string user;
    while (true) {
        cout << "  Username (4-20 karakter, huruf/angka): ";
        getline(cin, user);
        if (user == "0") { tampilkanPesan("info", "Pendaftaran dibatalkan."); return; }
        if (hasLeadingTrailingSpace(user)) { tampilkanPesan("error", "Username tidak boleh memiliki spasi di awal/akhir!"); continue; }
        user = trim(user);
        if (user.empty())          { tampilkanPesan("error", "Username tidak boleh kosong!"); continue; }
        if (user.length() < 4)     { tampilkanPesan("error", "Username minimal 4 karakter!"); continue; }
        if (user.length() > 20)    { tampilkanPesan("error", "Username maksimal 20 karakter!"); continue; }
        if (!hanyaHurufAngka(user)){ tampilkanPesan("error", "Username hanya boleh huruf dan angka!"); continue; }
        if (user == dataAdmin.username) { tampilkanPesan("error", "Username sudah digunakan, coba yang lain."); continue; }
        bool ada = false;
        for (int i = 0; i < jumlahPelanggan; i++)
            if (daftarPelanggan[i].username == user) { ada = true; break; }
        if (ada) { tampilkanPesan("error", "Username sudah digunakan, coba lain."); continue; }
        break;
    }
    string pass;
    while (true) {
        cout << "  Password (6-30 karakter): ";
        getline(cin, pass);
        if (pass == "0") { tampilkanPesan("info", "Pendaftaran dibatalkan."); return; }
        if (hasLeadingTrailingSpace(pass)) { tampilkanPesan("error", "Password tidak boleh memiliki spasi di awal/akhir!"); continue; }
        pass = trim(pass);
        if (pass.empty())       { tampilkanPesan("error", "Password tidak boleh kosong!"); continue; }
        if (pass.length() < 6)  { tampilkanPesan("error", "Password minimal 6 karakter!"); continue; }
        if (pass.length() > 30) { tampilkanPesan("error", "Password maksimal 30 karakter!"); continue; }
        break;
    }
    daftarPelanggan[jumlahPelanggan] = {nextIdPelanggan++, nama, user, pass};
    ++jumlahPelanggan;
    simpanData();
    tampilkanLoading();
    tampilkanPesan("sukses", "Pendaftaran berhasil! Silakan login.");
}

void tambahLayananBaru() {
    if (jumlahLayanan >= MAKS_LAYANAN) {
        tampilkanPesan("error", "Kapasitas layanan penuh! Maksimal " + to_string(MAKS_LAYANAN) + " layanan.");
        return;
    }
    cout << MG << B << "  TAMBAH LAYANAN BARU\n" << R;
    garis();
    string nama;
    while (true) {
        cout << "  Nama layanan (huruf saja, 3-50 karakter): ";
        getline(cin, nama);
        if (nama == "0") { tampilkanPesan("info", "Penambahan layanan dibatalkan."); return; }
        if (hasLeadingTrailingSpace(nama)) { tampilkanPesan("error", "Nama layanan tidak boleh memiliki spasi di awal/akhir!"); continue; }
        nama = trim(nama);
        if (nama.empty())        { tampilkanPesan("error", "Nama tidak boleh kosong!"); continue; }
        if (!hanyaHuruf(nama))   { tampilkanPesan("error", "Nama layanan hanya boleh huruf dan spasi!"); continue; }
        if (nama.length() < 3)   { tampilkanPesan("error", "Nama minimal 3 karakter!"); continue; }
        if (nama.length() > 50)  { tampilkanPesan("error", "Nama maksimal 50 karakter!"); continue; }
        break;
    }
    cout << "  Jenis layanan:\n";
    cout << "    " << CYN << "1" << R << ". Kiloan\n";
    cout << "    " << CYN << "2" << R << ". Satuan\n";
    int pj = inputAngkaBatas("  Pilih (1/2): ", 1, 2);
    string jenis = (pj == 1) ? "Kiloan" : "Satuan";
    double harga = inputHarga("  Harga (per kg/pcs) Rp: ");
    int hariEst;
    while (true) {
        hariEst = inputAngka("  Estimasi selesai (hari, 1-30): ");
        if (hariEst >= 1 && hariEst <= 30) break;
        tampilkanPesan("error", "Estimasi harus antara 1 sampai 30 hari!");
    }
    string estimasi = to_string(hariEst) + " Hari";
    cout << "  Layanan tambahan:\n";
    cout << "    " << CYN << "1" << R << ". Tanpa Setrika\n";
    cout << "    " << CYN << "2" << R << ". Setrika + Lipat\n";
    int pt = inputAngkaBatas("  Pilih (1/2): ", 1, 2);
    string tambahan = (pt == 1) ? "Tanpa Setrika" : "Setrika + Lipat";
    bool duplikatPenuh = false;
    for (int i = 0; i < jumlahLayanan; i++) {
        if (daftarLayanan[i].namaLayanan == nama &&
            daftarLayanan[i].jenis == jenis &&
            daftarLayanan[i].harga == harga &&
            daftarLayanan[i].estimasi == estimasi &&
            daftarLayanan[i].tambahan == tambahan) {
            duplikatPenuh = true;
            break;
        }
    }
    if (duplikatPenuh) {
        tampilkanPesan("error", "Layanan dengan spesifikasi yang sama sudah ada!");
        return;
    }
    cout << "\n" << DM << "  Ringkasan:\n" << R;
    cout << "  Nama     : " << nama << "\n";
    cout << "  Jenis    : " << jenis << "\n";
    cout << "  Harga    : Rp " << static_cast<int>(harga) << "\n";
    cout << "  Estimasi : " << estimasi << "\n";
    cout << "  Tambahan : " << tambahan << "\n\n";
    if (!konfirmasi("  Simpan layanan ini?")) {
        tampilkanPesan("info", "Penambahan layanan dibatalkan.");
        return;
    }
    daftarLayanan[jumlahLayanan] = {nextIdLayanan++, nama, jenis, harga, estimasi, tambahan};
    ++jumlahLayanan;
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
    cout << "  Harga    : Rp " << static_cast<int>(l.harga) << "\n";
    cout << "  Estimasi : " << l.estimasi << "\n";
    cout << "  Tambahan : " << l.tambahan << "\n\n";
    cout << DM << IT << "  (Tekan Enter untuk melewati / tidak mengubah)\n\n" << R;
    
    string tmp;
    while (true) {
        tmp = inputTeksBolehKosong("  Nama baru     : ");
        if (tmp.empty()) break;
        if (hasLeadingTrailingSpace(tmp)) { tampilkanPesan("peringatan", "Nama tidak boleh memiliki spasi di awal/akhir, ulangi."); continue; }
        if (tmp.length() > 50) { tampilkanPesan("peringatan", "Nama terlalu panjang, ulangi."); continue; }
        if (!hanyaHuruf(tmp)) { tampilkanPesan("peringatan", "Nama hanya boleh huruf dan spasi, ulangi."); continue; }
        l.namaLayanan = tmp;
        break;
    }

    cout << "  Jenis baru:\n";
    cout << "    " << CYN << "1" << R << ". Kiloan\n";
    cout << "    " << CYN << "2" << R << ". Satuan\n";
    string inputJenis;
    while (true) {
        cout << "  Pilih (1/2): ";
        getline(cin, inputJenis);
        inputJenis = trim(inputJenis);
        if (inputJenis.empty()) { break; }
        if (inputJenis == "1") { l.jenis = "Kiloan"; break; }
        if (inputJenis == "2") { l.jenis = "Satuan"; break; }
        tampilkanPesan("peringatan", "Input tidak valid!");
    }

    while (true) {
        tmp = inputTeksBolehKosong("  Harga baru (Enter=skip)  : Rp ");
        if (tmp.empty()) break;
        try {
            double h = stod(tmp);
            if (h < 2000 || h > 1000000) { tampilkanPesan("peringatan", "Harga harus 2000-1000000, ulangi."); continue; }
            l.harga = h;
            break;
        } catch (...) { tampilkanPesan("peringatan", "Format harga tidak valid, ulangi."); }
    }

    tmp = inputTeksBolehKosong("  Estimasi baru (angka, Enter=skip): ");
    if (!tmp.empty()) {
        try {
            int est = stoi(tmp);
            if (est >= 1 && est <= 30) {
                l.estimasi = to_string(est) + " Hari";
            } else {
                tampilkanPesan("peringatan", "Estimasi harus 1-30 hari.");
            }
        } catch (...) {
            tampilkanPesan("peringatan", "Input estimasi harus angka.");
        }
    }

    cout << "  Tambahan baru:\n";
    cout << "    " << CYN << "1" << R << ". Tanpa Setrika\n";
    cout << "    " << CYN << "2" << R << ". Setrika + Lipat\n";
    string inputTambahan;
    while (true) {
        cout << "  Pilih (1/2): ";
        getline(cin, inputTambahan);
        inputTambahan = trim(inputTambahan);
        if (inputTambahan.empty()) { break; }
        if (inputTambahan == "1") { l.tambahan = "Tanpa Setrika"; break; }
        if (inputTambahan == "2") { l.tambahan = "Setrika + Lipat"; break; }
        tampilkanPesan("peringatan", "Input tidak valid!");
    }

    cout << "\n" << DM << "  Ringkasan perubahan:\n" << R;
    cout << "  Nama     : " << l.namaLayanan << "\n";
    cout << "  Jenis    : " << l.jenis << "\n";
    cout << "  Harga    : Rp " << static_cast<int>(l.harga) << "\n";
    cout << "  Estimasi : " << l.estimasi << "\n";
    cout << "  Tambahan : " << l.tambahan << "\n\n";
    if (!konfirmasi("  Simpan perubahan ini?")) {
        tampilkanPesan("info", "Edit layanan dibatalkan.");
        return;
    }
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
    --jumlahLayanan;
    if (jumlahLayanan > 0) {
        int minId = daftarLayanan[0].idLayanan;
        for (int i = 1; i < jumlahLayanan; i++)
            if (daftarLayanan[i].idLayanan < minId) minId = daftarLayanan[i].idLayanan;
        nextIdLayanan = minId;
    } else {
        nextIdLayanan = 1;
    }
    for (int i = 0; i < jumlahLayanan; i++)
        daftarLayanan[i].idLayanan = i + 1;
    if (jumlahLayanan > 0) nextIdLayanan = daftarLayanan[jumlahLayanan-1].idLayanan + 1;
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
        cout << "  " << CYN << "1" << R << ". Lihat Semua Layanan\n";
        cout << "  " << CYN << "2" << R << ". Tambah Layanan Baru\n";
        cout << "  " << CYN << "3" << R << ". Edit Layanan\n";
        cout << "  " << CYN << "4" << R << ". Hapus Layanan\n";
        cout << "  " << RD << "0" << R << ". Kembali\n";
        garis();
        int p = inputAngka("Pilih: ");
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

void lihatSemuaOrder() {
    cout << CYN << B << "  SEMUA ORDER\n" << R;
    garis('=', 115);
    if (jumlahTransaksi == 0) {
        tampilkanPesan("info", "Belum ada order masuk.");
        return;
    }
    cout << B << "  " << left
        << setw(6)  << "Antri"
        << setw(7)  << "ID"
        << setw(18) << "Pelanggan"
        << setw(30) << "Layanan"
        << setw(8)  << "Berat"
        << setw(12) << "Harga"
        << setw(12) << "Status"
        << setw(12) << "Bayar"
        << "TglOrder" << R << "\n";
    garis('-', 115);
    for (int i = 0; i < jumlahTransaksi; i++) {
        Transaksi& t = daftarTransaksi[i];
        string beratStr  = (t.berat == 0) ? DM + string("-") + R : (fixed, stringstream() << setprecision(2) << t.berat).str() + " kg";
        string hargaStr  = (t.hargaTotal == 0) ? DM + string("-") + R : "Rp " + to_string(t.hargaTotal);
        string namaPel   = getNamaAktual(t);
        string namaLay   = getNamaLayanan(t.idLayanan);
        
        cout << "  " << YL << setw(4) << ("#" + to_string(t.noAntrian)) << R << "  "
            << left << setw(7)  << t.idTransaksi
            << setw(18) << namaPel.substr(0,16)
            << setw(30) << namaLay.substr(0, 28)
            << setw(8)  << beratStr
            << setw(12) << hargaStr
            << setw(12) << t.status
            << setw(12) << t.statusBayar
            << t.tglOrder << "\n";
    }
    garis('=', 115);
    cout << DM << "  Total: " << jumlahTransaksi << " order\n" << R;
}

void tambahTransaksiManual() {
    if (jumlahTransaksi >= MAKS_TRANSAKSI) {
        tampilkanPesan("error", "Kapasitas transaksi penuh! Maksimal " + to_string(MAKS_TRANSAKSI) + " transaksi.");
        return;
    }
    cout << "  Tipe pelanggan:\n";
    cout << "    " << CYN << "1" << R << ". Pelanggan terdaftar\n";
    cout << "    " << CYN << "2" << R << ". Walk-in (tanpa akun)\n";
    cout << RD << "  (Ketik 0 untuk batal)\n" << R;
    cout << "" << endl;
    int tipe = inputAngkaBatas("  Pilih (1/2): ", 0, 2);
    bersihkanLayar(); tampilkanHeader();
    int idPel = -1, idxPel = -1;
    string namaWalkin = "";
    if (tipe == 0) {
        tampilkanPesan("info", "Penambahan transaksi dibatalkan.");
        return;
    }
    else if (tipe == 1) {
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
        while (idxPel == -1) {
            idPel  = inputAngka("  ID Pelanggan: ");
            idxPel = cariIdxPelanggan(idPel);
            if (idxPel == -1)
                tampilkanPesan("error", "Pelanggan dengan ID " + to_string(idPel) + " tidak ditemukan!");
        }
    } else {
        namaWalkin = inputNama("  Nama pelanggan walk-in: ");
    }
    lihatSemuaLayanan();
    int idLay, idxLay = -1;
    while (idxLay == -1) {
        idLay  = inputAngka("  ID Layanan: ");
        idxLay = cariIdxLayanan(idLay);
        if (idxLay == -1)
            tampilkanPesan("error", "Layanan dengan ID " + to_string(idLay) + " tidak ditemukan!");
    }
    double berat = inputBerat("  Berat (kg/pcs): ");
    long long harga = static_cast<long long>(berat * daftarLayanan[idxLay].harga);
    cout << "\n" << DM << "  Ringkasan transaksi:\n" << R;
    if (tipe == 1) cout << "  Pelanggan : " << daftarPelanggan[idxPel].nama << "\n";
    else cout << "  Pelanggan : " << namaWalkin << " (Walk-in)\n";
    cout << "  Layanan   : " << daftarLayanan[idxLay].namaLayanan << "\n";
    cout << "  Berat     : " << fixed << setprecision(2) << berat << " kg\n";
    cout << "  Total     : " << GR << B << "Rp " << harga << R << "\n\n";
    if (!konfirmasi("  Konfirmasi tambah transaksi?")) {
        tampilkanPesan("info", "Transaksi dibatalkan.");
        return;
    }
    Transaksi trx;
    trx.idTransaksi  = nextIdTransaksi++;
    trx.noAntrian    = nextNoAntrian++;
    trx.idPelanggan  = (tipe == 1) ? idPel : -1;
    trx.idLayanan    = idLay;
    trx.berat        = berat;
    trx.hargaTotal   = harga;
    trx.status       = "Diproses";
    trx.statusBayar  = "Belum Bayar";
    trx.tglOrder     = tanggalHariIni();
    trx.tglSelesai   = "";
    trx.namaWalkin   = (tipe == 2) ? namaWalkin : "";
    trx.namaPelanggan= (tipe == 1) ? daftarPelanggan[idxPel].nama : namaWalkin;
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
    cout << "\n" << RD << "  (Ketik 0 untuk batal)\n" << R;
    int id, idx = -1;
    while (idx == -1) {
        id  = inputAngka("  Masukkan ID Transaksi: ");
        if (id == 0) return;
        idx = cariIdxTransaksi(id);
        if (idx == -1) tampilkanPesan("error", "ID transaksi " + to_string(id) + " tidak ditemukan!");
    }
    Transaksi& t = daftarTransaksi[idx];
    cout << "\n  Detail Order:\n";
    cout << "  ID         : " << t.idTransaksi << "\n";
    cout << "  Tgl Order  : " << t.tglOrder << "\n";
    if (!t.tglSelesai.empty()) cout << "  Tgl Selesai: " << t.tglSelesai << "\n";
    cout << "  Status     : " << t.status << "\n\n";
    
    if (t.status == "Menunggu") {
        int idxL = cariIdxLayanan(t.idLayanan);
        cout << "  Layanan : " << (idxL >= 0 ? daftarLayanan[idxL].namaLayanan : "-") << "\n";
        double berat = inputBerat("  Input berat (kg/pcs): ");
        t.berat      = berat;
        t.hargaTotal = (idxL >= 0) ? static_cast<long long>(berat * daftarLayanan[idxL].harga) : 0;
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
    cout << "\n" << RD << "  (Ketik 0 untuk batal)\n" << R;
    int id, idx = -1;
    while (true) {
        id  = inputAngka("  Masukkan ID Transaksi: ");
        if (id == 0) return;
        idx = cariIdxTransaksi(id);
        if (idx != -1) break;
        tampilkanPesan("error", "ID transaksi " + to_string(id) + " tidak ditemukan!");
    }
    
    string statusSaatIni = daftarTransaksi[idx].status;
    if (statusSaatIni == "Selesai" || statusSaatIni == "Lunas" || statusSaatIni == "Dibatalkan") {
        tampilkanPesan("error", "Order tidak bisa dibatalkan karena status sudah '" + statusSaatIni + "'!");
        return;
    }
    
    cout << "\n  Order yang akan dibatalkan:\n";
    cout << "  ID     : " << daftarTransaksi[idx].idTransaksi << "\n";
    cout << "  Status : " << statusSaatIni << "\n\n";
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
        cout << CYN << B << "  ╔══════════════════════════╗\n";
        cout << "  ║    KELOLA TRANSAKSI      ║\n";
        cout << "  ╚══════════════════════════╝\n" << R;
        garis();
        cout << "  " << CYN << "1" << R << ". Lihat Semua Order\n";
        cout << "  " << CYN << "2" << R << ". Tambah Transaksi Manual (Walk-in)\n";
        cout << "  " << CYN << "3" << R << ". Update Status Order\n";
        cout << "  " << CYN << "4" << R << ". Cancel Order\n";
        cout << "  " << RD << "0" << R << ". Kembali\n";
        garis();
        int p = inputAngka("Pilih: ");
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

void merge_(Transaksi* arr, int kiri, int tengah, int kanan) {
    int n1 = tengah - kiri + 1;
    int n2 = kanan - tengah;
    Transaksi* L  = new Transaksi[n1];
    Transaksi* R_ = new Transaksi[n2];
    for (int i = 0; i < n1; i++) L[i]  = arr[kiri + i];
    for (int j = 0; j < n2; j++) R_[j] = arr[tengah + 1 + j];
    int i = 0, j = 0, k = kiri;
    while (i < n1 && j < n2) {
        string nL = getNamaAktual(L[i]);
        string nR = getNamaAktual(R_[j]);
        for (char& c : nL) c = tolower(static_cast<unsigned char>(c));
        for (char& c : nR) c = tolower(static_cast<unsigned char>(c));
        if (nL <= nR) arr[k++] = L[i++];
        else          arr[k++] = R_[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R_[j++];
    delete[] L;
    delete[] R_;
}

void mergeSortNama(Transaksi* arr, int kiri, int kanan) {
    if (kiri < kanan) {
        int tengah = kiri + (kanan - kiri) / 2;
        mergeSortNama(arr, kiri, tengah);
        mergeSortNama(arr, tengah + 1, kanan);
        merge_(arr, kiri, tengah, kanan);
    }
}

int partisiQuick(Transaksi* arr, int lo, int hi) {
    long long pivot = arr[hi].hargaTotal;
    int i     = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j].hargaTotal >= pivot) {
            ++i;
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
        << setw(7) << " Antri"
        << setw(7) << "ID"
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
            << setw(18) << getNamaAktual(arr[i])
            << setw(14) << hargaStr
            << arr[i].status << "\n";
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
        p = inputAngka("Pilih (1-2): ");
        if (p == 1 || p == 2) break;
        cout << RD << "  Pilihan tidak valid!\n" << R;
    }

    Transaksi temp[MAKS_TRANSAKSI];
    for (int i = 0; i < jumlahTransaksi; i++) temp[i] = daftarTransaksi[i];
    cout << "\n" << YL << "  DATA ASLI (URUTAN MASUK):\n" << R;
    tampilkanRingkasTransaksi(daftarTransaksi, jumlahTransaksi);

    if (p == 1) {
        mergeSortNama(temp, 0, jumlahTransaksi - 1);
        cout << GR << "\n  Merge Sort Nama Pelanggan (A-Z) selesai.\n" << R;
    } else {
        quickSortHarga(temp, 0, jumlahTransaksi - 1);
        cout << GR << "\n  Quick Sort Harga Total (Tertinggi ke Terendah) selesai.\n" << R;
    }

    cout << "\n" << GR << "  HASIL SORTING (HANYA TAMPILAN):\n" << R;
    tampilkanRingkasTransaksi(temp, jumlahTransaksi);
    cout << CYN << "\n  [i] Sorting hanya untuk keperluan tampilan. Data asli tidak diubah.\n" << R;
}

int binarySearchId(int targetId) {
    try {
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
            if (temp[mid].idTransaksi == targetId) return cariIdxTransaksi(targetId);
            else if (temp[mid].idTransaksi < targetId) lo = mid + 1;
            else hi = mid - 1;
        }
    } catch (...) { return -1; }
    return -1;
}

void linearSearchNama(const string& nama) {
    string namaLow = nama;
    for (char& c : namaLow) c = tolower(static_cast<unsigned char>(c));
    bool ada = false;
    cout << "\n" << CYN << "  Hasil pencarian nama mengandung '" << nama << "':\n" << R;
    garis('=', 100);
    cout << B << left
        << setw(6)  << "Antri"
        << setw(7)  << "ID"
        << setw(18) << "Nama Pelanggan"
        << setw(25) << "Layanan"
        << setw(8)  << "Berat"
        << setw(12) << "Harga"
        << setw(10) << "Status"
        << setw(10) << "Bayar"
        << "TglOrder" << R << "\n";
    garis('-', 100);
    for (int i = 0; i < jumlahTransaksi; i++) {
        const Transaksi& t = daftarTransaksi[i];
        string np    = getNamaAktual(t);
        string npLow = np;
        for (char& c : npLow) c = tolower(static_cast<unsigned char>(c));
        
        if (npLow.find(namaLow) != string::npos) {
            string beratStr  = (t.berat == 0) ? "-" : (fixed, stringstream() << setprecision(2) << t.berat).str() + " kg";
            string hargaStr  = (t.hargaTotal == 0) ? "-" : "Rp " + to_string(t.hargaTotal);
            cout << left
                << setw(6) << ("#"+to_string(t.noAntrian))
                << setw(7) << t.idTransaksi
                << setw(18) << np.substr(0,16)
                << setw(25) << getNamaLayanan(t.idLayanan).substr(0,23)
                << setw(8) << beratStr
                << setw(12) << hargaStr
                << setw(10) << t.status
                << setw(10) << t.statusBayar
                << t.tglOrder << "\n";
            ada = true;
        }
    }
    garis('=', 100);
    if (!ada) tampilkanPesan("info", "Tidak ada transaksi yang ditemukan dengan nama tersebut.");
}

void cariTransaksi() {
    cout << BL << "  CARI TRANSAKSI\n" << R;
    garis();
    cout << "  1. Cari berdasarkan ID Transaksi   (Binary Search)\n";
    cout << "  2. Cari berdasarkan Nama Pelanggan (Linear Search)\n";
    garis();
    int p;
    while (true) {
        p = inputAngka("Pilih (1/2): ");
        if (p == 1 || p == 2) break;
        cout << RD << "  Pilihan tidak valid!\n" << R;
    }
    if (p == 1) {
        int id  = inputAngka("  Masukkan ID Transaksi: ");
        int idx = binarySearchId(id);
        if (idx == -1) tampilkanPesan("info", "Transaksi dengan ID " + to_string(id) + " tidak ditemukan!");
        else {
            cout << GR << "  Transaksi ditemukan!\n" << R;
            tampilkanStruk(idx);
        }
    } else {
        string nama = inputTeks("  Masukkan nama pelanggan (cukup ketik sebagian): ");
        linearSearchNama(nama);
    }
}

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
            << GR << setw(12) << ("Rp " + to_string(static_cast<int>(l.harga)) + satuanHarga) << R
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
    int idLay, idxLay = -1;
    while (idxLay == -1) {
        cout << RD << "  (Ketik 0 untuk batal)\n" << R;
        cout << "" << endl;
        idLay  = inputAngka("  Pilih ID Layanan: ");
        idxLay = cariIdxLayanan(idLay);
        if (idLay == 0) {
            tampilkanPesan("info", "Pembuatan order dibatalkan.");
            return;
        }
        if (idxLay == -1) cout << RD << "  Layanan tidak ditemukan, ulangi.\n" << R;
    }
    Layanan& l = daftarLayanan[idxLay];
    cout << "\n  Detail layanan yang dipilih:\n";
    garis('-', 40);
    cout << "  Nama     : " << l.namaLayanan << "\n";
    cout << "  Jenis    : " << l.jenis << "\n";
    cout << "  Harga    : " << GR << "Rp " << static_cast<int>(l.harga) << R << " / " << (l.jenis == "Kiloan" ? "kg" : "pcs") << "\n";
    cout << "  Estimasi : " << l.estimasi << "\n";
    cout << "  Tambahan : " << l.tambahan << "\n";
    garis('-', 40);
    cout << CYN << "  Lanjutkan order? (y/n): " << R;
    string k; getline(cin, k);
    k = trim(k);
    if (k != "y" && k != "Y") {
        tampilkanPesan("info", "Order dibatalkan.");
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
    trx.namaWalkin  = "";
    trx.namaPelanggan = daftarPelanggan[idxPelangganAktif].nama;
    daftarTransaksi[jumlahTransaksi++] = trx; 
    simpanData();
    tampilkanPesan("sukses", "Order berhasil dibuat! Nomor antrian: #" + to_string(trx.noAntrian));
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
        << setw(30) << "Layanan"
        << setw(12) << "TglOrder"
        << setw(12) << "Status"
        << "Bayar" << R << "\n";
    garis();
    for (int i = 0; i < jumlahTransaksi; i++) {
        if (daftarTransaksi[i].idPelanggan == idPel) {
            cout << "  " << YL << setw(5) << daftarTransaksi[i].idTransaksi << R << "  "
                << left
                << setw(30) << getNamaLayanan(daftarTransaksi[i].idLayanan)
                << setw(12) << daftarTransaksi[i].tglOrder
                << setw(12) << daftarTransaksi[i].status
                << daftarTransaksi[i].statusBayar << "\n";
            ada = true;
        }
    }
    if (!ada) {
        cout << DM << "  Belum ada order.\n" << R;
        return;
    }
    garis();
    cout << RD << "  (Ketik 0 untuk batal)\n" << R;
    cout << "" << endl;
    int id = inputAngka("  Masukkan ID untuk lihat struk: ");
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
        << setw(32) << "Layanan"
        << "Status" << R << "\n";
    garis();
    for (int i = 0; i < jumlahTransaksi; i++) {
        if (daftarTransaksi[i].idPelanggan == idPel) {
            cout << "  " << YL << setw(5) << daftarTransaksi[i].idTransaksi << R << "  "
                << left
                << setw(32) << getNamaLayanan(daftarTransaksi[i].idLayanan)
                << daftarTransaksi[i].status << "\n";
            ada = true;
        }
    }
    if (!ada) {
        cout << DM << "  Tidak ada order yang bisa dibatalkan.\n" << R;
        return;
    }
    garis();
    cout << RD << "  (Ketik 0 untuk batal)\n" << R;
    int id, idx = -1;
    while (true) {
        id = inputAngka("  Masukkan ID order yang dibatalkan: ");
        if (id == 0) return;
        idx = cariIdxTransaksi(id);
        if (idx != -1 && daftarTransaksi[idx].idPelanggan == idPel) break;
        tampilkanPesan("error", "Transaksi tidak ditemukan!");
    }
    if (daftarTransaksi[idx].status != "Menunggu") {
        tampilkanPesan("error", "Order sudah diproses, tidak bisa dibatalkan.");
        return;
    }
    cout << CYN << "  Yakin batalkan order ini? (y/n): " << R;
    string k; getline(cin, k);
    k = trim(k);
    if (k != "y" && k != "Y") {
        cout << DM << "  Pembatalan dibatalkan.\n" << R;
        return;
    }
    daftarTransaksi[idx].status = "Dibatalkan";
    simpanData();
    tampilkanPesan("sukses", "Order berhasil dibatalkan.");
}

void kelolaPelanggan() {
    bool aktif = true;

    while (aktif) {
        bersihkanLayar();
        tampilkanHeader();

        cout << MG << B << "  ╔══════════════════════════╗\n";
        cout << "  ║    KELOLA PELANGGAN      ║\n";
        cout << "  ╚══════════════════════════╝\n" << R;
        garis();
        cout << "  " << CYN << "1" << R << ". Lihat Semua Pelanggan\n";
        cout << "  " << CYN << "2" << R << ". Hapus Akun Pelanggan\n";
        cout << "  " << RD  << "0" << R << ". Kembali\n";
        garis();

        int p = inputAngkaBatas("Pilih: ", 0, 2);

        if (p == 0) {
            aktif = false;
            break;
        }
        else if (p == 1) {

            bersihkanLayar();
            tampilkanHeader();
            cout << MG << "  DAFTAR PELANGGAN\n" << R;
            garis();

            if (jumlahPelanggan == 0) {
                cout << DM << "  Belum ada pelanggan.\n" << R;
                jedaLayar();
                continue;
            }
            cout << B << left
                << setw(5)  << "  ID"
                << setw(22) << "Nama"
                << setw(16) << "Username"
                << R << "\n";
            garis();

            for (int i = 0; i < jumlahPelanggan; i++) {

                cout << "  "
                    << YL << setw(3) << daftarPelanggan[i].idPelanggan << R << "  "
                    << left << setw(22) << daftarPelanggan[i].nama
                    << daftarPelanggan[i].username
                    << "\n";
            }
            garis();
            jedaLayar();
        }
        else if (p == 2) {

            bersihkanLayar();
            tampilkanHeader();
            cout << RD << B << "  HAPUS AKUN PELANGGAN\n" << R;
            garis();

            if (jumlahPelanggan == 0) {
                cout << DM << "  Belum ada pelanggan.\n" << R;
                jedaLayar();
                continue;
            }

            for (int i = 0; i < jumlahPelanggan; i++) {

                cout << "  "
                    << YL << daftarPelanggan[i].idPelanggan << R
                    << ". "
                    << daftarPelanggan[i].nama
                    << " (" << daftarPelanggan[i].username << ")\n";
            }
            garis();
            cout << RD << "  (Ketik 0 untuk batal)\n" << R;
            cout << endl;

            int id = inputAngka("  Masukkan ID pelanggan yang dihapus: ");
            if (id == 0) {
                tampilkanPesan("info", "Penghapusan dibatalkan.");
                jedaLayar();
                continue;
            }

            int idx = cariIdxPelanggan(id);
            if (idx == -1) {
                tampilkanPesan("error", "ID pelanggan tidak ditemukan!");
                jedaLayar();
                continue;
            }

            cout << "\n  Hapus akun: "
                << RD << B
                << daftarPelanggan[idx].nama
                << R << "\n";
            if (!konfirmasi("  Yakin ingin menghapus akun ini?")) {
                tampilkanPesan("info", "Penghapusan dibatalkan.");
                jedaLayar();
                continue;
            }

            int idPelHapus = daftarPelanggan[idx].idPelanggan;
            string namaPelHapus = daftarPelanggan[idx].nama;
            for (int i = 0; i < jumlahTransaksi; i++) {
                if (daftarTransaksi[i].idPelanggan == idPelHapus) {
                    daftarTransaksi[i].namaPelanggan = namaPelHapus;
                }
            }

            for (int i = idx; i < jumlahPelanggan - 1; i++) {
                daftarPelanggan[i] = daftarPelanggan[i + 1];
            }
            jumlahPelanggan--;

            simpanData();
            tampilkanPesan("sukses", "Akun pelanggan berhasil dihapus.");
            jedaLayar();
        }
    }
}

void menuAdmin() {
    bool aktif = true;
    while (aktif) {
        bersihkanLayar(); tampilkanHeader();
        cout << BL << "  MENU ADMIN  |  " << dataAdmin.username << "\n" << R;
        garis();
        cout << "  " << CYN << "1" << R << ". Kelola Layanan\n";
        cout << "  " << CYN << "2" << R << ". Kelola Transaksi\n";
        cout << "  " << CYN << "3" << R << ". Sorting Data\n";
        cout << "  " << CYN << "4" << R << ". Cari Transaksi\n";
        cout << "  " << CYN << "5" << R << ". Kelola Pelanggan\n";
        cout << "  " << RD << "6" << R << ". Logout\n";
        garis();
        int p;
        while (true) {
            try {
                p = inputAngka("Pilih: ");
                if (p >= 1 && p <= 6) break;
                cout << RD << "  Pilihan tidak valid! Masukkan angka 1-6.\n" << R;
            } catch (...) {
                tampilkanPesan("error", "Error input menu!");
            }
        }
        switch (p) {
            case 1: bersihkanLayar(); tampilkanHeader(); menuKelolalayanan(); break;
            case 2: bersihkanLayar(); tampilkanHeader(); menuKelolaTransaksi(); break;
            case 3: bersihkanLayar(); tampilkanHeader(); sortingData(); jedaLayar(); break;
            case 4: bersihkanLayar(); tampilkanHeader(); cariTransaksi(); jedaLayar(); break;
            case 5: bersihkanLayar(); tampilkanHeader(); kelolaPelanggan(); break;
            case 6:
                aktif = false;
                loginSebagaiAdmin = false;
                cout << YL << "\n  Logout berhasil. Sampai jumpa!\n" << R;
                jedaLayar();
                break;
        }
    }
}

void menuPelanggan() {
    bool aktif = true;
    while (aktif) {
        bersihkanLayar(); tampilkanHeader();
        cout << BL << "  MENU PELANGGAN  |  " << daftarPelanggan[idxPelangganAktif].nama << "\n" << R;
        garis();
        cout << "  " << CYN << "1" << R << ". Lihat Layanan Tersedia\n";
        cout << "  " << CYN << "2" << R << ". Buat Order Baru\n";
        cout << "  " << CYN << "3" << R << ". Lihat Order & Struk Saya\n";
        cout << "  " << CYN << "4" << R << ". Batalkan Order\n";
        cout << "  " << RD << "5" << R << ". Logout\n";
        garis();
        int p;
        while (true) {
            try {
                p = inputAngka("Pilih: ");
                if (p >= 1 && p <= 5) break;
                cout << RD << "  Pilihan tidak valid.\n" << R;
            } catch (...) {
                tampilkanPesan("error", "Error input menu!");
            }
        }
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
        }
    }
}

int main() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    #endif
    system("chcp 65001 > nul");
    muatData();
    simpanData();
    
    int pilihan = -1;
    do {
        bersihkanLayar(); tampilkanHeader();
        cout << MG << B << "  ╔══════════════════════════╗\n";
        cout << "  ║     MENU UTAMA           ║\n";
        cout << "  ╚══════════════════════════╝\n" << R;
        garis();
        cout << "  " << CYN << "1" << R << ". Login Admin\n";
        cout << "  " << CYN << "2" << R << ". Login Pelanggan\n";
        cout << "  " << CYN << "3" << R << ". Daftar Pelanggan\n";
        cout << "  " << RD << "0" << R << ". Keluar\n";
        garis();
        
        while (true) {
            try {
                pilihan = inputAngka("Pilih: ");
                if (pilihan >= 0 && pilihan <= 3) break;
                cout << RD << "  Pilihan tidak valid! Masukkan angka 0-3.\n" << R;
            } catch (...) {
                tampilkanPesan("error", "Error input menu!");
            }
        }
        
        switch (pilihan) {
            case 1:
                bersihkanLayar(); tampilkanHeader();
                if (loginAdmin()) {
                    tampilkanLoading();
                    cout << "\nLogin berhasil! Selamat datang, " << dataAdmin.username << ".\n";
                    jedaLayar();
                    menuAdmin();
                };
                break;
            case 2:
                bersihkanLayar(); tampilkanHeader();
                if (loginPelanggan()) {
                    tampilkanLoading();
                    cout << "\nLogin berhasil! Selamat datang, " << daftarPelanggan[idxPelangganAktif].nama << ".\n";
                    jedaLayar();
                    menuPelanggan();
                };
                break;
            case 3:
                bersihkanLayar(); tampilkanHeader();
                if (jumlahPelanggan >= MAKS_PELANGGAN) {
                    cout << RD << "  Pendaftaran penuh, hubungi admin.\n" << R;
                    jedaLayar();
                } else {
                    daftarPelanggan_();
                    jedaLayar();
                }
                break;
            case 0:
                cout << GR << "\n  Terima kasih!\n" << R;
                break;
        }
    } while (pilihan != 0);
    return 0;
}