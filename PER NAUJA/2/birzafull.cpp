#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <cmath>

using namespace std;

const double MOKESTIS_PROC = 0.01;
const double BANKROTO_RIBA = 0.20;
const int ZAIDIMO_TRUKME = 10;
const int SMA_PERIODAS = 3;
const double NORM_STRAT_MIN_POKYTIS = -0.3;
const double NORM_STRAT_MAX_POKYTIS = 0.3;
const double BULL_STRAT_MIN_POKYTIS = -0.1;
const double BULL_STRAT_MAX_POKYTIS = 0.5;
const double BEAR_STRAT_MIN_POKYTIS = -0.5;
const double BEAR_STRAT_MAX_POKYTIS = 0.1;

class Aktyvas {
protected:
    string pavadinimas;
    double kaina;
    vector<double> istorija;
//
public:
    Aktyvas(string p, double k) : pavadinimas(p), kaina(k) {
        istorija.push_back(k);
    }
    virtual ~Aktyvas() {}

    string gautiPavadinima() const { return pavadinimas; }
    double gautiKaina() const { return kaina; }
    const vector<double>& gautiIstorija() const { return istorija; }

    void atnaujintiKaina(double naujaKaina) {
        if (naujaKaina < 0.10) naujaKaina = 0.10;
        kaina = naujaKaina;
        istorija.push_back(naujaKaina);
    }

    virtual double gautiRizika() const { return 1.0; }

    virtual string gautiTipa() const = 0;
};

class Akcija : public Aktyvas {
public:
    Akcija(string p, double k) : Aktyvas(p, k) {}
    string gautiTipa() const override { return "Akcija"; }
};

class Kripto : public Aktyvas {
public:
    Kripto(string p, double k) : Aktyvas(p, k) {}
    string gautiTipa() const override { return "Kriptovaliuta"; }

    double gautiRizika() const override { return 2.0; }
};

class AktyvuFactory {
public:
    static Aktyvas* sukurti(int tipas, string pavadinimas, double kaina) {
        if (tipas == 2) return new Kripto(pavadinimas, kaina);
        return new Akcija(pavadinimas, kaina);
    }
};

class Strategija {
public:
    virtual double skaiciuotiPokyti() = 0;
    virtual ~Strategija() {}
protected:
    double randomRange(double min, double max) {
        return min + (double)rand() / RAND_MAX * (max - min);
    }
};

class NormaliStrategija : public Strategija {
    double skaiciuotiPokyti() override { return randomRange(NORM_STRAT_MIN_POKYTIS, NORM_STRAT_MAX_POKYTIS); }
};

class BullStrategy : public Strategija {
    double skaiciuotiPokyti() override { return randomRange(BULL_STRAT_MIN_POKYTIS, BULL_STRAT_MAX_POKYTIS); }
};

class BearStrategy : public Strategija {
    double skaiciuotiPokyti() override { return randomRange(BEAR_STRAT_MIN_POKYTIS, BEAR_STRAT_MAX_POKYTIS); }
};

class Rinka {
private:
    Strategija* strategija;
public:
    Rinka(Strategija* s) : strategija(s) {}
    ~Rinka() { delete strategija; }

    bool vykdytiDiena(Aktyvas* aktyvas) {
        double pokytis = strategija->skaiciuotiPokyti();
        pokytis*= aktyvas->gautiRizika();
        bool news = false;
        if (((double)rand() / RAND_MAX) < 0.20) {
            pokytis *= 2.0;
            news = true;
        }
        double nauja = aktyvas->gautiKaina() * (1.0 + pokytis);
        aktyvas->atnaujintiKaina(nauja);
        return news;
    }

    double skaiciuotiSMA(const Aktyvas* aktyvas, int periodas) {
        const auto& ist = aktyvas->gautiIstorija();
        if (ist.size() < periodas) return 0.0;
        double sum = 0;
        for (int i = 0; i < periodas; i++) sum += ist[ist.size() - 1 - i];
        return sum / periodas;
    }
};

class Zaidejas {
private:
    double balansas;
    int akcijos;

public:
    Zaidejas(double start) : balansas(start), akcijos(0) {}

    double gautiBalansa() const { return balansas; }
    int gautiAkcijas() const { return akcijos; }
    int gautiMaxKieki(double kaina) const{
        double vntKaina = kaina * (1.0 + MOKESTIS_PROC);
        return (int)(balansas / vntKaina);
    } 
    
