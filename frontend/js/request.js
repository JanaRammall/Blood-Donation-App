// js/request.js

document.getElementById("requestForm").addEventListener("submit", function (e) {
    e.preventDefault();
  
    const request = {
      recipient: document.getElementById("recipientName").value,
      bloodType: document.getElementById("bloodType").value,
      units: document.getElementById("units").value,
      hospital: document.getElementById("hospital").value,
    };
  
    console.log("📋 New Request:", request);
  
    const reqDiv = document.createElement("div");
    reqDiv.textContent = `${request.units} units of ${request.bloodType} for ${request.recipient} at ${request.hospital}`;
    document.getElementById("requestList").appendChild(reqDiv);
  
    e.target.reset();
  });
  