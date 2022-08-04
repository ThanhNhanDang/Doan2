import { useMqttState } from "mqtt-react-hooks";
import React, { Fragment } from "react";

function StatusMQTT() {
  /*
   * Status list
   * - Offline
   * - Connected
   * - Reconnecting
   * - Closed
   * - Error: printed in console too
   */
  const { client } = useMqttState();
  function handleClick(message) {
    return client.publish("doan2/status/led", message);
  }

  return (
    <Fragment>
      <button type="button" onClick={() => handleClick("{\"onOff\":0}")}>
        Disable led
      </button>
      <button type="button" onClick={() => handleClick("{\"onOff\":1}")}>
        On led
      </button>
    </Fragment>
  );
}

export default StatusMQTT;
