// File: frontend/js/admin.js

document.addEventListener("DOMContentLoaded", () => {
  const links = document.querySelectorAll("a[data-section]");
  const contentDiv = document.getElementById("content");

  links.forEach(link => {
      link.addEventListener("click", (e) => {
          e.preventDefault();
          const section = link.getAttribute("data-section");
          loadSection(section);
      });
  });

  function loadSection(section) {
      fetch(`sections/${section}.html`)
          .then(response => {
              if (!response.ok) throw new Error("Failed to load section.");
              return response.text();
          })
          .then(html => {
              contentDiv.innerHTML = html;

              // Optional: load JS if needed per section (once)
              const existingScript = document.querySelector(`script[src="js/${section}.js"]`);
              if (!existingScript) {
                  const script = document.createElement('script');
                  script.src = `js/${section}.js`;
                  script.defer = true;
                  document.body.appendChild(script);
              }
          })
          .catch(err => {
              contentDiv.innerHTML = `<p style="color: red;">Error loading section: ${err.message}</p>`;
          });
  }
});
