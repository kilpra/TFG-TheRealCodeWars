import random
import RealCodeWars

class Bot(RealCodeWars.RCW):
    # Override of RCW_turn to implement actions
    def RCW_turn(self, soldiers):
        # Iterate through all soldiers and decide what to do
        for soldier in soldiers:
            # Check if Soldier has actions available
            while soldier.actionsRemaining > 0:
                tiles = self.RCW_getTiles(soldier)
                # Error control
                if not tiles:
                    return
                
                 # Save actions to check if unit did something or is stuck at the end of its turn
                actions = soldier.actionsRemaining

                # Differentiate between different types of soldiers by their name
                if soldier.name == 'Base':
                    # Create a random soldier.
                    # (In your code you may want to check if the base can actually create soldiers by checking if any of the 4 nearby tiles are free.)
                    rand = random.randint(1, 4)

                    if rand == 1:
                        soldier_name = 'Helicopter'
                    elif rand == 2:
                        soldier_name = 'Tank'
                    elif rand == 3:
                        soldier_name = 'Healer'
                    elif rand == 4:
                        soldier_name = 'Attacker'

                    soldier = self.RCW_createSoldier(soldier, soldier_name)

                    if not soldier:
                        return
                else:
                    if soldier.name == "Healer":
                        can_heal = False
                        pos = False
                        # Find a suitable tile to heal
                        for tile in tiles:
                            if tile.occupied and not tile.enemy:
                                # Maybe you can also check if ally has less health than his max Health... :)
                                can_heal = True
                                pos = tile
                                break

                        if can_heal:
                            soldier = self.RCW_heal(soldier, pos)
                            if not soldier:
                                return
                    else:
                        can_attack = False
                        pos = False
                
                        # Find a suitable tile to attack
                        for tile in tiles:
                            if tile.occupied and tile.enemy:
                                can_attack = True
                                pos = tile
                                break

                        if can_attack:
                            soldier = self.RCW_attack(soldier, pos)
                            if not soldier:
                                return

                        # Move in a random direction
                        rand = random.randint(1, 4)

                        if rand == 1:
                            direction = 'up'
                        elif rand == 2:
                            direction = 'right'
                        elif rand == 3:
                            direction = 'down'
                        elif rand == 4:
                            direction = 'left'

                        soldier = self.RCW_move(soldier, direction)

                        if not soldier:
                            return
                    # Avoid infinite loop if soldier is stuck
                    if actions == soldier.actionsRemaining:
                        break
        # End turn
        self.RCW_end_turn()


if __name__ == "__main__":
    # Initialize and start the bot
    bot = Bot()
    bot.RCW_start()
    