  console.log("✅ recipient.js loaded");

  let form = document.getElementById("addRecipientForm");
  const tableBody = document.getElementById("recipientTable");

  function escape(str) {
    return str.replace(/'/g, "\\'").replace(/"/g, '\\"');
  }

  function loadRecipients() {
    fetch("http://localhost:8080/recipients")
      .then(res => res.json())
      .then(data => {
        tableBody.innerHTML = "";
        data.forEach(r => {
          const row = document.createElement("tr");
          row.innerHTML = `
            <td>${r.id}</td>
            <td>${r.name}</td>
            <td>${r.bloodType}</td>
            <td>${r.urgency}</td>
            <td>${r.contact}</td>
            <td>${r.hospitalID}</td>
            <td>${r.fulfilled ? "✅ Fulfilled" : "⏳ Waiting"}</td>
            <td>
              <button onclick="editRecipient(${r.id}, '${escape(r.name)}', '${r.bloodType}', '${r.urgency}', '${r.contact}', ${r.hospitalID})">✏️</button>
              <button onclick="deleteRecipient(${r.id})">🗑️</button>
              <button onclick="fulfillRecipient(${r.id})">💉 Fulfill</button>
            </td>
          `;
          tableBody.appendChild(row);
        });
      });
  }

  form.addEventListener("submit", function (e) {
    e.preventDefault();
    const data = {
      name: form.name.value.trim(),
      bloodType: form.bloodType.value.trim(),
      urgency: form.urgency.value.trim(),
      contact: form.contact.value.trim(),
      hospitalID: parseInt(form.hospitalID.value)
    };

    fetch("http://localhost:8080/recipient", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(data)
    })
      .then(res => res.json())
      .then(resp => {
        alert(resp.success ? "✅ Added!" : "❌ Failed to add.");
        form.reset();
        loadRecipients();
      })
      .catch(err => console.error("❌ POST error:", err));
  });

  window.editRecipient = function (id, name, bloodType, urgency, contact, hospitalID) {
    const updated = {
      name: prompt("Name:", name),
      bloodType: prompt("Blood Type:", bloodType),
      urgency: prompt("Urgency:", urgency),
      contact: prompt("Contact:", contact),
      hospitalID: parseInt(prompt("Hospital ID:", hospitalID))
    };

    fetch(`http://localhost:8080/recipient/${id}`, {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(updated)
    })
      .then(res => res.json())
      .then(resp => {
        alert(resp.success ? "✅ Updated!" : "❌ Update failed.");
        loadRecipients();
      });
  };

  window.deleteRecipient = function (id) {
    if (confirm("Are you sure to delete this recipient?")) {
      fetch(`http://localhost:8080/recipient/${id}`, { method: "DELETE" })
        .then(res => res.json())
        .then(resp => {
          alert(resp.success ? "✅ Deleted!" : "❌ Failed to delete.");
          loadRecipients();
        });
    }
  };

  window.fulfillRecipient = function (id) {
    const quantity = prompt("How many blood units to use?");
    if (!quantity) return;

    fetch(`http://localhost:8080/recipient/${id}/fulfill`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ quantity: parseInt(quantity) })
    })
      .then(res => res.json())
      .then(resp => {
        alert("✅ Marked as fulfilled.");
        loadRecipients();
      });
  };

  loadRecipients();
