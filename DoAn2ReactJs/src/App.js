// import { Connector } from "mqtt-react-hooks";
import { styled } from "@mui/material/styles";

import Dashboard from "./pages/Dashboard/Dashboard";
import { Routes, Route } from "react-router-dom";
import ButtonAppBar from "./components/ButtonAppBar/ButtonAppBar";
import { Box, Fab, Toolbar } from "@mui/material";
import KeyboardArrowUpIcon from "@mui/icons-material/KeyboardArrowUp";
import BackToTop from "./components/BackToTop/BackToTop";

function App(props) {
  // const options = {
  //   hostname: "116.118.48.230",
  //   port: 8883,
  //   username: "doan2",
  //   password: "doan2",
  // };
  const DrawerHeader = styled("div")(({ theme }) => ({
    display: "flex",
    alignItems: "center",
    justifyContent: "flex-end",
    padding: theme.spacing(0, 1),
    // necessary for content to be below app bar
    ...theme.mixins.toolbar,
  }));
  return (
    // <Connector options={options} parserMethod={(msg) => msg}>
    <Box sx={{ display: "flex" }}>
      <ButtonAppBar />
      <Toolbar id="back-to-top-anchor" />
      <Box component="main" sx={{ flexGrow: 1, pt: 3 }}>
        <DrawerHeader />

        <Routes>
          <Route path="/" element={<Dashboard />} />
        </Routes>
      </Box>
      <BackToTop {...props}>
        <Fab color="primary" size="small" aria-label="scroll back to top">
          <KeyboardArrowUpIcon />
        </Fab>
      </BackToTop>
      {/* </Connector> */}
    </Box>
  );
}

export default App;
