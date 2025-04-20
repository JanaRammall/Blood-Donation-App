// File: hospital.js

    // 👇 Wait a bit to ensure section is fully injected
    setTimeout(() => {
      const form = document.getElementById("addHospitalForm");
      const table = document.querySelector("#hospitalTable tbody");
  
      if (!form || !table) {
        console.warn("🚫 hospital.js: Form or table not found — aborting init.");
        return;
      }
  
      console.log("✅ hospital.js initialized");
  
      // Escape helper
      function escapeString(str) {
        return str.replace(/\\/g, "\\\\")
                  .replace(/'/g, "\\'")
                  .replace(/"/g, '\\"');
      }
  
      // Submit form
      form.addEventListener("submit", function (e) {
        e.preventDefault();
  
        const name = document.getElementById("hospitalName").value.trim();
        const location = document.getElementById("hospitalLocation").value.trim();
        const contact = document.getElementById("hospitalContact").value.trim();
  
        console.log("🆕 Adding:", { name, location, contact });
  
        if (!name || !location || !contact) {
          alert("❗ Please fill in all fields.");
          return;
        }
  
        fetch("http://localhost:8080/hospital", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ name, location, contact })
        })
          .then(res => res.json())
          .then(data => {
            if (data.success) {
              alert("✅ Hospital added.");
              form.reset();
              loadHospitals();
            } else {
              alert("❌ Failed to add hospital.");
            }
          })
          .catch(err => console.error("❌ POST error:", err));
      });
  
      // Load hospitals
      function loadHospitals() {
        fetch("http://localhost:8080/hospitals")
          .then(res => res.json())
          .then(data => {
            table.innerHTML = "";
  
            data.forEach(h => {
              const row = document.createElement("tr");
              row.innerHTML = `
                <td>${h.id}</td>
                <td>${h.name}</td>
                <td>${h.location}</td>
                <td>${h.contact || "N/A"}</td>
                <td>${h.createdAt || "N/A"}</td>
                <td>
                  <button onclick="editHospital(${h.id}, '${escapeString(h.name)}', '${escapeString(h.location)}', '${escapeString(h.contact)}')">✏️</button>
                  <button onclick="deleteHospital(${h.id})">🗑️</button>
                </td>`;
              table.appendChild(row);
            });
          })
          .catch(err => console.error("❌ GET error:", err));
      }
  
      // Delete
      window.deleteHospital = function (id) {
        if (!confirm("Delete hospital?")) return;
  
        fetch(`http://localhost:8080/hospital/${id}`, {
          method: "DELETE"
        })
          .then(res => res.json())
          .then(data => {
            if (data.success) {
              alert("✅ Deleted.");
              loadHospitals();
            } else {
              alert("❌ Failed to delete hospital.");
            }
          })
          .catch(err => console.error("❌ DELETE error:", err));
      };
  
      // Edit
      window.editHospital = function (id, currentName, currentLocation, currentContact) {
        const name = prompt("New name:", currentName);
        if (!name) return;
        const location = prompt("New location:", currentLocation);
        if (!location) return;
        const contact = prompt("New contact:", currentContact);
        if (!contact) return;
  
        fetch(`http://localhost:8080/hospital/${id}`, {
          method: "PUT",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ name, location, contact })
        })
          .then(res => res.json())
          .then(data => {
            if (data.success) {
              alert("✅ Updated.");
              loadHospitals();
            } else {
              alert("❌ Update failed.");
            }
          })
          .catch(err => console.error("❌ PUT error:", err));
      };
  
      // Initial fetch
      loadHospitals();
    }, 100); // <- Delay to wait for HTML load

  