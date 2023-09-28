import socket
import time
import json


class Tile:
    def __init__(self, tile_json):
        self.__posX = tile_json["posX"]
        self.__posY = tile_json["posY"]
        self.__occupied = tile_json["occupied"]
        self.__enemy = tile_json["enemy"]
        self.__type = tile_json["type"]

    @property
    def posX(self):
        return self.__posX

    @property
    def posY(self):
        return self.__posY

    @property
    def occupied(self):
        return self.__occupied

    @property
    def enemy(self):
        return self.__enemy

    @property
    def type(self):
        return self.__type


class Soldier:
    def __init__(self, soldier_json):
        self.__name = soldier_json["name"]
        self.__swims = soldier_json["swims"]
        self.__flies = soldier_json["flies"]
        self.__range = soldier_json["range"]
        self.__attack = soldier_json["attack"]
        self.__healingPoints = soldier_json["healingPoints"]
        self.__health = soldier_json["health"]
        self.__maxHealth = soldier_json["maxHealth"]
        self.__team = soldier_json["team"]
        self.__index = soldier_json["index"]
        self.__posX = soldier_json["posX"]
        self.__posY = soldier_json["posY"]
        self.__actionsRemaining = soldier_json["actionsRemaining"]

    @property
    def name(self):
        return self.__name

    @property
    def swims(self):
        return self.__swims

    @property
    def flies(self):
        return self.__flies

    @property
    def range__val(self):
        return self.__range

    @property
    def attack(self):
        return self.__attack

    @property
    def healingPoints(self):
        return self.__healingPoints

    @property
    def health(self):
        return self.__health

    @property
    def maxHealth(self):
        return self.__maxHealth

    @property
    def team(self):
        return self.__team

    @property
    def index(self):
        return self.__index

    @property
    def posX(self):
        return self.__posX

    @property
    def posY(self):
        return self.__posY

    @property
    def actionsRemaining(self):
        return self.__actionsRemaining


def get_dir(soldier: Soldier, tile: Tile):
    """Returns direction(up, down, left, right) in which tile is from soldier or
    False if is not a neighbouring tile"""
    if soldier.posX == tile.posX and tile.posY == soldier.posY + 1:
        return "up"
    if soldier.posX == tile.posX and tile.posY == soldier.posY - 1:
        return "down"
    if tile.posY == soldier.posY and tile.posX == soldier.posX + 1:
        return "right"
    if tile.posY == soldier.posY and tile.posX == soldier.posX - 1:
        return "left"

    return False


def get_soldier_at_pos(soldiers: list[Soldier], tile: Tile) -> bool | Soldier:
    """Returns the soldier at tile, or False if tile does not have a soldier"""
    for soldier in soldiers:
        if soldier.posX == tile.posX and soldier.posY == tile.posY:
            return soldier

    return False


