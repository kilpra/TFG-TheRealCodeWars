window.addEventListener('DOMContentLoaded', function() {
    var host = window.location.hostname;
    var protocol = window.location.protocol;
    var xhr = new XMLHttpRequest();

    xhr.onload = function() {
        if (xhr.status === 200) {
            data = JSON.parse(xhr.responseText);
            if(data["winner"]) {
            var text = document.getElementById("winner");
            text.textContent = "Winner is " + data["winner"];
    }
        } else {
            console.log("Request failed. Status: " + xhr.status + " " + xhr.responseURL + " " + xhr.responseText);
        }
    };

    xhr.open("GET", protocol + "//" + host + "/gameStatus");
    xhr.send();
});