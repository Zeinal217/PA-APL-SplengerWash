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