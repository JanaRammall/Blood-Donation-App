function loadSection(section) {
    // Fetch and display the corresponding section
    fetch(`sections/${section}.html`)
      .then(response => response.text())
      .then(html => {
        document.getElementById('content').innerHTML = html;
        // Optionally, load corresponding JS for the section
        const script = document.createElement('script');
        script.src = `js/${section}.js`;
        document.body.appendChild(script);
      });
  }
  