    bool pirkti(double kaina, int kiekis) {
        if (kiekis <= 0) return false;
        double suma = kaina * kiekis;
        double mokestis = suma * MOKESTIS_PROC;
        double viso = suma + mokestis;

        if (balansas >= viso) {
            balansas -= viso;
            akcijos += kiekis;
            return true;
        }
        return false;
    }

    bool parduoti(double kaina, int kiekis) {
        if (kiekis <= 0 || akcijos < kiekis) return false;
        double suma = kaina * kiekis;
        double mokestis = suma * MOKESTIS_PROC;
        double viso = suma - mokestis;

        balansas += viso;
        akcijos -= kiekis;
        return true;
    }

    double turtoVerte(double kaina) const {
        return balansas + (akcijos * kaina);
    }
};

void assertTest(bool condition, string name) {
    if (condition) {
        cout << "[OK] " << name << endl;
    } else {
        cout << "[FAIL] " << name << " <--- KLAIDA!" << endl;
        exit(1);
    }
}

void testPirkimas() {
    Zaidejas z(1000.0);
    bool pavyko = z.pirkti(100.0, 1);
    
    assertTest(pavyko == true, "1. Pirkimas sekmingas");
    assertTest(abs(z.gautiBalansa() - 899.0) < 0.01, "1.1 Balansas po pirkimo");
    assertTest(z.gautiAkcijas() == 1, "1.2 Akciju kiekis padidejo");
}

void testPardavimas() {
    Zaidejas z(1000.0);
    z.pirkti(100.0, 1);
    
    bool pavyko = z.parduoti(100.0, 1);
    
    assertTest(pavyko == true, "2. Pardavimas sekmingas");
    assertTest(abs(z.gautiBalansa() - 998.0) < 0.01, "2.1 Balansas po pardavimo");
    assertTest(z.gautiAkcijas() == 0, "2.2 Akciju kiekis sumazejo");
}

void testNepakankaPinigu() {
    Zaidejas z(50.0);
    bool pavyko = z.pirkti(100.0, 1);
    
    assertTest(pavyko == false, "3. Apsauga: Nepakanka pinigu");
    assertTest(abs(z.gautiBalansa() - 50.0) < 0.01, "3.1 Pinigai nepakito");
}

void testNeigiamasKiekis() {
    Zaidejas z(1000.0);
    bool pavyko = z.pirkti(100.0, -5);
    assertTest(pavyko == false, "4. Apsauga: Neigiamas kiekis");
}

void testSMA() {
    Aktyvas* a = AktyvuFactory::sukurti(1, "Test", 10.0);
    a->atnaujintiKaina(20.0);
    a->atnaujintiKaina(30.0);
    
    Rinka r(new NormaliStrategija());
    double sma = r.skaiciuotiSMA(a, 3);
    
    assertTest(abs(sma - 20.0) < 0.01, "5. SMA matematika teisinga");
    delete a;
}

void testFactory() {
    Aktyvas* k = AktyvuFactory::sukurti(2, "BTC", 500.0);
    assertTest(k->gautiTipa() == "Kriptovaliuta", "6. Factory sukuria teisinga tipa");
    delete k;
}

void testRizika() {
    Aktyvas* a = AktyvuFactory::sukurti(1, "Stock", 100);
    Aktyvas* b = AktyvuFactory::sukurti(2, "Crypto", 100);

    assertTest(abs(a->gautiRizika() - 1.0 ) < 0.01, "Akcijos rizika 1.0");
    assertTest(abs(b->gautiRizika() - 2.0 ) < 0.01, "Kripto rizika 2.0");

    delete a;
    delete b;
}

void leistiVisusTestus() {
    cout << "\n=== VYKDOMI UNIT TESTAI ===\n";
    testPirkimas();
    testPardavimas();
    testNepakankaPinigu();
    testNeigiamasKiekis();
    testSMA();
    testFactory();
    testRizika();
    cout << "Visi testai ivykdyti sekmingai!\n\n";
}

class Zaidimas {
private:
    Aktyvas* aktyvas;
    Zaidejas* zaidejas;
    Rinka* rinka;
    double pradinisKapitalas;

