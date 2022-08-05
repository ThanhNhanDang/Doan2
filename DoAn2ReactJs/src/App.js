import { Connector } from "mqtt-react-hooks";
import Dashboard from "./pages/Dashboard/Dashboard";
import { BrowserRouter, Routes, Route } from "react-router-dom";
function App() {
  const options = {
    hostname: "116.118.48.230",
    port: 8883,
    username: "doan2",
    password: "doan2",
  };
  return (
    <Connector options={options} parserMethod={(msg) => msg}>
      <BrowserRouter>
        <Routes>
          <Route path="/" element={<Dashboard />} />
        </Routes>
      </BrowserRouter>
    </Connector>
  );
}

export default App;
