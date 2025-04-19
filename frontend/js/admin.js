// File: js/Admin.js

function loadSection(section) {
  // Fetch and display the corresponding section
  fetch(`sections/${section}.html`)
    .then(response => response.text())
    .then(html => {
      document.getElementById('content').innerHTML = html;

      // Dynamically load the corresponding JS for the section
      const script = document.createElement('script');
      script.src = `js/${section}.js`;
      script.onload = () => {
        console.log(`${section}.js has been loaded successfully.`);
        // You can initialize functions from the loaded script here if needed
      };
      script.onerror = () => {
        console.error(`Failed to load ${section}.js`);
      };
      document.body.appendChild(script);
    })
    .catch(error => {
      console.error(`Error loading section ${section}:`, error);
    });
}
