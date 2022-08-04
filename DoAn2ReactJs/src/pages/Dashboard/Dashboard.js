import React, { Fragment } from "react";
import Nav from "../../components/Nav/Nav";
import Children from "../Children/Children";
import StatusMQTT from "../StatusMQTT/StatusMQTT";

function Dashboard() {
  return (
    <Fragment>
      <Nav />
      <StatusMQTT />
      <Children />
    </Fragment>
  );
}

export default Dashboard;
