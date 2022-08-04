const aedes = require("aedes")({
  authenticate: (client, username, password, callback) => {
    console.log("username:" + username);
    callback(
      null,
      username == process.env.MQTT_USERNAME &&
        password == process.env.MQTT_PASSWORD
    );
  },
});
const server = require("net").createServer(aedes.handle);
const httpServer = require("http").createServer();
const port = 1883;
server.listen(port, function () {
  console.log("Ades MQTT listening on port: " + port);
  console.log("Username: " + process.env.MQTT_USERNAME);
});
//new
const ws = require("websocket-stream");
ws.createServer({ server: httpServer }, aedes.handle);

aedes.on("client", function (client) {
  console.log("new client", client.id);
});
aedes.on("publish", function (packet, client) {
  if (client) {
    console.log(
      "message from client",
      client.id,
      packet.topic,
      packet.payload.toString()
    );
  }
});

aedes.on("subscribe", function (subscriptions, client) {
  if (client) {
    console.log("subscribe from client", subscriptions, client.id);
  }
});
