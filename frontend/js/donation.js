console.log("✅ donation.js loaded");

function showSection(id) {
  document.querySelectorAll('.donation-section').forEach(sec => sec.style.display = 'none');
  document.getElementById(`section-${id}`).style.display = 'block';
  if (id === 'pending') loadPending();
  if (id === 'fulfilled') loadFulfilled();
}

// Create request
document.getElementById("donationForm").addEventListener("submit", function (e) {
  e.preventDefault();
  const data = {
    fullName: document.getElementById("fullName").value,
    bloodType: document.getElementById("bloodType").value,
    age: parseInt(document.getElementById("age").value),
    gender: document.getElementById("gender").value,
    contact: document.getElementById("contact").value,
    hospitalID: parseInt(document.getElementById("hospitalID").value),
    scheduledDate: document.getElementById("scheduledDate").value,
  };

  fetch("http://localhost:8080/donation-request", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(data)
  })
    .then(res => res.json())
    .then(r => {
      document.getElementById("msg-create").textContent = r.success ? "✅ Created!" : "❌ Failed";
      if (r.success) e.target.reset();
    });
});

// Load pending
function loadPending() {
  fetch("http://localhost:8080/donation-requests/pending")
    .then(res => res.json())
    .then(data => {
      const tbody = document.getElementById("pendingTable");
      tbody.innerHTML = "";
      data.forEach(d => {
        const row = document.createElement("tr");
        row.innerHTML = `
          <td>${d.id}</td><td>${d.fullName}</td><td>${d.bloodType}</td><td>${d.age}</td>
          <td>${d.gender}</td><td>${d.contact}</td><td>${d.hospitalName}</td><td>${d.scheduledDate}</td>
          <td><button onclick="fulfill(${d.id}, '${d.bloodType}')">💉 Fulfill</button></td>`;
        tbody.appendChild(row);
      });
    });
}

// Load fulfilled
function loadFulfilled() {
  fetch("http://localhost:8080/donation-requests/fulfilled")
    .then(res => res.json())
    .then(data => {
      const tbody = document.getElementById("fulfilledTable");
      tbody.innerHTML = "";
      data.forEach(d => {
        const row = document.createElement("tr");
        row.innerHTML = `
          <td>${d.id}</td><td>${d.fullName}</td><td>${d.bloodType}</td><td>${d.age}</td>
          <td>${d.gender}</td><td>${d.contact}</td><td>${d.hospitalName}</td><td>${d.scheduledDate}</td>`;
        tbody.appendChild(row);
      });
    });
}

// Fulfill action
function fulfill(id, type) {
  const qty = prompt("Units to fulfill?", 1);
  if (!qty) return;

  fetch(`http://localhost:8080/donation-request/${id}/fulfill`, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ quantity: parseInt(qty) })
  })
    .then(res => res.json())
    .then(r => {
      alert(r.success ? "✅ Fulfilled!" : "❌ Fail");
      loadPending();
    });
}