    double ivestis() {
        double x;
        while (!(cin >> x) || x <= 0) {
            cout << "Bloga ivestis. Iveskite teigiama skaiciu: ";
            cin.clear(); cin.ignore(1000, '\n');
        }
        return x;
    }

public:
    Zaidimas() : aktyvas(nullptr), zaidejas(nullptr), rinka(nullptr) {}
    ~Zaidimas() { delete aktyvas; delete zaidejas; delete rinka; }

    void start() {
        cout << "- - - BIRZOS SIMULIATORIUS - - -" << endl;
        
        cout << "Pradinis balansas: "; pradinisKapitalas = ivestis();
        zaidejas = new Zaidejas(pradinisKapitalas);

        cout << "Pradine kaina: "; double kaina = ivestis();
        cout << "Tipas (1-Akcija, 2-Kripto): "; int t; cin >> t;
        aktyvas = AktyvuFactory::sukurti(t, "DaliusCoin", kaina);

        cout << "Rinka (1-Normal, 2-Bull, 3-Bear): "; int r; cin >> r;
        Strategija* s = nullptr;
        if (r == 2) s = new BullStrategy();
        else if (r == 3) s = new BearStrategy();
        else s = new NormaliStrategija();
        rinka = new Rinka(s);

        for(int diena=1; diena<=ZAIDIMO_TRUKME; diena++) {
            if (zaidejas->turtoVerte(aktyvas->gautiKaina()) < pradinisKapitalas * BANKROTO_RIBA) {
                cout << "BANKROTAS!" << endl;
                break;
            }

            cout << "\n--- DIENA " << diena << " ---" << endl;
            double k = aktyvas->gautiKaina();
            double sma = rinka->skaiciuotiSMA(aktyvas, SMA_PERIODAS);
            
            cout << "Aktyvas: " << aktyvas->gautiPavadinima() << " (" << aktyvas->gautiTipa() << ")\n";
            cout << "Kaina: " << fixed << setprecision(2) << k << " EUR";

            const vector<double>& ist = aktyvas->gautiIstorija();

            if (ist.size() > 1){
                double senaKaina = ist[ist.size() - 2];
                double pokytis = ((k - senaKaina) / senaKaina) * 100.0;

                cout  << " (";
                if (pokytis >= 0) cout << "+";
                cout << fixed << setprecision(2) << pokytis << "%)";
            }

            if (sma > 0) cout << " (SMA: " << sma << ")";
            cout << endl;
            cout << "Balansas: " << zaidejas->gautiBalansa() << " | Turima: " << zaidejas->gautiAkcijas() << endl;

            while(true) {
                cout << "[1] Pirkti [2] Parduoti [3] Laukti: ";
                int v; cin >> v;
                if(cin.fail()) { cin.clear(); cin.ignore(1000,'\n'); continue; }
                if (v == 3) break;

                cout << "Kiekis: "; int kiek; cin >> kiek;
                if(cin.fail()) { cin.clear(); cin.ignore(1000,'\n'); continue; }

                bool ok = false;
                if (v == 1) ok = zaidejas->pirkti(k, kiek);
                else if (v == 2) ok = zaidejas->parduoti(k, kiek);

                if (ok) { cout << "Sekminga operacija!" << endl; break; }
                else{
                    if (v == 1) {
                        int maxKiekis = zaidejas->gautiMaxKieki(k);
                        double kainaSuMok = k * maxKiekis * (1.0 + MOKESTIS_PROC);
                        cout << "Klaida: nepakanka pinigu/akciju." << endl;
                        cout << "Maksimaliai galite pirkti: " << maxKiekis << " vnt. (Kaina: "
                             << fixed << setprecision(2) << kainaSuMok << " EUR)" << endl;
                    }
                    else{
                        cout << "Klaida: Nepakanka akciju. Turite tik " << zaidejas->gautiAkcijas() << endl;
                    }
                }
            }
            if (rinka->vykdytiDiena(aktyvas) && diena < ZAIDIMO_TRUKME) cout << "BREAKING NEWS! Rinkoje dideli pokyciai." << endl;
        }
        cout << "\nZAIDIMO PABAIGA. Galutine turto verte: " << zaidejas->turtoVerte(aktyvas->gautiKaina()) << " EUR." << endl;
    }
};

int main() {
    srand(time(0));
    leistiVisusTestus();
    Zaidimas game;
    game.start();
    return 0;
}