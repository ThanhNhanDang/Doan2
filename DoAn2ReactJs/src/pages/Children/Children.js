import React, { Fragment, useEffect, useState } from "react";
import { useSubscription } from "mqtt-react-hooks";

function Children() {
  const { message, connectionStatus } = useSubscription("doan2/status");
  const [t, setMessage] = useState({});

  useEffect(() => {
    if (message) setMessage(JSON.parse(message.message));
  }, [message]);

  return (
    <Fragment>
      <span>{connectionStatus}</span>
      <hr />
      <span>{t.t}</span>
      <br />
      <span>{t.h} %</span>
    </Fragment>
  );
}

export default Children;
