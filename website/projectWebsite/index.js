function downloadFile(pathToFile, downloadName){
    let a = document.createElement("a")
    a.href = pathToFile
    a.download = downloadName;
    document.body.appendChild(a)
    a.click()
    document.body.removeChild(a)
}
document.getElementById("playButton").addEventListener("click",(event)=>{
    // change to game init type
    console.log("event")
    document.getElementById("welcomeScreen").style.display = "none"
    document.getElementById("gameSelector").style.display = "flex"

})

document.getElementById("returnToHomePage").addEventListener("click",(event)=>{
    // change to welcome screen
    console.log("event")
    document.getElementById("welcomeScreen").style.display = "flex"
    document.getElementById("gameSelector").style.display = "none"

})

document.getElementById("linuxPlayButton").addEventListener("click",(event)=>{
    downloadFile("files/minecraftClone-linux.zip", "minecraftGame-linux.zip")
})
document.getElementById("windowsPlayButton").addEventListener("click",(event)=>{
    downloadFile("files/minecraftClone-win.zip", "minecraftGame-win.zip")
})
document.getElementById("webPlayButton").addEventListener("click",(event)=>{
    // redirect
})
document.getElementById("macPlayButton").addEventListener("click",(event)=>{
    // N/ A
})