
async function ls(ls_callback){

    const response = await fetch(
        '/6',
        {method: "POST"},
    )

    let files = await response.text()
    files = files.split('\n')
    files = files.filter(n => n) // get rid of the empty line
    // console.log(files)

    ls_callback(files)
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
