// Name: Hung Siew Kee
// ID: 5986606

#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

struct Peer;
typedef Peer* Peerptr;

struct Peer
{
	int ID;
	Peerptr* finger_table;
	multimap <int, string> peer_storage;
};

//initialize chord
void init(int, Peerptr&);


//functions for creating new peer
Peerptr create_peer(int, int);
void add_peer(Peerptr, int, int);
Peerptr succ_k(Peerptr&, Peerptr, int, int);		
Peerptr resolve(Peerptr&, int, int, int);
void recompute_FT(Peerptr&, Peerptr&, int, int, int);

//functions for delete 
void delete_chord(Peerptr&);
void delete_peer(Peerptr&, int, int);

//other functions
void print_peer(Peerptr&, int);
void print_chord(Peerptr&);
void print_storage(Peerptr&);
int hash_data(string, int);
void store(Peerptr&, string, int);
void transferItems_forAddPeer(Peerptr&, int);
void transferItems_forDeletePeer(Peerptr&, int);
void delete_item(Peerptr&, string, int);

void read(string, Peerptr&, int&, int);
void construct_command(int, string, string&);
int check_command(string);
void execute_command(int, string, Peerptr&, int&);
vector<string> split(const string &, char);



/*
release memory:
	Peerptrs starting from P0
	fingertable of each peer
	multimap storage of each peer
*/

