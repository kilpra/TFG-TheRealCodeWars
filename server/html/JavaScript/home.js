function LogOut() {
    if(confirm("Se cerrará la sesión actual.\n ¿Desea continuar?")) {
        var cookies = document.cookie.split(";");
    
        // Set expiration of cookies in the past to delete them
        for (var i = 0; i < cookies.length; i++) {
        var cookie = cookies[i];
        var eqPos = cookie.indexOf("=");
        var nombre = eqPos > -1 ? cookie.substr(0, eqPos) : cookie;
        document.cookie = nombre + "=;expires=Thu, 01 Jan 1970 00:00:00 GMT;path=/";
        }
    }
  }

function gameOngoing() {
    var host = window.location.hostname;
    var protocol = window.location.protocol;
    var xhr = new XMLHttpRequest();

    xhr.onload = function() {
        if (xhr.status === 200) {
            data = JSON.parse(xhr.responseText);
            if(! data || data["winner"]) {
                let link = document.getElementById("watchLink");
                let text = document.getElementById("gameOngoing");
                text.textContent = "No hay batallas actualmente";
                link.hidden = true;
            }
            else {
                let link = document.getElementById("watchLink");
                let text = document.getElementById("gameOngoing");
                text.textContent = "Hay una partida en curso. Pulse en el enlace para verla";
                link.hidden = false;
            }
        } else {
            console.log("Request failed. Status: " + xhr.status + " " + xhr.responseURL + " " + xhr.responseText);
        }
    };

    xhr.open("GET", protocol + "//" + host + "/gameStatus");
    xhr.send();
}
// Get game info when loading page and each 3s elapsed
window.addEventListener('DOMContentLoaded', gameOngoing());
setInterval(function(){gameOngoing()},3000);