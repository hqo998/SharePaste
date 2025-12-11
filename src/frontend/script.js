document.getElementById("shareButton").addEventListener("click", function () {
    const pasteRequest = document.getElementById("pasteBox").value;

    fetch("/api", {
        method: "POST",
        headers: {
            "Content-type": "application/json; charset=UTF-8"
        },
        body: JSON.stringify({ pasteBody: pasteRequest })
        })

    .then((response) => response.text())

    .then((link) => {document.getElementById("shareLink").value = link;})
    
});

document.getElementById("newButton").addEventListener("click", function () {

    const emptyText = "";
    document.getElementById("pasteBox").value = emptyText;
});
