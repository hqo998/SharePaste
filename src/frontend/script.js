document.getElementById("shareButton").addEventListener("click", function ()
{
    const pasteRequest = document.getElementById("pasteBox").value;
    const origin = window.location.protocol + '//' + window.location.hostname + (window.location.port ? ':' + window.location.port : '');
    fetch("/api/new",
        {
        method: "POST",
        headers: 
        {
            "Content-type": "application/json; charset=UTF-8"
        },
        body: JSON.stringify({ pasteBody: pasteRequest })
        })

    .then((response) => response.text())
    
    .then((code) => {document.getElementById("shareLink").value = origin + '/' + code;
        
    })
});

document.getElementById("newButton").addEventListener("click", function ()
{
    const emptyText = "";
    document.getElementById("pasteBox").value = emptyText;
    document.getElementById("shareLink").value = emptyText;
    // document.location.href = '/';
    document.getElementById("viewCount").value = "👁 0";
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


document.addEventListener('DOMContentLoaded', () => {
  const uniqueCode = window.location.pathname.slice(1);

  if (!uniqueCode) return; // exit if no code

  fetch(`/api/find?code=${encodeURIComponent(uniqueCode)}`)
    .then(res => {
      if (!res.ok) throw new Error('Paste not found');
      return res.json();
    })
    .then(data => {
      document.getElementById('pasteBox').textContent = data.pasteBody;
      document.getElementById("shareLink").value = document.URL;

      document.getElementById("viewCount").textContent = "👁 " + data.viewCount;
    })
    .catch(err => {
      document.getElementById('pasteBox').textContent = '';
      console.error(err);
    });
});