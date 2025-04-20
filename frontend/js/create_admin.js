// js/create_admin.js

document.getElementById("adminRegisterForm").addEventListener("submit", function (e) {
    e.preventDefault();
  
    const username = document.getElementById("newUsername").value;
    const password = document.getElementById("newPassword").value;
  
    fetch("http://localhost:8080/create_admin", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ username, password })
    })
      .then(res => res.json())
      .then(data => {
        const msg = document.getElementById("adminCreateMsg");
        if (data.success) {
          msg.textContent = "✅ Admin created successfully.";
          msg.style.color = "green";
          e.target.reset();
        } else {
          msg.textContent = "❌ Failed to create admin.";
          msg.style.color = "red";
        }
      })
      .catch(() => {
        document.getElementById("adminCreateMsg").textContent = "❌ Server error.";
      });
  });
  