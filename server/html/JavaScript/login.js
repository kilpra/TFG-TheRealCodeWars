function hashPassword() {
    var password = document.getElementById("password-field").value;
    var hashedPassword = sha256(password);
    document.getElementById("password-field").value = hashedPassword;
}