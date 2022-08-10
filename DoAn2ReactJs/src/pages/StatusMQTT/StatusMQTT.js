import { useMqttState } from "mqtt-react-hooks";
import React from "react";
import Typography from "@mui/material/Typography";
function StatusMQTT() {
  /*
   * Status list
   * - Offline
   * - Connected
   * - Reconnecting
   * - Closed
   * - Error: printed in console too
   */
  const { connectionStatus } = useMqttState();

  return (
    <Typography variant="h3" component="div" gutterBottom>
      {connectionStatus}
    </Typography>
  );
}

export default StatusMQTT;
