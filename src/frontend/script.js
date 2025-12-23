let G_LASTTEXT = "";

document.getElementById("shareButton").addEventListener("click", function ()
{
    const pasteRequest = document.getElementById("pasteBox").value;

    if (!pasteRequest) return; // early return for empty value

    if (pasteRequest == G_LASTTEXT && !document.getElementById("shareLink")?.value.startsWith("http")) return;
    else G_LASTTEXT = pasteRequest;

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

    .then(async (response) => {
      const text = await response.text();
      const status = response.status;
      return { text, status };
      })

    .then(({text, status}) => {
      if (status == 200) {
        document.getElementById("shareLink").value = origin + '/' + text;
      }
      else {
        document.getElementById("shareLink").value = text;
      }
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


document.getElementById("shareLink").onclick = async function()
{
    if (this.value == "") return;

    this.disabled = true;
    const timeDelay = 500;

    try {
        await navigator.clipboard.writeText(this.value);
        const old = this.value;
        this.value = "Copied!";
        this.classList.add("shareConfirmation");
        setTimeout(() => {
            this.value = old;
            this.classList.remove("shareConfirmation");
            this.disabled = false;
        }, timeDelay);
    } catch (err) {
        console.error("Clipboard copy failed", err);
        this.disabled = false;
    }
}


document.addEventListener('DOMContentLoaded', () => {
  const uniqueCode = window.location.pathname.slice(1);

  const storageData = sessionStorage.getItem(uniqueCode);

  if (!uniqueCode) return; // exit if no code

  if (storageData) { // use locally found data instead.
    console.log("Local Cache Found for Code");
    const pasteData = JSON.parse(storageData);
    document.getElementById("viewCount").textContent = "👁 " + pasteData.viewCount;
    document.getElementById('pasteBox').textContent = pasteData.pasteBody;
    document.getElementById("shareLink").value = document.URL;
    return;
  }

  fetch(`/api/find?code=${encodeURIComponent(uniqueCode)}`)
    .then(res => {
      if (!res.ok) throw new Error('Paste not found');
      return res.json();
    })
    .then(data => {
      document.getElementById('pasteBox').textContent = data.pasteBody;
      document.getElementById("shareLink").value = document.URL;
      document.getElementById("viewCount").textContent = "👁 " + data.viewCount;
      const localDataObj = {
        pasteBody: data.pasteBody,
        viewCount: data.viewCount
      }
      sessionStorage.setItem(uniqueCode, JSON.stringify(localDataObj));

    })
    .catch(err => {
      document.getElementById('pasteBox').textContent = '';
      console.error(err);
    });
});