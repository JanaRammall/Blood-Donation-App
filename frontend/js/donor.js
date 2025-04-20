console.log("✅ donor.js loaded");
loadDonors();

let donorForm  = document.getElementById("donorForm");
const table = document.getElementById("donorTable");

async function loadDonors(type = "") {
  const url = type
    ? `http://localhost:8080/donors/type/${type}`
    : `http://localhost:8080/donors`;

  try {
    const res = await fetch(url);
    const data = await res.json();
    table.innerHTML = "";

    data.forEach(d => {
      const row = document.createElement("tr");
      row.innerHTML = `
        <td>${d.id}</td>
        <td>${d.username}</td>
        <td>${d.name}</td>
        <td>${d.age}</td>
        <td>${d.gender}</td>
        <td>${d.bloodType}</td>
        <td>${d.contact}</td>
        <td>${d.createdAt}</td>
        <td>
          <button onclick="editDonor(${d.id}, '${d.name}', ${d.age}, '${d.gender}', '${d.bloodType}', '${d.contact}')">✏️</button>
          <button onclick="deleteDonor(${d.id})">🗑️</button>
        </td>`;
      table.appendChild(row);
    });
  } catch (err) {
    console.error("❌ Failed to load donors", err);
  }
}

donorForm.addEventListener("submit", async (e) => {
  e.preventDefault();

  const data = {
    username: document.getElementById("username").value,
    name: document.getElementById("name").value,
    age: parseInt(document.getElementById("age").value),
    gender: document.getElementById("gender").value,
    bloodType: document.getElementById("bloodType").value,
    contact: document.getElementById("contact").value,
  };

  try {
    const res = await fetch("http://localhost:8080/donor", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(data)
    });

    const result = await res.json();
    document.getElementById("msg").textContent = result.success
      ? "✅ Donor registered successfully."
      : "❌ Registration failed.";

    loadDonors(); // Refresh the list
    donorForm.reset();
  } catch (err) {
    document.getElementById("msg").textContent = "❌ Server error.";
  }
});

window.deleteDonor = async function (id) {
  if (!confirm("Delete donor?")) return;

  const res = await fetch(`http://localhost:8080/donor/${id}`, { method: "DELETE" });
  const result = await res.json();
  alert(result.success ? "🗑️ Deleted!" : "❌ Failed!");
  loadDonors();
};

window.editDonor = async function (id, name, age, gender, bloodType, contact) {
  const newName = prompt("Edit name:", name);
  const newAge = prompt("Edit age:", age);
  const newGender = prompt("Edit gender:", gender);
  const newBlood = prompt("Edit blood type:", bloodType);
  const newContact = prompt("Edit contact:", contact);

  const res = await fetch(`http://localhost:8080/donor/${id}`, {
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      name: newName,
      age: parseInt(newAge),
      gender: newGender,
      bloodType: newBlood,
      contact: newContact,
    }),
  });

  const result = await res.json();
  alert(result.success ? "✅ Updated!" : "❌ Update failed");
  loadDonors();
};

window.filterByBloodType = () => {
  const type = document.getElementById("filterType").value;
  if (type) loadDonors(type);
};



loadDonors();
