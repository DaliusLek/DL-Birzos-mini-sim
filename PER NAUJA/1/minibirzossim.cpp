#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

class Aktyvas {
private:
    string pavadinimas;
    double kaina;
    vector<double> istorija;

public:
    Aktyvas(string p, double k) {
        pavadinimas = p;
        kaina = k;
        istorija.push_back(k);
    }

    string gautiPavadinima() { return pavadinimas; }
    double gautiKaina() { return kaina; }
    vector<double> gautiIstorija() { return istorija; }

    void atnaujintiKaina(double naujaKaina) {
        if (naujaKaina < 0.10) naujaKaina = 0.10;
        kaina = naujaKaina;
        istorija.push_back(naujaKaina);
    }

    double gautiSMA(int periodas) {
        if (istorija.size() < periodas) return 0.0;
        double suma = 0;
        for (int i = 0; i < periodas; i++) {
            suma += istorija[istorija.size() - 1 - i];
        }
        return suma / periodas;
    }
};

class Zaidejas {
private:
    double balansas;
    int turimosAkcijos;

public:
    Zaidejas(double start) {
        balansas = start;
        turimosAkcijos = 0;
    }

    double gautiBalansa() { return balansas; }
    int gautiAkcijas() { return turimosAkcijos; }

    void pirkti(Aktyvas* a, int kiekis) {
        if (kiekis <= 0) {
            cout << "Klaida: Kiekis turi buti teigiamas." << endl;
            return;
        }

        double kaina = a->gautiKaina() * kiekis;
        double mokestis = kaina * 0.01;
        double viso = kaina + mokestis;

        if (balansas >= viso) {
            balansas -= viso;
            turimosAkcijos += kiekis;
            cout << "Nupirkta " << kiekis << " vnt. Liko pinigu: " << balansas << endl;
        } else {
            cout << "Klaida: Nepakanka pinigu. Reikia: " << viso << endl;
        }
    }

    void parduoti(Aktyvas* a, int kiekis) {
        if (kiekis <= 0 || turimosAkcijos < kiekis) {
            cout << "Klaida: Neturite tiek akciju." << endl;
            return;
        }

        double kaina = a->gautiKaina() * kiekis;
        double mokestis = kaina * 0.01;
        double viso = kaina - mokestis;

        balansas += viso;
        turimosAkcijos -= kiekis;
        cout << "Parduota " << kiekis << " vnt. Gavote: " << viso << endl;
    }

    double gautiTurtoVerte(double kaina) {
        return balansas + (turimosAkcijos * kaina);
    }
};

class Rinka {
private:
    int modelis;

public:
    Rinka(int m) {
        modelis = m;
    }

    void keistiKaina(Aktyvas* a) {
        double min_pokytis, max_pokytis;

        if (modelis == 2) {
            min_pokytis = -0.05; max_pokytis = 0.20;
        } else if (modelis == 3) {
            min_pokytis = -0.20; max_pokytis = 0.05;
        } else {
            min_pokytis = -0.10; max_pokytis = 0.10;
        }

        double r = (double)rand() / RAND_MAX;
        double pokytis = min_pokytis + r * (max_pokytis - min_pokytis);

        double newsChance = (double)rand() / RAND_MAX;
        if (newsChance < 0.10) {
            cout << "BREAKING NEWS! Rinka stipriai sujudejo!" << endl;
            pokytis *= 2.0;
        }

        double naujaKaina = a->gautiKaina() * (1.0 + pokytis);
        a->atnaujintiKaina(naujaKaina);
    }
};

class Zaidimas {
private:
    Aktyvas* aktyvas;
    Zaidejas* zaidejas;
    Rinka* rinka;
    double pradinisKapitalas;

public:
    Zaidimas() {
        aktyvas = nullptr;
        zaidejas = nullptr;
        rinka = nullptr;
    }

    ~Zaidimas() {
        if (aktyvas) delete aktyvas;
        if (zaidejas) delete zaidejas;
        if (rinka) delete rinka;
    }

    void start() {
        cout << "=== BIRZOS SIMULIATORIUS ===" << endl;

        cout << "Iveskite pradini balansa: ";
        cin >> pradinisKapitalas;
        zaidejas = new Zaidejas(pradinisKapitalas);

        cout << "Iveskite pradine akcijos kaina: ";
        double kaina;
        cin >> kaina;
        aktyvas = new Aktyvas("ManoCoin", kaina);

        cout << "Pasirinkite rinka (1-Normal, 2-Bull, 3-Bear): ";
        int m;
        cin >> m;
        rinka = new Rinka(m);

        int dienos = 10;
        for (int d = 1; d <= dienos; d++) {
            if (zaidejas->gautiTurtoVerte(aktyvas->gautiKaina()) < pradinisKapitalas * 0.20) {
                cout << "BANKROTAS! Zaidimas baigtas." << endl;
                break;
            }

            cout << "\n--- DIENA " << d << " ---" << endl;
            cout << "Kaina: " << fixed << setprecision(2) << aktyvas->gautiKaina() << " EUR";

            vector<double> ist = aktyvas->gautiIstorija();
            if (ist.size() > 1) {
                double sena = ist[ist.size() - 2];
                double pokytis = ((aktyvas->gautiKaina() - sena) / sena) * 100.0;
                cout << " (";
                if (pokytis >= 0) cout << "+";
                cout << pokytis << "%)";
            }

            double sma = aktyvas->gautiSMA(3);
            if (sma > 0) cout << " [SMA-3: " << sma << "]";
            cout << endl;

            cout << "Balansas: " << zaidejas->gautiBalansa() << " | Akcijos: " << zaidejas->gautiAkcijas() << endl;

            while (true) {
                cout << "[1] Pirkti [2] Parduoti [3] Laukti: ";
                int veiksmas;
                cin >> veiksmas;

                if (veiksmas == 3) break;

                cout << "Kiekis: ";
                int kiekis;
                cin >> kiekis;

                if (veiksmas == 1) {
                    zaidejas->pirkti(aktyvas, kiekis);
                    break;
                } else if (veiksmas == 2) {
                    zaidejas->parduoti(aktyvas, kiekis);
                    break;
                } else {
                    cout << "Neteisingas pasirinkimas." << endl;
                }
            }

            rinka->keistiKaina(aktyvas);
        }

        cout << "\nZAIDIMO PABAIGA. Galutine verte: " << zaidejas->gautiTurtoVerte(aktyvas->gautiKaina()) << endl;
    }
};

int main() {
    srand(time(0));

    Zaidimas game;
    game.start();

    return 0;
}