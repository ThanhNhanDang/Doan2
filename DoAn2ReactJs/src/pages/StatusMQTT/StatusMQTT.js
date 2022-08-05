import { useSubscription } from "mqtt-react-hooks";
import React, { Fragment, useEffect, useState } from "react";
import ReactSwitch from "../../components/ReactSwitch/ReactSwitch";
function StatusMQTT() {
  /*
   * Status list
   * - Offline
   * - Connected
   * - Reconnecting
   * - Closed
   * - Error: printed in console too
   */
  const { message } = useSubscription([
    "doan2/onOff/led/feedback",
    "doan2/status",
  ]);
  const [checks, setChecks] = useState([false, false, false, false]);
  const ledPins = [1, 2, 3, 4];

  useEffect(() => {
    if (message) {
      let json = JSON.parse(message.message);
      switch (message.topic) {
        case "doan2/onOff/led/feedback":
          setChecks([
            json.r1 ? true : false,
            json.r2 ? true : false,
            json.r3 ? true : false,
            json.r4 ? true : false,
          ]);
          break;
        case "doan2/status":
          setChecks([
            json.r1 ? true : false,
            json.r2 ? true : false,
            json.r3 ? true : false,
            json.r4 ? true : false,
          ]);
          break;
        default:
          break;
      }
    }
  }, [message]);

  return (
    <Fragment>
      <hr />
      {ledPins.map((value, index) => (
        <Fragment key={value}>
          <label htmlFor="">Button {value}</label>
          <ReactSwitch checked={checks[index]} value={value} />
          <br />
        </Fragment>
      ))}
      <hr />
    </Fragment>
  );
}

export default StatusMQTT;
