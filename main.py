import random

class Aktyvas:
    def __init__(self, name, price):
        self.name=name
        self.price=price
        self.history=[price]
        print("\n------------------------------------------")
        print(f"Sukurtas aktyvas {self.name}, \nPradinė kaina = {self.price} eur")

    def update_price(self, new_price):
        self.price=new_price
        self.history.append(new_price)

class Player:
    def __init__(self, pradinisbalansas):
        self.balansas=pradinisbalansas
        self.akcijos=0
        self.mokestis=2.00
        print(f"Žaidėjas sukurtas, balansas = {self.balansas} eur")
        print("------------------------------------------")
        print("Dėmesio! Jei jūsų turto vertė kris\nžemiau 10, bus skelbiamas bankrotas \nir žaidimas bus baigtas!")
        print("------------------------------------------\n")

    def buy(self, aktyvas, kiekis):
        raw_cost=aktyvas.price*kiekis
        total_cost=raw_cost+self.mokestis

        if self.balansas>=total_cost:
            self.balansas-=total_cost
            self.akcijos+=kiekis
            print(f"NUPIRKOTE: {kiekis} vnt. po {aktyvas.price}, + mokestis: {self.mokestis}. Liko {round(self.balansas, 2)}")
        else:
            print(f"KLAIDA: Nepakanka pinigų! Reikia {total_cost} (Įskaitant {self.mokestis} eur. mokestį), turi {round(self.balansas, 2)}")

    def sell(self, aktyvas, kiekis):
        if self.akcijos>=kiekis:
            raw_income=aktyvas.price*kiekis
            total_income=raw_income-self.mokestis

            #total_income=aktyvas.price*kiekis
            self.balansas+=total_income
            self.akcijos-=kiekis
            print(f"PARDUOTA: {kiekis} vnt. (Kaina: {raw_income} - Mokestis: {self.mokestis}). Balansas: {round(self.balansas, 2)}")
        else:
            print(f"KLAIDA: Neturite tiek akciju! Turite: {self.akcijos}, bandote parduoti: {kiekis}")

class Rinka:
    def generuoti_kainu_keitima(self, aktyvas):
        event_chance = random.random()

        if event_chance < 0.1:
            keitimas = random.uniform(-0.3, 0.3)
            print(f"\nSVARBU! Rinka nestabili...")
        else:
            keitimas=random.uniform(-0.1, 0.1)

        

        #keitimas=random.uniform(-0.05, 0.05)
        nauja_kaina=aktyvas.price*(1+keitimas)

        #print(f"[test] senas: {aktyvas.price}, pokytis: {keitimas:.2f}, naujas: {nauja_kaina:.2f}")

        if nauja_kaina<0.01:
            nauja_kaina=0.01
        
        aktyvas.update_price (round(nauja_kaina, 2))
    
    def SMA(self, aktyvas, period):
        if len(aktyvas.history)<period:
            return 0.0
        last_prices=aktyvas.history[-period:]

        average=sum(last_prices)/period
        return round(average, 2)

class Zaidimas:
    def __init__(self):
        self.aktyvas=Aktyvas("DaliusCoin", 100.0)
        self.player= Player(1000.0)
        self.rinka=Rinka()

    def start(self):
        print("\n=== SVEIKI ATVYKĘ Į BIRŽOS SIMULIATORIŲ ===")
        days_total=10

        for day in range(1, days_total+1):
            current_value=self.player.balansas+(self.player.akcijos*self.aktyvas.price)
            if current_value<10.0:
                print(f"\n BANKROTAS! Jūsų vertė nukrito iki {round(current_value,2)} eur.")
                print("Žaidimas baigiamas anksčiau laiko...")
                break


            print(f"\n--- DIENA {day} iš {days_total} ---")

            smaa=self.rinka.SMA(self.aktyvas,3)

            print(f"Akcija: {self.aktyvas.name} | Kaina: {self.aktyvas.price}")

            if smaa>0:
                print(f"SMA-3 Indikatorius: {smaa} eur.")
                if self.aktyvas.price > smaa:
                    print(f"Tendencija: Kyla. Kaina aukščiau vidurkio.")
                else:
                    print(f"Tendencija: Krenta. Kaina žemiau vidurkio.")
            else:
                print("(Indikatorius renkasi duomenis...)")

            print(f"Tavo turtas: {round(self.player.balansas,2)} eur | Akcijos: {self.player.akcijos} vnt.")

            print("[1] Pirkti (BUY)")
            print("[2] Parduoti (SELL)")
            print("[3] Laukti (HOLD)")

            action=input("Pasirinkimas: ")

            if action=="1":
                try:
                    kiekis=int(input("Kiek pirkti? "))
                    self.player.buy(self.aktyvas, kiekis)
                except ValueError:
                    print("Klaida: Įveskite skaičių.")

            elif action=="2":
                try:
                    kiekis=int(input("Kiek parduoti? "))
                    self.player.sell(self.aktyvas, kiekis)
                except ValueError:
                    print("Klaida: Įveskite skaičių.")
            
            elif action=="3":
                print("Laukiame geresnių laikų...")
            
            else:
                print("Neteisingas pasirinkimas, dabar praleisime ėjimą :)")

            self.rinka.generuoti_kainu_keitima(self.aktyvas)

        total_value=self.player.balansas+(self.player.akcijos*self.aktyvas.price)
        print("\n=== ŽAIDIMO PABAIGA ===")
        print(f"Galutinė vertė: {round(total_value, 2)} eur.")


        #print("\n--- Žaidimas prasideda! ---")
        # ciklas
        #print(f"Pradinė {self.aktyvas.name} kaina = {self.aktyvas.price}")

        #for day in range(1, 11):
        #    self.rinka.generuoti_kainu_keitima(self.aktyvas)
        #    print(f"Diena {day}: kaina tapo {self.aktyvas.price}")
#
         #   if day==1:
         #       print("--- Bandome pirkti 5 vnt. ---")
          #      self.player.buy(self.aktyvas, 5)

if __name__ == "__main__":
    app = Zaidimas()
    app.start()