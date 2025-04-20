console.log("✅ log.js loaded");
loadLog() ;
document.addEventListener("DOMContentLoaded", loadLog);

function loadLog() {
  fetch("http://localhost:8080/logs")
    .then(res => res.json())
    .then(data => {
      const table = document.getElementById("logTable");
      table.innerHTML = "";

      data.forEach(entry => {
        const row = document.createElement("tr");
        row.innerHTML = `
          <td>${entry.timestamp}</td>
          <td>${entry.username}</td>
          <td>${entry.activity}</td>
        `;
        table.appendChild(row);
      });
    })
    .catch(err => console.error("❌ Failed to load log", err));
}
