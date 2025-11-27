import random

class Aktyvas:
    def __init__(self, name, price):
        self.name=name
        self.price=price
        print(f"Sukurtas aktyvas {self.name}, \nPradinė kaina = {self.price} eur")

class Player:
    def __init__(self, pradinisbalansas):
        self.balansas=pradinisbalansas
        self.akcijos=0
        print(f"Žaidėjas sukurtas, balansas = {self.balansas} eur")

    def buy(self, aktyvas, kiekis):
        pass

    def sell(self, aktyvas, kiekis):
        pass

class Rinka:
    def generuoti_kainu_keitima(self, aktyvas):
        pass

class Zaidimas:
    def __init__(self):
        self.aktyvas=Aktyvas("Kripto-Moneta", 100.0)
        self.player= Player(1000.0)
        self.rinka=Rinka()

    def start(self):
        print("\n--- Žaidimas prasideda! ---")
        # ciklas

if __name__ == "__main__":
    app = Zaidimas()
    app.start()