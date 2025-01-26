import Keypad from "./components/Keypad";
import LedTimeDisplay from "./components/LedTimeDisplay";
import "./App.css";

function App() {
  return (
    <div className="bg-gray-900 text-white min-h-screen p-2 font-Courier-New">
      <h1 className="text-5xl font-bold mb-8 ">Fat Cat Controller</h1>
      <LedTimeDisplay />
      <Keypad />
    </div>
  );
}

export default App;
