import random
import RealCodeWars

class Bot(RealCodeWars.RCW):
    def RCW_turn(self, soldiers):
        for soldier in soldiers:
            while soldier.actionsRemaining > 0:
                tiles = self.RCW_getTiles(soldier)
                if not tiles:
                    return

                # Save actions to check if unit did something or is stuck at the end of its turn
                actions = soldier.actionsRemaining

                if soldier.name == 'Base':
                    can_create = False

                    # Check if there is a free tile near the base, so that create won´t fail and loose the turn.
                    for tile in tiles:
                        if not tile.occupied:
                            can_create = True
                            break

                    if can_create:
                        # Create a random soldier
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
                    # Avoid infinite loop if Base can not create.
                    else:
                        break
                else:
                    if soldier.name == "Healer":
                        can_heal = False
                        pos = False
                        # Find a suitable tile to heal
                        for tile in tiles:
                            if tile.occupied and not tile.enemy:
                                ally = RealCodeWars.get_soldier_at_pos(soldiers, tile)
                                if ally and ally.name != "Base" and ally.health < ally.maxHealth:
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
                    if soldier.actionsRemaining > 0:
                        distanceR = distanceL = distanceU = distanceD = 1000
                        can_move = {"up": False, "down": False, "left": False, "right": False}

                        # Check in which directions soldier can move
                        for tile in tiles:
                            if not tile.occupied:
                                # If soldier flies, it can move to any tile
                                if soldier.flies:
                                    # Get the direction in which soldier should move to reach tile
                                    direction = RealCodeWars.get_dir(soldier, tile)
                                    if direction:
                                        can_move[direction] = True
                                # If soldier swims, it can not go to lava tiles
                                elif soldier.swims and tile.type != 'lava':
                                    direction = RealCodeWars.get_dir(soldier, tile)
                                    if direction:
                                        can_move[direction] = True
                                # Else, it can only go to earth tiles
                                elif tile.type == 'earth':
                                    direction = RealCodeWars.get_dir(soldier, tile)
                                    if direction:
                                        can_move[direction] = True

                        # Only change directions in which soldier can move
                        if can_move["right"]:
                            distanceR = abs(self.enemy_base['posX'] - (soldier.posX + 1)) + abs(
                                self.enemy_base['posY'] - soldier.posY)

                        if can_move["left"]:
                            distanceL = abs(self.enemy_base['posX'] - (soldier.posX - 1)) + abs(
                                self.enemy_base['posY'] - soldier.posY)

                        if can_move["up"]:
                            distanceU = abs(self.enemy_base['posX'] - soldier.posX) + abs(
                                self.enemy_base['posY'] - (soldier.posY + 1))

                        if can_move["down"]:
                            distanceD = abs(self.enemy_base['posX'] - soldier.posX) + abs(
                                self.enemy_base['posY'] - (soldier.posY - 1))

                        # Minimize the distance to enemy base
                        distance = min(distanceR, distanceD, distanceU, distanceL)

                        direction = False

                        if can_move["up"] and distance == distanceU:
                            direction = 'up'
                        elif can_move["right"] and distance == distanceR:
                            direction = 'right'
                        elif can_move["down"] and distance == distanceD:
                            direction = 'down'
                        elif can_move["left"] and distance == distanceL:
                            direction = 'left'

                        if direction != False:
                            soldier = self.RCW_move(soldier, direction)

                        if not soldier:
                            return
                    # Avoid infinite loop if soldier is stuck
                    if actions == soldier.actionsRemaining:
                        break
        self.RCW_end_turn()


if __name__ == "__main__":
    bot = Bot()
    bot.RCW_start()
