// js/assign_role.js

document.getElementById("assignRoleForm").addEventListener("submit", function (e) {
    e.preventDefault();
  
    const username = document.getElementById("targetUsername").value;
    const role = document.getElementById("targetRole").value;
  
    fetch("http://localhost:8080/assign_role", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ username, role })
    })
      .then(res => res.json())
      .then(data => {
        const msg = document.getElementById("roleAssignMsg");
        if (data.success) {
          msg.textContent = "✅ Role updated successfully.";
          msg.style.color = "green";
          e.target.reset();
        } else {
          msg.textContent = "❌ Failed to update role.";
          msg.style.color = "red";
        }
      })
      .catch(() => {
        document.getElementById("roleAssignMsg").textContent = "❌ Server error.";
      });
  });
// js/assign_role.js

document.getElementById("assignRoleForm").addEventListener("submit", function (e) {
    e.preventDefault();
  
    const username = document.getElementById("targetUsername").value;
    const role = document.getElementById("targetRole").value;
  
    fetch("http://localhost:8080/assign_role", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ username, role })
    })
      .then(res => res.json())
      .then(data => {
        const msg = document.getElementById("roleAssignMsg");
        if (data.success) {
          msg.textContent = "✅ Role updated successfully.";
          msg.style.color = "green";
          e.target.reset();
        } else {
          msg.textContent = "❌ Failed to update role.";
          msg.style.color = "red";
        }
      })
      .catch(() => {
        document.getElementById("roleAssignMsg").textContent = "❌ Server error.";
      });
  });
    