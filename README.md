## About The Project

This project is based on OMNeT++'s routing example, using cTopology to calc the shortest paths.

This project aims to implement different congestion algorithms.

## Notice

- No packet loss
- No handling of package disorder
- Socket only supports Simplex Communication

## Why not use INET?

If we just need to test the congestion control algorithms, most INET codes are useless to us.

For example, We don't need ip or mac addresses. Things should be kept as simple as possible as simulations.

## Roadmap

- [x] Packet type: DATA and ACK
- [x] label packet ECN and cut the window into half
- [x] AIMD algorithm should should split the bandwidth equally
- [x] Aggregate packets one level forward and backward
- [ ] Aggregate packets for more levels
- [ ] Add buffer module
- [ ] Simulate buffer overflow
