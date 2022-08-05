import { useMqttState } from "mqtt-react-hooks";
import React from "react";
import Switch from "react-switch";

function ReactSwitch({ checked, value }) {
  const { client } = useMqttState();

  function handleClick(message) {
    return client.publish("doan2/onOff/led", message);
  }

  function handleChange(checked) {
    checked
      ? handleClick(`{"onOff":1, "getState":0, "iddv":${value}}`)
      : handleClick(`{"onOff":0, "getState":0, "iddv":${value}}`);
  }
  return <Switch onChange={handleChange} checked={checked} />;
}

export default ReactSwitch;
