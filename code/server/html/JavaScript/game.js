var maxTiles = 20;
var teamA_Color = '#ffcd6c';
var teamB_Color = '#304c4c';

// Check if container fits in screen (for smaller devices)
window.addEventListener("load", () => {
    const container = document.querySelector(".container");
    const content = document.querySelector(".content");

    if (content.offsetWidth > container.offsetWidth) {
        content.style.position = "absolute";
        content.style.left = "50%";
        content.style.transform = "translateX(-50%)";
    }
});

document.addEventListener('DOMContentLoaded', function(){
    var host = window.location.hostname;
    var protocol = window.location.protocol;
    var xhr = new XMLHttpRequest();

    xhr.open("GET", protocol + "//" + host + "/teamNames", false);

    xhr.onload = function() {
    if (xhr.status === 200) {
        data = JSON.parse(xhr.responseText);
        if(data["TeamA"] && data["TeamB"]) {
            var text = document.getElementById("TeamAName");
            text.textContent = "Equipo A: " + data["TeamA"];
            text.style.color = teamA_Color;

            text = document.getElementById("TeamBName");
            text.textContent = "Equipo B: " + data["TeamB"];
            text.style.color = teamB_Color;
        }
    } else {
        console.log("Request failed. Status: " + xhr.status + " " + xhr.responseURL + " " + xhr.responseText);
    }
    };

    xhr.send(); 
});


setInterval(
    function() {
        var host = window.location.hostname;
        var protocol = window.location.protocol;
        var xhr = new XMLHttpRequest();

        xhr.open("GET", protocol + "//" + host + "/gameStatus", false);

        xhr.onload = function() {
        if (xhr.status === 200) {
            data = JSON.parse(xhr.responseText);
        } else {
            console.log("Request failed. Status: " + xhr.status + " " + xhr.responseURL + " " + xhr.responseText);
        }
        };

        xhr.send();

        if(data["winner"]) {
            var host = window.location.hostname;
            var protocol = window.location.protocol;
            const url = protocol + "//" + host + "/winner";
            window.location.href = url;
            return;
        }

        var table = document.getElementById("myTable");

        for (var i = 0, row; row = table.rows[i]; i++) {
            for (var j = 0, cell; cell = row.cells[j]; j++) {
                cell.innerHTML = '';
                switch (data[maxTiles - i - 1][j].type) {
                    case 0: // Earth
                        tile_png = "/images/earth.png";
                        break;
                    case 1: // Water
                        tile_png = "/images/water.png";
                        break;
                    case 2: // Lava
                        tile_png = "/images/lava.png";
                        break;
                }

                cell.setAttribute("title", '');
                
                var title = "X: " + data[maxTiles - i - 1][j].x + " Y: " + data[maxTiles - i - 1][j].y;

                cell.innerHTML += "<img class=\"tile\" style= \"position: absolute;top: 0;left: 0; object-fit: cover;  width: 100%; height: 100%;\" src=\"" + tile_png +"\">";

                if(data[maxTiles - i -1][j].soldier !== undefined) {
                    if(data[maxTiles - i -1][j].soldier.name !== undefined && data[maxTiles - i -1][j].soldier.health > 0) {
                        switch (data[maxTiles - i -1][j].soldier.name) {
                            case "Tank":
                                soldier_png = "/images/tank.png";
                                break;
                            case "Helicopter":
                                soldier_png = "/images/helicopter.png";
                                break;
                            case "Healer":
                                soldier_png = "/images/healer.png";
                                break;
                            case "Attacker":
                                soldier_png = "/images/attacker.png";
                                break;
                            case "Base":
                                soldier_png = "/images/base.png";
                                break;
                        }
                        
                        title += " Soldier: " + data[maxTiles - i -1][j].soldier.name + " ATK: " + data[maxTiles - i -1][j].soldier.attack + " HP: " + data[maxTiles - i -1][j].soldier.health + "/" + data[maxTiles - i -1][j].soldier.maxHealth;

                        var color = teamB_Color;

                        if(data[maxTiles - i -1][j].soldier.team == 'TeamA') {
                            var color = teamA_Color; 
                        }

                        cell.innerHTML += '<progress id="health" class="' + data[maxTiles - i -1][j].soldier.team + '" value="' + data[maxTiles - i -1][j].soldier.health + '" max="'+ data[maxTiles - i -1][j].soldier.maxHealth +'"></progress>';
                        if (data[maxTiles - i -1][j].soldier.attack > 0) {
                            cell.innerHTML += '<div style="position: absolute; top: 0; right: 0;font-size: 50%;font-weight: bold; z-index: 1000; color: ' +color+'"><b>' + data[maxTiles - i -1][j].soldier.attack + ' ATK</b></div>';
                        }
                        if (data[maxTiles - i -1][j].soldier.healing > 0) {
                            cell.innerHTML += '<div style="position: absolute; top: 0; left: 0;font-size: 50%;font-weight: bold; z-index: 1000; color: ' +color+'"><b>' + data[maxTiles - i -1][j].soldier.healing + ' Heal</b></div>';
                        }
                        
                        // Set Different position inside the cell depending on the soldier.
                        if(soldier_png == "/images/base.png") {
                            cell.innerHTML += "<img class=\"soldier\" style= \"position: absolute;top: 0;left: 0; object-fit: cover;  width: 100%;height: 100%;top: 0%;left: 0%;\" src=\"" + soldier_png + "\">";
                        
                        }else if (soldier_png == "/images/healer.png") {
                            cell.innerHTML += "<img class=\"soldier\" style= \"position: absolute;top: 0;left: 0; object-fit: cover;  width: 60%;height: 70%;top: 15%;left: 20%;\" src=\"" + soldier_png + "\">";
                            
                        } else if(soldier_png == "/images/tank.png") {
                            cell.innerHTML += "<img class=\"soldier\" style= \"position: absolute;top: 0;left: 0; object-fit: cover;  width: 65%;height: 60%;top: 23%;left: 20%;\" src=\"" + soldier_png + "\">";
                        
                        } else if(soldier_png == "/images/attacker.png") {
                            cell.innerHTML += "<img class=\"soldier\" style= \"position: absolute;top: 0;left: 0; object-fit: cover;  width: 70%;height: 70%;top: 15%;left: 15%;\" src=\"" + soldier_png + "\">";
                        
                        }
                        else {
                            cell.innerHTML += "<img class=\"soldier\" style= \"position: absolute;top: 0;left: 0; object-fit: cover;  width: 50%;height: 50%;top: 25%;left: 25%;\" src=\"" + soldier_png + "\">";
                        }
                    }
                }

                cell.setAttribute("title", title);
            }  
        }
    },
    300
);