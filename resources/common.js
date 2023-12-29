
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

async function translate_name(name, callback){

    const response = await fetch(
        '/7' + name,
        {method: "POST"},
    )

    const translated_name = await response.text()
    // console.log(translated_name)

    callback(translated_name)
}
