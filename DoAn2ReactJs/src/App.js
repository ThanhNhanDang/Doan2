import { Connector } from "mqtt-react-hooks";
import Dashboard from "./pages/Dashboard/Dashboard";
import { Routes, Route } from "react-router-dom";
import ButtonAppBar from "./components/ButtonAppBar/ButtonAppBar";

function App() {
  const options = {
    hostname: "116.118.48.230",
    port: 8883,
    username: "doan2",
    password: "doan2",
  };
  return (
    <Connector options={options} parserMethod={(msg) => msg}>
      <ButtonAppBar />
      <Routes>
        <Route path="/" element={<Dashboard />} />
      </Routes>
    </Connector>
  );
}

export default App;
