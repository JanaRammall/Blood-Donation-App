// File: js/admin.js

function loadSection(section) {
  // Fetch and display the corresponding section
  fetch(`sections/${section}.html`)
    .then(response => {
      if (!response.ok) {
        throw new Error(`Failed to load section: ${section}`);
      }
      return response.text();
    })
    .then(html => {
      const container = document.getElementById('dashboardContent');
      container.innerHTML = html;

      // Wait for DOM to be updated before loading script
      setTimeout(() => {
        const script = document.createElement('script');
        script.src = `js/${section}.js`;
        script.onload = () => console.log(`✅ ${section}.js loaded`);
        document.body.appendChild(script);
      }, 50); // slight delay ensures DOM elements exist
    })
    .catch(error => {
      console.error(error);
      document.getElementById('dashboardContent').innerHTML = `<p>Error loading section: ${section}</p>`;
    });
}


// Attach event listeners to navigation links
document.addEventListener('DOMContentLoaded', () => {
  document.querySelectorAll('nav a[data-section]').forEach(link => {
    link.addEventListener('click', event => {
      event.preventDefault();
      const section = link.getAttribute('data-section');
      loadSection(section);
    });
  });
});
