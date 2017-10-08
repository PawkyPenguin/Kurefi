# Kurefi
Kurefi is a keydaemon written in C and Ruby. It has client-server architecture style. The C client is responsible for grabbing the keys and passing them to the Ruby server using a socket. The Ruby server handles events and corresponding actions according to a configuration file.

The goal is to allow for an architecture that can easily be extended to allow the server to react to events other than just keypresses, which hopefully makes the whole keydaemon very flexible and configurable.
In its current state, Kurefi is for the X window system only. However, due to the architecture it should hopefully be easy to add a Wayland client in the futre.
