document.getElementById("shareButton").addEventListener("click", function () {
    // Simulated link — replace this with the link your server generates
    const generatedLink = "https://sharepaste.example/p/abcde12345";

    document.getElementById("shareLink").value = generatedLink;
});

document.getElementById("newButton").addEventListener("click", function () {
    // Simulated link — replace this with the link your server generates
    const emptyText = "";

    document.getElementById("pasteBox").value = emptyText;
});