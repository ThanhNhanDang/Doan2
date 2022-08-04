import { useMqttState } from "mqtt-react-hooks";
import React, { Fragment, useState } from "react";
import Switch from "react-switch";
function StatusMQTT() {
  /*
   * Status list
   * - Offline
   * - Connected
   * - Reconnecting
   * - Closed
   * - Error: printed in console too
   */
  const [checked, setChecked] = useState(false);
  const { client } = useMqttState();
  function handleClick(message) {
    return client.publish("doan2/status/led", message);
  }

  function handleChange(checked) {
    checked ? handleClick('{"onOff":1}') : handleClick('{"onOff":0}');
    setChecked(checked);
  }
  return (
    <Fragment>
      <Switch onChange={handleChange} checked={checked} />
    </Fragment>
  );
}

export default StatusMQTT;
