// js/hospital.js

document.getElementById("addHospitalForm").addEventListener("submit", function (e) {
    e.preventDefault();
  
    const name = document.getElementById("hospitalName").value;
    const location = document.getElementById("hospitalLocation").value;
  
    const newHospital = {
      name: name,
      location: location
    };
  
    // Simulated POST request (replace with actual API call)
    console.log("✅ New hospital to add:", newHospital);
  
    // For now, just add it to the UI directly
    const hospitalDiv = document.createElement("div");
    hospitalDiv.textContent = `${name} (${location})`;
    document.getElementById("hospitalList").appendChild(hospitalDiv);
  
    // Clear form
    e.target.reset();
  });
  