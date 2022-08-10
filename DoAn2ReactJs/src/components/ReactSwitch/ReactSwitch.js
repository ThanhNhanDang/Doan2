import { useMqttState } from "mqtt-react-hooks";
import React from "react";
import Switch from "react-switch";

function ReactSwitch({ checked, value, idtp }) {
  const { client } = useMqttState();
  let json = { onOff: 1, iddv: 0, idtp: 2 };
  function handleClick(message) {
    if (json.idtp === 2) return client.publish("doan2/onOff/led", message);
    if (json.idtp === 3) return client.publish("doan2/onOff/door", message);
    if (json.idtp === 4) return client.publish("doan2/onOff/led/all", message);
  }

  function handleChange(checked) {
    if (idtp === undefined) value === 5 ? (json.idtp = 3) : (json.idtp = 2);
    else json.idtp = idtp;
    if (value === undefined) json.iddv = 0;
    else json.iddv = value;
    checked ? (json.onOff = 1) : (json.onOff = 0);
    handleClick(JSON.stringify(json));
  }

  return <Switch onChange={handleChange} checked={checked} />;
}

export default ReactSwitch;
