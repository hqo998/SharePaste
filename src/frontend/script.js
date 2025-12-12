document.getElementById("shareButton").addEventListener("click", function ()
{
    const pasteRequest = document.getElementById("pasteBox").value;

    fetch("/api",
        {
        method: "POST",
        headers: 
        {
            "Content-type": "application/json; charset=UTF-8"
        },
        body: JSON.stringify({ pasteBody: pasteRequest })
        })

    .then((response) => response.text())

    .then((link) => {document.getElementById("shareLink").value = link;})
});

document.getElementById("newButton").addEventListener("click", function ()
{

    const emptyText = "";
    document.getElementById("pasteBox").value = emptyText;
});


document.getElementById("shareLink").onclick = function()
{
    if (this.value == "")
    {
        return
    }

    this.disabled = true;

    const timeDelay = 2000;

    this.select();
    document.execCommand('copy');
    
    const old = this.value;
    this.value = "Copied!";
    this.classList.add("shareConfirmation");
    setTimeout(() => {
        this.value = old;
        this.classList.remove("shareConfirmation");
        // this.select();
        this.disabled = false;
     }, timeDelay);
}
