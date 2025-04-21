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
          <td>
            <select onchange="editUserRole('${user.username}', this.value)">
              <option value="admin" ${user.role === 'admin' ? 'selected' : ''}>Admin</option>
              <option value="staff" ${user.role === 'staff' ? 'selected' : ''}>Staff</option>
              <option value="viewer" ${user.role === 'viewer' ? 'selected' : ''}>Viewer</option>
              <option value="audit" ${user.role === 'audit' ? 'selected' : ''}>Audit</option>
            </select>
          </td>
          <td>${user.createdAt || 'N/A'}</td>
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
        loadUsers();
      } else {
        alert("❌ Failed to delete user.");
      }
    });
}

function editUserRole(username, newRole) {
  fetch(`http://localhost:8080/user/${username}`, {
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ role: newRole })
  })
    .then(res => res.json())
    .then(data => {
      if (!data.success) alert("❌ Failed to update role.");
    });
}

document.getElementById("createUserForm").addEventListener("submit", function (e) {
  e.preventDefault();
  const username = document.getElementById("newUsername").value;
  const password = document.getElementById("newPassword").value;
  const role = document.getElementById("newRole").value;

  fetch("http://localhost:8080/user", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ username, password, role })
  })
    .then(res => res.json())
    .then(data => {
      document.getElementById("userCreateMsg").textContent = data.success
        ? "✅ User created!"
        : "❌ Failed to create user";
      if (data.success) {
        loadUsers();
        e.target.reset();
      }
    });
});

loadUsers();
