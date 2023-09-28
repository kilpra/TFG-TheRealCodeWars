document.addEventListener('DOMContentLoaded', function() {
    var fileListElement = document.getElementById('list');

    function handleDeleteButtonClick(fileName) {
        if (confirm('¿Estás seguro de que deseas eliminar el archivo "' + fileName + '"?')) {
            var formData = new FormData();
            formData.append('File', fileName);

            fetch('/deleteFile', {
                credentials: 'same-origin',
                method: 'POST',
                body: formData
            })
            .then(function(response) {
                if (response.ok) {
                console.log('Archivo eliminado con éxito');                
                } else {
                    alert('Oops, Ha ocurrido un error');
                }
                location.reload();
            })
            .catch(function(error) {
                console.log('Error en la solicitud:', error);
            });
        }
    }

    function handleViewCodeButtonClick(fileName) {
        var formData  = new FormData();
        formData.append('FileName', fileName)

        fetch('/getFile', {
            method: 'POST', 
            body: formData ,
            credentials: "same-origin"
        })
        .then(function(response) {
            if (response.ok) {
                return response.text();
            } else {
                throw new Error('Error en la respuesta de la solicitud');
            }
        })
        .then(function(fileText) {
            let newWindow = window.open();
            newWindow.document.open();
            newWindow.document.write('<pre>' + fileText + '</pre>');
            newWindow.document.close();
        })
        .catch(function(error) {
            console.log('Error en la solicitud:', error);
        });
    }

    fetch('/getFilesNames', {
        credentials: 'same-origin',
        method: 'POST'
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
                var listItem = document.createElement('li');
                listItem.classList.add('list-group-item', 'd-flex', 'justify-content-between', 'align-items-center');
                listItem.id = fileName + '-item';
    
                var fileNameSpan = document.createElement('span');
                fileNameSpan.textContent = fileName;
    
                var buttonsDiv = document.createElement('div');
                buttonsDiv.classList.add('ml-auto');

                var deleteButton = document.createElement('button');
                deleteButton.classList.add('btn', 'btn-danger', 'mr-2');
                deleteButton.textContent = 'Borrar';
                deleteButton.addEventListener('click', function() {
                  handleDeleteButtonClick(fileName);
                });
    
                var viewCodeButton = document.createElement('button');
                viewCodeButton.classList.add('btn', 'btn-primary');
                viewCodeButton.textContent = 'Ver código';
                viewCodeButton.addEventListener('click', function() {
                  handleViewCodeButtonClick(fileName);
                });
    
                buttonsDiv.appendChild(deleteButton);
                buttonsDiv.appendChild(viewCodeButton);
    
                listItem.appendChild(fileNameSpan);
                listItem.appendChild(buttonsDiv);
    
                fileListElement.appendChild(listItem);
            });
        })
        .catch(function(error) {
        console.log('Error al obtener la lista de archivos:', error);
        });
});

$('#fileup').change(function(){
    //here we take the file extension and set an array of valid extensions
    var res=$('#fileup').val();
    var arr = res.split("\\");
    var filename=arr.slice(-1)[0];
    filextension=filename.split(".");
    filext="."+filextension.slice(-1)[0];
    valid=[".py"];
    //if file is not valid we show the error icon, the red alert, and hide the submit button
    if (valid.indexOf(filext.toLowerCase())==-1){
        $( ".imgupload" ).hide("slow");
        $( ".imgupload.ok" ).hide("slow");
        $( ".imgupload.stop" ).show("slow");

        $('#namefile').css({"color":"red","font-weight":700});
        $('#namefile').html("File "+filename+" is not .py!");

        $( "#submitbtn" ).hide();
        $( "#fakebtn" ).show();
    }else{
        //if file is valid we show the green alert and show the valid submit
        $( ".imgupload" ).hide("slow");
        $( ".imgupload.stop" ).hide("slow");
        $( ".imgupload.ok" ).show("slow");

        $('#namefile').css({"color":"green","font-weight":700});
        $('#namefile').html(filename);

        $( "#submitbtn" ).show();
        $( "#fakebtn" ).hide();
    }
});
const form = document.querySelector('form');
        
form.addEventListener('submit', (event) => {
    var formData  = new FormData(form);

    var fileSelected = document.getElementById('fileup');
    var files = fileSelected.files;

    for (let i = 0; i < files.length; i++) {
        const file = files[i];

        formData.append('File', file);
        formData.append('FileName', file.name)
    }


    fetch('/code', {
        method: 'POST', 
        body: formData ,
    })
});