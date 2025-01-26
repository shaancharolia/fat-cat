import { useState, useEffect } from "react";

// Replace with your ESP32's IP (check Serial Monitor)
const ESP32_IP = "192.168.1.100";

export default function MotorStatus() {
  const [isMotorOn, setIsMotorOn] = useState(false);

  useEffect(() => {
    const interval = setInterval(() => {
      fetch(`http://${ESP32_IP}/motor-status`)
        .then((res) => res.json())
        .then((data) => setIsMotorOn(data.status))
        .catch((err) => console.error("Error fetching status:", err));
    }, 1000); // Poll every 1 second

    return () => clearInterval(interval);
  }, []);

  return <div>Motor Status: {isMotorOn ? "ON 🟢" : "OFF 🔴"}</div>;
}
