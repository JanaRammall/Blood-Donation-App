document.getElementById("donorForm").addEventListener("submit", async (e) => {
    e.preventDefault();
  
    const data = {
      username: document.getElementById("username").value,
      name: document.getElementById("name").value,
      bloodtype: document.getElementById("bloodtype").value,
      contact: document.getElementById("contact").value,
    };
  
    try {
      const res = await fetch("http://localhost:8080/donor", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data),
      });
  
      const result = await res.json();
      document.getElementById("msg").textContent = result.success
        ? "✅ Donor registered successfully."
        : "❌ Registration failed.";
    } catch (err) {
      document.getElementById("msg").textContent = "❌ Server error.";
    }
  });
  