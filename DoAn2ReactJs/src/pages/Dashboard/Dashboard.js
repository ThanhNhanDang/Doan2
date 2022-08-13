import { useSubscription } from "mqtt-react-hooks";
import React, { Fragment, useEffect, useState } from "react";
import ReactSwitch from "../../components/ReactSwitch/ReactSwitch";
import { styled, createTheme } from "@mui/material/styles";
import Paper from "@mui/material/Paper";
import Grid from "@mui/material/Unstable_Grid2";
import { Box } from "@mui/system";
import { ThemeProvider } from "@emotion/react";

function Dashboard() {
  /*
   * idtp = 1: doan2/status
   * idtp = 2: doan2/onOff/led
   * idtp = 3: doan2/onOff/door
   */
  const { message } = useSubscription(["doan2/onOff/feedback", "doan2/status"]);
  const [checks, setChecks] = useState([false, false, false, false, false]);
  const [checkAll, setCheckAll] = useState(false);

  const ledPins = [1, 2, 3, 4, 5];

  const Item = styled(Paper)(({ theme }) => ({
    textAlign: "center",
    color: theme.palette.text.secondary,
    height: 60,
    lineHeight: "60px",
  }));
  const lightTheme = createTheme({ palette: { mode: "light" } });

  useEffect(() => {
    if (message) {
      let json = JSON.parse(message.message);
      setChecks([
        json.l1 ? true : false,
        json.l2 ? true : false,
        json.l3 ? true : false,
        json.l4 ? true : false,
        json.d ? true : false,
      ]);
      setCheckAll(json.al ? true : false);
    }
  }, [message]);

  return (
    <Fragment>
      <Grid container spacing={2}>
        <Grid xs={12} sx={{ paddingBottom: 4 }}>
          <Item></Item>
        </Grid>

        <Grid item xs={4}>
          <ThemeProvider theme={lightTheme}>
            <Box
              sx={{
                p: 2,
                bgcolor: "background.default",
                display: "grid",
                gridTemplateColumns: { md: "1fr 1fr" },
                gap: 2,
              }}
            >
              {ledPins.map((value, index) => (
                <Item key={value} elevation={8}>
                  <ReactSwitch checked={checks[index]} value={value} />
                  <br />
                </Item>
              ))}
            </Box>
          </ThemeProvider>
        </Grid>
        <Grid xs={6}>
          <Item>xs=6</Item>
        </Grid>
        <Grid xs={4}>
          <Item>xs=4</Item>
        </Grid>
        <Grid xs={4}>
          <Item>xs=4</Item>
        </Grid>
        <Grid xs={4}>
          <Item>xs=4</Item>
        </Grid>
      </Grid>

      <hr />
      {ledPins.map((value, index) => (
        <Fragment key={value}>
          <label htmlFor="">Button {value}</label>
          <ReactSwitch checked={checks[index]} value={value} />
          <br />
        </Fragment>
      ))}
      <hr />
      <label htmlFor="">Turn all led</label>
      <ReactSwitch checked={checkAll} idtp={4} />
      <hr />
    </Fragment>
  );
}

export default Dashboard;
