import { useState } from "react";
import "./Keypad.css";

export default function Keypad() {
  const [inputValue, setInputValue] = useState("");

  const handleNumberClick = (num: string) => {
    if (inputValue.length < 4) {
      // Limit to 4 digits
      setInputValue((prev) => prev + num);
    }
  };

  const handleSubmit = async () => {
    try {
      const response = await fetch("/api/set-timer", {
        method: "POST",
        headers: {
          "Content-Type": "application/x-www-form-urlencoded",
        },
        body: `value=${inputValue}`,
      });

      if (!response.ok) throw new Error("Failed to send");
      setInputValue("");
    } catch (error) {
      console.error("Error:", error);
    }
  };

  return (
    <div className="keypad">
      <div className="input-display">{inputValue || "0"}</div>
      <div className="number-grid">
        {[1, 2, 3, 4, 5, 6, 7, 8, 9, 0].map((num) => (
          <button
            key={num}
            onClick={() => handleNumberClick(num.toString())}
            disabled={num === 0 && inputValue === ""}
          >
            {num}
          </button>
        ))}
      </div>
      <div className="control-buttons">
        <button onClick={handleSubmit}>Set Timer</button>
        <button onClick={() => setInputValue("")}>Clear</button>
      </div>
    </div>
  );
}
