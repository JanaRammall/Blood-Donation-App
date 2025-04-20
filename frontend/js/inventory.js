console.log("✅ inventory.js loaded");

setTimeout(() => {
  const tableBody = document.querySelector('#bloodTable tbody');

  if (!tableBody) {
    console.error("❌ Could not find #bloodTable tbody. Is your HTML loaded?");
    return;
  }

  fetch("http://localhost:8080/blood")
    .then(res => res.json())
    .then(data => {
      console.log("✅ Data fetched from backend:", data);

      // Clear old rows if any
      tableBody.innerHTML = "";

      data.forEach(item => {
        const row = document.createElement("tr");
        row.innerHTML = `
          <td>${item.bloodType}</td>
          <td>${item.units}</td>
          <td>${item.updated}</td>
        `;
        tableBody.appendChild(row);
      });
    })
    .catch(err => console.error("❌ Error fetching blood data:", err));
}, 100);  // Delay to ensure DOM is updated before script runs
