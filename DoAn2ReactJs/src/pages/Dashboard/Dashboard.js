import { useSubscription } from "mqtt-react-hooks";
import React, { Fragment, useEffect, useState } from "react";
import ReactSwitch from "../../components/ReactSwitch/ReactSwitch";
import StatusMQTT from "../StatusMQTT/StatusMQTT";

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
      <StatusMQTT />

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
