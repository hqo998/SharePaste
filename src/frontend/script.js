document.getElementById("shareButton").addEventListener("click", function () {
    const pasteRequest = document.getElementById("pasteBox").value;

    fetch("/api", {
        method: "POST",
        headers: {
            "Content-type": "application/json; charset=UTF-8"
        },
        body: JSON.stringify({ pasteBody: pasteRequest })
        })

    .then((response) => response.json())

    .post((json) => {document.getElementById("shareLink").value = json.link;})

    // placeholder text
    // const generatedLink = "https://sharepaste.example/p/abcde12345";
    // document.getElementById("shareLink").value = generatedLink;
});

document.getElementById("newButton").addEventListener("click", function () {
    const emptyText = "";
    document.getElementById("pasteBox").value = emptyText;
});
