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
    
    def __init__(self, startmoney):
        self.balansas=startmoney
        self.akcijos=0
        self.mokestis=0.01
        print(f"Žaidėjas sukurtas. Balansas: {self.balansas}")

    def buy(self, aktyvas, kiekis):
        raw_cost=aktyvas.price*kiekis
        mokestis=raw_cost*0.01
        total_cost=raw_cost+mokestis
        if self.balansas>=total_cost:
            self.balansas-=total_cost
            self.akcijos+=kiekis
            print(f"NUPIRKTA: {kiekis} vnt. (suma: {round(raw_cost,2)} + mokestis: {round(mokestis,2)}. Liko: {round(self.balansas, 2)}")
            return True
        else:
            print(f"KLAIDA: Nepakanka pinigų.. Reikia {round(total_cost,2)}")
            return False
        
    def sell(self, aktyvas, kiekis):
        if self.akcijos>=kiekis:
            raw_income=aktyvas.price*kiekis
            mokestis=raw_income*self.mokestis
            total_income=raw_income-mokestis
            self.balansas+=total_income
            self.akcijos-=kiekis
            print(f"PARDUOTA {kiekis} vnt. Pajamos: {round(raw_income,2)} - mokestis ({round(mokestis,2)}). BALANSAS: {round(self.balansas,2)}")
            return True
        else:
            print(f"Klaida: neturite tiek akcijų!")
            return False


class Rinka:
    def __init__(self, model_type):
        self.model_type=model_type
    
    def generate_price_change(self, aktyvas):
        if self.model_type=="2":
            min_change, max_change = -0.1, 0.3

        elif self.model_type=="3":
            min_change, max_change= -0.3, 0.1

        else:
            min_change, max_change= -0.2, 0.2

        event_chance=random.random()

        if event_chance<0.1:
            if self.model_type=="2":
                change_percent=random.uniform(-0.1,0.3)
            elif self.model_type=="3":
                change_percent=random.uniform(-0.3,0.1)
            else:
                change_percent=random.uniform(-0.2,0.2)
            print(f"\n SVARBU! Rinkoje dideli pokyčiai!")
        else:
            change_percent=random.uniform(min_change, max_change)

        new_price = aktyvas.price*(1+change_percent)
        if new_price<0.01: new_price=0.01

        aktyvas.update_price(round(new_price,2))

    def sma(self, aktyvas, period):
        if len(aktyvas.history) < period: return 0.0
        return round(sum(aktyvas.history[-period:])/period, 2)

class Zaidimas:
    def __init__(self):
        ##self.aktyvas=Aktyvas("DaliusCoin", 100.0)
        ##self.player= Player(1000.0)
        ##self.rinka=Rinka()

        print("\n=== ŽAIDIMO KONFIGŪRACIJA ===")
        while True:
            try:
                pradine_suma=float(input("Įveskite pradinį pinigų balansą (pvz. 1000): "))
                if pradine_suma>0:
                    break
                else:
                    print("Klaida: Suma turi būti teigiama.")
            except ValueError:
                print("Klaida: Prašome įvesti skaičių.")
        
        while True:
            try:
                pradine_kaina=float(input("Įveskite pradinę akcijos kainą (pvz. 50): "))
                if pradine_suma>0:
                    break
                else:
                    print("Klaida: Kaina turi būti teigiama.")
            except ValueError:
                print("Klaida: Prašome įvesti skaičių!")

        print("\nPasirinkite rinkos modelį (sunkumo lygį):")
        print("1. Normali rinka (Balansuota)")
        print("2. Buliaus rinka (Tendencija kilti - Lengva)")
        print("3. Lokio rinka (Tendencija kristi - Sunku)")

        while True:
            model_choice = input("Pasirinkimas (1, 2 arba 3): ")
            if model_choice in ["1", "2", "3"]:
                break
            print("Neteisingas pasirinkimas. Įveskite 1, 2 arba 3.")




        self.starteur=pradine_suma
        self.aktyvas=Aktyvas("DaliusCoin", pradine_kaina)
        self.player=Player(pradine_suma)
        #self.rinka=Rinka()
        self.rinka=Rinka(model_choice)

    def start(self):
        print("\n=== SVEIKI ATVYKĘ Į BIRŽOS SIMULIATORIŲ ===")
        days_total=10
        
        for day in range(1, days_total+1):
            current_value=self.player.balansas+(self.player.akcijos*self.aktyvas.price)
            bankroto_riba=self.starteur*0.2
            if current_value<bankroto_riba:
                print(f"BANKROTAS!!! Jūsų turto vertė nukrito iki {round(current_value,2)} eur.")
                break
            print(f"\n--- DIENA {day} iš {days_total} ---")

            smaa=self.rinka.sma(self.aktyvas,3)
            change_str="(0.00%)"
            if len(self.aktyvas.history)>1:
                prev_price=self.aktyvas.history[-2]
                change_raw=((self.aktyvas.price-prev_price) / prev_price)*100
                if change_raw>=0:
                    change_str=f"(+{change_raw:.2f}%)"
                else:
                    change_str=f"({change_raw:.2f}%)"

            print(f"Akcija: {self.aktyvas.name} | Kaina: {self.aktyvas.price} Eur. {change_str}")

            if smaa>0:
                print(f"SMA-3 Indikatorius: {smaa} eur.")
                if(self.aktyvas.price>smaa):
                    print(" (kyla) ")
                else:
                    print(" (krenta)")
            else:
                print("(Indikatorius renka duomenis)")

            print(f"Tavo turtas: {round(self.player.balansas, 2)} eur | akcijos: {self.player.akcijos} vnt.")
            while True:
                print("\n[1] Pirkti (Buy)")
                print("[2] Parduoti (Sell)")
                print("[3] Laukti (Hold)")

                action=input("Pasirinkimas ")
                succesfull_turn=False
                
                if action=="1":
                    try:
                        kiekis=int(input("Kiek pirkti?: "))
                        if self.player.buy(self.aktyvas, kiekis):
                            succesfull_turn=True
                    except ValueError:
                        print("Neteisingas pasirinkimas. Bandykite dar kartą.")
                elif action=="2":
                    try:
                        kiekis=int(input("Kiek parduoti?: "))
                        if self.player.sell(self.aktyvas, kiekis):
                            succesfull_turn=True
                    except ValueError:
                        print("Klaida: Įveskite sveikąjį skaičių!")
                elif action=="3":
                    print("Laukiam...")
                    succesfull_turn=True

                else:
                    print("Neteisingas pasirinkimas. Bandykite darkart.")
                
                if succesfull_turn:
                    break
                else:
                    print("Veiksmas nepavyko arba įvyko klaida. Diena nepasikeitė.")
            self.rinka.generate_price_change(self.aktyvas)

        total_value=self.player.balansas+(self.player.akcijos*self.aktyvas.price)
        rezultatas=total_value-self.starteur

        print("\n=== ŽAIDIMO PABAIGA ===")
        print(f"Pradinis kapitalas: {self.starteur} eur.")
        print(f"Galutine verte: {round(total_value,2)} eur.")
        
        if rezultatas>0:
            print(f"PELNAS: {round(rezultatas,2)} eur.")
        elif rezultatas<0:
            print(f"NUOSTOLIS {round(abs(rezultatas), 2)}")
        else:
            print("Nei pelno, nei nuostoliu, saugu...")

        print("Geras bandymas!")

if __name__ == "__main__":
    app = Zaidimas()
    app.start()