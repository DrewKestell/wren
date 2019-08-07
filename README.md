# Wren

A multiplayer game engine written in C++ / DirectX 11.

## Game Loop

WrenClient:

1) Process UDP Packets
	a) if any PlayerCorrection events are received, correct player's state
2) if 0.016~ seconds have elapsed, update simulation
    a) playerController.Update()
        -if player is moving, check if destination is reached. if so, set player's loc to destination
        -if player not moving and right click is held, start moving and set destination
    b) camera.update()
    c) playerController.GeneratePlayerUpdate()
        -generate player update based on current state
        -increment update counter
        -send PlayerUpdate packet to server
    d) objectManager.Update()
        -move player according to their current movementVector
    e) publish events
        -first sort uiComponents by z-index
        -pass event to each uiComponent from back to front (higher z-index elements will get events first)
        -if no uiComponents stopped event propagation, pass event to all gameObjects
3) Render


WrenServer:

1) Process UDP Packets
	a) if any PlayerUpdate events are received, verify that state is the same between client and server. if not, sent PlayerCorrection event
2) if 0.016~ seconds have elapsed, update simulation
	a) objectManager.Update()
        -move player according to their current movementVector
    b) publish events
        -first sort uiComponents by z-index
        -pass event to each uiComponent from back to front (higher z-index elements will get events first)
        -if no uiComponents stopped event propagation, pass event to all gameObjects