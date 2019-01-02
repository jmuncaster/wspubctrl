# Examples

## pubctrl_server

A simple server that publishes text and allows control over a control socket.

Run the server:
```
$ ./pubctrl_server
```

Run a sub client (can run multiple times):
```
$ ./sub_client localhost:5554/pub
```

Run a control client:
```
$ ./ctrl_client localhost:5554/ctrl
```
Type in text to change what is published.


## multipubctrl_server

This example shows adding several publish endpoints.

Run the server:
```
./multipubctrl_server /pub1 /pub2 /pub3
```

Run several clients in their own windows (can run multiple subscribers for one endpoint):
```
./sub_client localhost:5554/pub1
./sub_client localhost:5554/pub2
./sub_client localhost:5554/pub3
```

Run a control client:
```
$ ./ctrl_client localhost:5554/ctrl
```
Type in text commands in the form of `endpoint text` to change the text published to a particular endpoint.