class RCW:
    def __init__(self):
        self.BUFFER_SIZE = 4096
        self.TCP_IP = 'localhost'
        self.TCP_PORT = 8080
        self.enemy_base = {'posX': -1, 'posY': -1}
        self.clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def __handle_response(self, response):
        if response.decode().find('EXIT\n') != -1:
            print('Cerrando Socket')
            self.clientSocket.close()
            exit(0)

        if response.decode().find('END\n') != -1:
            print('END recibido')
            message = 'OK\n'
            self.clientSocket.sendto(message.encode(), (self.TCP_IP, self.TCP_PORT))
            return False

        out = json.loads(response.decode())

        if 'Error' in out:
            print('Error:' + out['Error'])
            return False

        return out

    def RCW_end_turn(self):
        """This should be called when turn is finished, if not, bot will get stuck at the end of the turn"""
        message = "DONE\n"
        self.clientSocket.sendto(message.encode(), (self.TCP_IP, self.TCP_PORT))

    def RCW_turn(self, data):
        """This should be implemented by the student in their bot.py file. It is where all the logic
        of processing the turn will be.
        """
        raise NotImplementedError("Please Implement this method")

    def RCW_move(self, soldier: Soldier, direction):
        """Move soldier in direction.\n
        Posible directions: 'up', 'down', 'left', 'right'.\n
        Returns soldier with updated params or False if error."""
        message = {
            'action': 'move',
            'direction': direction,
            'index': soldier.index,
        }

        message = json.dumps(message)
        self.clientSocket.sendto(message.encode(), (self.TCP_IP, self.TCP_PORT))
        response = self.__handle_response(self.clientSocket.recv(self.BUFFER_SIZE))

        if not response:
            return False

        return Soldier(response)

    def RCW_attack(self, soldier: Soldier, tile: Tile):
        """Attacks a tile on the map.\n
         Returns soldier with updated params or False if error."""
        message = {
            'action': 'attack',
            'posX': tile.posX,
            'posY': tile.posY,
            'index': soldier.index,
        }

        message = json.dumps(message)
        self.clientSocket.sendto(message.encode(), (self.TCP_IP, self.TCP_PORT))
        response = self.__handle_response(self.clientSocket.recv(self.BUFFER_SIZE))

        if not response:
            return False

        return Soldier(response)

    def RCW_heal(self, soldier: Soldier, tile: Tile):
        """Heals a tile on the map.\n
         Only units with healing points can do this, others will lose the action.\n
         Returns soldier with updated params or False if error."""
        message = {
            'action': 'heal',
            'posX': tile.posX,
            'posY': tile.posY,
            'index': soldier.index,
        }

        message = json.dumps(message)
        self.clientSocket.sendto(message.encode(), (self.TCP_IP, self.TCP_PORT))
        response = self.__handle_response(self.clientSocket.recv(self.BUFFER_SIZE))

        if not response:
            return False

        return Soldier(response)

    def RCW_createSoldier(self, soldier: Soldier, soldier_name):
        """Creates a soldier of type: soldier_name.\n
         Returns soldier with updated params or False if error."""
        message = {
            'action': 'create',
            'soldier': soldier_name,
            'index': soldier.index,
        }

        message = json.dumps(message)
        self.clientSocket.sendto(message.encode(), (self.TCP_IP, self.TCP_PORT))
        response = self.__handle_response(self.clientSocket.recv(self.BUFFER_SIZE))

        if not response:
            return False

        return Soldier(response)

    def RCW_getTiles(self, soldier: Soldier):
        """Gets an array with the tiles in rage of soldier.\n
         Returns False if error occurred"""
        message = {
            'action': 'getNearTiles',
            'index': soldier.index,
        }

        message = json.dumps(message)
        self.clientSocket.sendto(message.encode(), (self.TCP_IP, self.TCP_PORT))
        response = self.__handle_response(self.clientSocket.recv(self.BUFFER_SIZE))

        if not response:
            return False

        tiles = []

        for tile in response["nearTiles"]:
            tiles.append(Tile(tile))

        return tiles

    def __RCW_connect(self):
        """Connects to the server via TCP."""
        loop = True
        while loop:
            try:
                self.clientSocket.connect((self.TCP_IP, self.TCP_PORT))
                loop = False
            except:
                time.sleep(2)

        # Set enemy_base position

        data = self.clientSocket.recv(self.BUFFER_SIZE)
        data = json.loads(data.decode())
        self.enemy_base['posX'] = data['posX']
        self.enemy_base['posY'] = data['posY']
        message = 'OK\n'
        self.clientSocket.sendto(message.encode(), (self.TCP_IP, self.TCP_PORT))
        return

    def RCW_start(self):
        """Connects to the server via TCP and Starts the server loop."""
        self.__RCW_connect()

        while True:
            data = b""
            while True:
                response = self.clientSocket.recv(self.BUFFER_SIZE)
                data += response
                if data.endswith(b'\n'):
                    break

            data = data.decode().rstrip('\n')

            if data.find('EXIT') != -1:
                print('Cerrando Socket')
                self.clientSocket.close()
                return 0

            data = json.loads(data)

            if 'Error' in data:
                print('Error:' + data['Error'])
                continue

            soldiers = []

            for soldier in data:
                soldiers.append(Soldier(soldier))

            self.RCW_turn(soldiers)
