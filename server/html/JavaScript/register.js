function checkUser() {
    var host = window.location.hostname;
    var protocol = window.location.protocol;
    var xhr = new XMLHttpRequest();
    
    xhr.open("POST", protocol + "//" + host + "/checkUser", false);

    xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
    var params = "user=" + document.getElementById("username").value;

    xhr.onload = function() {
        if (xhr.status === 200) {
            data = xhr.responseText;
        } else {
            console.log("Request failed. Status: " + xhr.status + " " + xhr.responseURL + " " + xhr.responseText);
        }
    };

    xhr.send(params);

    if (data == 'false') {
        return false;
    }
    return true;
}

function validarFormulario() {
    var password = document.getElementById("password-field").value;
    var confirmPassword = document.getElementById("passwordRep").value;
    
    if (password != confirmPassword) {
        alert("Passwords are not equal");
        return false;
    }

    if(! checkUser()) {
        alert("Username already exists");
        return false;
    }
    
    document.getElementById("passwordRep").value = "";
    document.getElementById("password-field").value = sha256(password);

    return true;
}