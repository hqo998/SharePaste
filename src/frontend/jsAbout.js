document.addEventListener('DOMContentLoaded', () => {
  const contactEmail = sessionStorage.getItem("ContactEmail");

  // console.log(contactEmail);
  if (contactEmail != null && contactEmail != "") {
    document.getElementById('ContactEmail').textContent = contactEmail;
    console.log("Found contact email stored.")
    return;
  }

    fetch(`/api/email`)
    .then(res => {
      if (!res.ok) throw new Error('Email not found');
      return res.text();
    })
    .then(data => {
      document.getElementById('ContactEmail').textContent = data;
      sessionStorage.setItem("ContactEmail", data);

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