console.log("✅ inventory.js loaded");

setTimeout(() => {
  const tableBody = document.querySelector('#bloodTable tbody');
  if (!tableBody) {
    console.error("❌ Could not find #bloodTable tbody. Is your HTML loaded?");
    return;
  }

  function loadInventory() {
    fetch("http://localhost:8080/blood")
      .then(res => res.json())
      .then(data => {
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
  }

  window.removeExpiredBlood = function () {
    if (!confirm("Are you sure you want to remove expired blood units?")) return;
    fetch("http://localhost:8080/blood/expired", { method: "DELETE" })
      .then(res => res.json())
      .then(r => {
        alert(r.success ? "✅ Expired units removed!" : "❌ Removal failed");
        loadInventory();
      })
      .catch(err => console.error("❌ Failed to remove expired blood:", err));
  };

  loadInventory();
}, 100);
