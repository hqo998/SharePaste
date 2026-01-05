document.addEventListener('DOMContentLoaded', () => {
    fetch(`/api/email`)
    .then(res => {
      if (!res.ok) throw new Error('Email not found');
      return res.text();
    })
    .then(data => {
      document.getElementById('ContactEmail').textContent = data;

    })
    .catch(err => {
      console.error(err);
    });
});

// footer close
document.getElementById("footerCloseButton").addEventListener("click", function ()
{
    document.getElementById("footerBar").classList.add("hidden");
    sessionStorage.setItem("isFooterClosed", "true");
});

// stay closed on reload
document.addEventListener('DOMContentLoaded', () => {
  const storageData = sessionStorage.getItem("isFooterClosed");
  if (storageData == "true") {
    document.getElementById("footerBar").classList.add("hidden");
    sessionStorage.setItem("isFooterClosed", "true");
  }
});