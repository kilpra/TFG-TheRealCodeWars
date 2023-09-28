import random
import time
import RealCodeWars

class Bot(RealCodeWars.RCW):
    def RCW_turn(self, soldiers):
        time.sleep(60)
        self.RCW_end_turn()


if __name__ == "__main__":
    bot = Bot()
    bot.RCW_start()
