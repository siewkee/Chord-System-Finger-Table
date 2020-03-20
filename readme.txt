
# Chord System

The Chord system is a structured peer-to-peer network architecture realizing a scalable distributed hash table (or hash ring to be more accurate). In Chord, the peers are organized as a ring. A position within Chord which is held by a computing node is called an index node or simply a peer. Each index node has a finger table and, in addition, holds data items associated with its key value (its ID within the ring) and any other data items associated with keys larger than the previous index node in the Chord ring.

This program simulates the Chord system on a single computer as a single (non-parallelized, not multi-threaded) process. 
The main objectives are:
• to demonstrate how the nodes in Chord communicate.
• to demonstrate how nodes are managed (added/removed) from the system.
• to demonstrate the effects of adding and removal of index nodes.
• to demonstrate how the finger table is computed and maintained.
• to demonstrate how memory utilization and data distribution takes place in Chord.
• to demonstrate how searching is done efficiently in Chord.
• To demonstrate why CHORD is a such a fast and efficient algorithm.
• to demonstrate the concept of scalable algorithms and fast code.


Terminal Commands:
g++ -o chord Chord.cpp
./chord test_instructions.txt

= "test_instructions.txt" can be replaced with file name
= file must be in the same folder as .cpp and .h file


