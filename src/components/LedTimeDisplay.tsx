import { useState, useEffect } from "react";

export default function LedTimeDisplay() {
  const [totalTime, setTotalTime] = useState(0);
  const [timerRemaining, setTimerRemaining] = useState(0);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const totalResponse = await fetch("/api/total-ontime");
        if (!totalResponse.ok) {
          throw new Error(`HTTP error! status: ${totalResponse.status}`);
        }
        const totalContentType = totalResponse.headers.get("content-type");
        if (totalContentType && totalContentType.includes("application/json")) {
          const data = await totalResponse.json();
          setTotalTime(data.totalOnTime / 1000);
        }

        const timerResponse = await fetch("/api/timer-remaining");
        if (!timerResponse.ok) {
          throw new Error(`HTTP error! status: ${timerResponse.status}`);
        }
        const timerContentType = timerResponse.headers.get("content-type");
        if (timerContentType && timerContentType.includes("application/json")) {
          const data = await timerResponse.json();
          setTimerRemaining(data.timeremaining / 1000);
        }
      } catch (error) {
        console.error("Error fetching data:", error);
      }
    };

    const interval = setInterval(fetchData, 1000);
    fetchData();

    return () => clearInterval(interval);
  }, []);

  return (
    <div className="container">
      <div className="stats">
        <tr>
          {/* Total Active Time */}
          <th>
            <div
              className="stat-card"
              style={{
                display: "flex",
                flexDirection: "column",
                justifyContent: "center",
                alignItems: "center",
                height: "150px",
                width: "300px", // Optional: Adjust the width
                margin: "10px", // Optional: Adds space between the cards
              }}
            >
              <div
                className="stat-card__header"
                style={{ textAlign: "center" }}
              >
                <svg
                  viewBox="0 0 24 24"
                  width="20"
                  height="20"
                  stroke="currentColor"
                  fill="none"
                  strokeWidth="2"
                  strokeLinecap="round"
                  strokeLinejoin="round"
                >
                  <path d="M22 12h-4l-3 9L9 3l-3 9H2" />
                </svg>
                <h3 className="stat-card__title">Total Active Time</h3>
              </div>
              <p className="stat-card__value">{totalTime.toFixed(2)}s</p>
            </div>
          </th>
          <th>
            {/* Workout Remaining */}
            <div
              className="stat-card"
              style={{
                display: "flex",
                flexDirection: "column",
                justifyContent: "center",
                alignItems: "center",
                height: "150px",
                width: "300px", // Optional: Adjust the width
                margin: "10px", // Optional: Adds space between the cards
              }}
            >
              <div
                className="stat-card__header"
                style={{ textAlign: "center" }}
              >
                <svg
                  viewBox="0 0 24 24"
                  width="20"
                  height="20"
                  stroke="currentColor"
                  fill="none"
                  strokeWidth="2"
                  strokeLinecap="round"
                  strokeLinejoin="round"
                >
                  <circle cx="12" cy="12" r="10" />
                  <polyline points="12 6 12 12 16 14" />
                </svg>
                <h3 className="stat-card__title">Workout Remaining</h3>
              </div>
              <p className="stat-card__value">{timerRemaining.toFixed(2)}s</p>
            </div>
          </th>
        </tr>
      </div>
    </div>
  );
}
