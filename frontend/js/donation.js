console.log("✅ donation.js loaded");

function showSection(id) {
  document.querySelectorAll(".donation-section").forEach(sec => sec.style.display = "none");
  document.getElementById(`section-${id}`).style.display = "block";
  if (id === "pending") loadPending();
  if (id === "fulfilled") loadFulfilled();
}

// 📝 Create Donation Request
document.getElementById("donationForm").addEventListener("submit", async function (e) {
  e.preventDefault();
  const data = {
    fullName: document.getElementById("fullName").value,
    bloodType: document.getElementById("bloodType").value,
    age: parseInt(document.getElementById("age").value),
    gender: document.getElementById("gender").value,
    contact: document.getElementById("contact").value,
    hospitalID: parseInt(document.getElementById("hospitalID").value),
    scheduledDate: document.getElementById("scheduledDate").value
  };

  try {
    const res = await fetch("http://localhost:8080/donation-request", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(data)
    });
    const result = await res.json();
    document.getElementById("msg-create").textContent = result.success ? "✅ Request created!" : "❌ Failed to create";
    if (result.success) e.target.reset();
  } catch (err) {
    document.getElementById("msg-create").textContent = "❌ Server error";
  }
});

// 📥 Load Pending
async function loadPending() {
  try {
    const res = await fetch("http://localhost:8080/donation-requests/pending");
    const data = await res.json();
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
  } catch (err) {
    console.error("❌ Failed to load pending requests", err);
  }
}

// ✅ Load Fulfilled
async function loadFulfilled() {
  try {
    const res = await fetch("http://localhost:8080/donation-requests/fulfilled");
    const data = await res.json();
    const tbody = document.getElementById("fulfilledTable");
    tbody.innerHTML = "";
    data.forEach(d => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <td>${d.id}</td><td>${d.fullName}</td><td>${d.bloodType}</td><td>${d.age}</td>
        <td>${d.gender}</td><td>${d.contact}</td><td>${d.hospitalName}</td><td>${d.scheduledDate}</td>`;
      tbody.appendChild(row);
    });
  } catch (err) {
    console.error("❌ Failed to load fulfilled requests", err);
  }
}

// 💉 Fulfill Request
async function fulfill(id, bloodType) {
  const quantity = prompt("Enter number of units to fulfill:", "1");
  if (!quantity || isNaN(quantity)) return alert("❌ Invalid quantity");

  try {
    const res = await fetch(`http://localhost:8080/donation-request/${id}/fulfill`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ quantity: parseInt(quantity) })
    });
    const result = await res.json();
    alert(result.success ? "✅ Request fulfilled!" : "❌ Fulfillment failed");
    loadPending();
  } catch (err) {
    alert("❌ Error fulfilling request");
  }
}
