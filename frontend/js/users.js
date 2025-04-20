function loadUsers() {
  fetch("http://localhost:8080/users")
    .then(res => res.json())
    .then(data => {
      const table = document.querySelector("#usersTable tbody");
      table.innerHTML = "";

      data.forEach(user => {
        const tr = document.createElement("tr");
        tr.innerHTML = `
          <td>${user.username}</td>
          <td>${user.role}</td>
          <td>${user.createdAt || "N/A"}</td>
          <td>
            <button class="danger-btn" onclick="deleteUser('${user.username}')">Delete</button>
          </td>
        `;
        table.appendChild(tr);
      });
    })
    .catch(err => console.error("❌ Error fetching users:", err));
}

function deleteUser(username) {
  if (!confirm(`Are you sure you want to delete user "${username}"?`)) return;

  fetch(`http://localhost:8080/user/${username}`, {
    method: "DELETE"
  })
    .then(res => res.json())
    .then(data => {
      if (data.success) {
        alert("✅ User deleted.");
        loadUsers();  // reload list
      } else {
        alert("❌ Failed to delete user.");
      }
    });
}

loadUsers();
