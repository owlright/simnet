## About The Project
This project is based on OMNeT++'s routing example, which uses cTopology to implemented the routing module.

So what this project does is implement different congestion algorithms.

## Notice
- No packet loss
- Socket only supports Simplex Communication

## Why not use INET?
If we just need to test the congestion control algorithms, most INET codes are useless to us.

For example, We don't need ip or mac addresses. Things should be kept as simple as possible as simulations.

## Roadmap

- [x] Packet type: DATA and ACK
- [x] label packet ECN and cut the window into half
- [ ] AIMD algorithm should should split the bandwidth equally

