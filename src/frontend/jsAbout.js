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
