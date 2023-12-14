
function ls(ls_callback){
    const Http = new XMLHttpRequest(); // it sucks that it's an XMLHttp, should compensate for that on the back end
    const url='/6';
    Http.open("POST", url);
    Http.send();

    Http.onreadystatechange = (event) => {
        // console.log(event)
        if (Http.readyState !== XMLHttpRequest.DONE) {
            return
        }

        let response = Http.responseText
        // console.log(response)

        files = response.split('\n') // split
        files = files.filter(n => n) // get rid of empty line

        ls_callback(files)
    }
}

function translate_name(name, callback){
    const Http = new XMLHttpRequest(); // it sucks that it's an XMLHttp, should compensate for that on the back end
    const url = '/7' + name;
    Http.open("POST", url);
    Http.send();

    Http.onreadystatechange = (event) => {
        // console.log(event)
        if (Http.readyState !== XMLHttpRequest.DONE) {
            return
        }

        let response = Http.responseText
        console.log(response)

        callback(response)
    }
}
