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
      document.getElementById('dashboardContent').innerHTML = html;
      // Optionally, load corresponding JS for the section
      const script = document.createElement('script');
      script.src = `js/${section}.js`;
      document.body.appendChild(script);
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
