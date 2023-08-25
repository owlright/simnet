## About The Project

This project is based on OMNeT++'s routing example, using cTopology to calc the shortest paths.

This project aims to implement different congestion algorithms.

## Notice

- ~~No packet loss~~
- ~~No handling of package disorder~~
- ~~Socket only supports Simplex Communication~~

## Why not use NS3/INET?

Too complicated for testing congestion control algorithms.
As a simulation, the code doesn't need to be implemented exactly like that in the linux kernel,
and a faithful representation of the network layers is also unnecessary.

We don't even need ip or mac addresses.
Things should be kept as simple as possible.

## Roadmap

- [x] Packet type: DATA and ACK
- [x] label packet ECN and cut the window into half
- [x] AIMD algorithm should should split the bandwidth equally
- [x] DCTCP
- [x] Aggregate packets one level forward and backward
- [x] Aggregate packets for more levels
- [x] aggregatorIndex hash collision, then resend packets
- [x] FatTree(k), k means pod number
- [x] job deployment policy
- [x] resend policy when diorders happen

