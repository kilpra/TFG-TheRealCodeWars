const form = document.querySelector('form');

document.addEventListener('DOMContentLoaded', function() {
    var usersA = document.getElementById('userA');
    var usersB = document.getElementById('userB');

    fetch('/getUsers', {
        credentials: 'same-origin'
      })
        .then(function(response) {
            if (response.ok) {
            return response.text();
            } else {
            throw new Error('Error en la respuesta de la solicitud');
            }
        })
        .then(function(data) {
            var userList = JSON.parse(data);
            console.log(userList);
            return userList;
        })
        .then(function(json) {
            json.forEach(function(user) {
                var listItem = document.createElement('option');
                listItem.textContent = user;
                var listItem2 = document.createElement('option');
                listItem2.textContent = user;
                usersA.appendChild(listItem);
                usersB.appendChild(listItem2);
            });
        })
        .catch(function(error) {
        console.log('Error al obtener la lista de archivos:', error);
    });

    var maps = document.getElementById('map');

    fetch('/getMapsNames', {
        credentials: 'same-origin'
      })
        .then(function(response) {
            if (response.ok) {
            return response.text();
            } else {
            throw new Error('Error en la respuesta de la solicitud');
            }
        })
        .then(function(data) {
            var userList = JSON.parse(data);
            console.log(userList);
            return userList;
        })
        .then(function(json) {
            json.forEach(function(user) {
                var listItem = document.createElement('option');
                listItem.textContent = user;
                map.appendChild(listItem);
            });
        })
        .catch(function(error) {
        console.log('Error al obtener la lista de archivos:', error);
    });
});

function getFilesNames(userName, code){
    var bots = document.getElementById(code);
    bots.replaceChildren();

    var formData  = new FormData();
    formData.append('user', userName)

    fetch('/getFilesNames', {
        method: 'POST',
        credentials: 'same-origin',
        body: formData
    })
    .then(function(response) {
        if (response.ok) {
        return response.text();
        } else {
        throw new Error('Error en la respuesta de la solicitud');
        }
    })
    .then(function(data) {
        var fileList = JSON.parse(data);
        console.log(fileList);
        return fileList;
    })
    .then(function(json) {
        json.forEach(function(fileName) {
            var listItem = document.createElement('option');
            listItem.textContent = fileName;
            bots.appendChild(listItem);
        });
    })
    .catch(function(error) {
    console.log('Error al obtener la lista de archivos:', error);
    });
}

$('#userA').change(function(){
    user = document.getElementById('userA');
    getFilesNames(user.options[user.selectedIndex].text, 'codeA')
});

$('#userB').change(function(){
    user = document.getElementById('userB');
    getFilesNames(user.options[user.selectedIndex].text, 'codeB')
});

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
                text.textContent = "No hay batallas en curso";
                link.hidden = true;
                document.getElementById("submit").removeAttribute("disabled");
            }
            else {
                let link = document.getElementById("watchLink");
                let text = document.getElementById("gameOngoing");
                text.textContent = "Hay una partida en curso. Espera a que acabe(info actualizada cada 1s). Puedes verla en el enlace:";
                link.hidden = false;
                document.getElementById("submit").setAttribute("disabled", "disabled");
            }
        } else {
            console.log("Request failed. Status: " + xhr.status + " " + xhr.responseURL + " " + xhr.responseText);
        }
    };

    xhr.open("GET", protocol + "//" + host + "/gameStatus");
    xhr.send();
}

document.addEventListener('DOMContentLoaded', gameOngoing());
setInterval(function(){gameOngoing()}, 1000);

form.addEventListener('submit', (event) => {
    // Evitar que la página se recargue después de enviar el formulario
    event.preventDefault();

    var host = window.location.hostname;
    var protocol = window.location.protocol;
    const url = protocol + "//" + host + "/watchGame";
    var formData  = new FormData(form);

    fetch('/runGame', {
        credentials: 'same-origin',
        method: 'POST', 
        body: formData ,
    })

    window.location.href = url;
});