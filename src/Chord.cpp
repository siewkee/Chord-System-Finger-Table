// Name: Hung Siew Kee
// ID: 5986606

#include <iostream>
#include <cstdlib>
#include <map>
#include <iterator> 
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <math.h>

#include "Chord.h"

// NOTE:
	// Each peer has n number of links to the calculated entires in FT
	// FT = Finger table
	// FT[0] = predessor
	// FT[1] = successor

using namespace std;

int main (int a, char* arg[])
{
	string file = arg[1];
	Peerptr chord = NULL;
	int n, size;
	
	if(file.length() > 0)
		read(file, chord, n, size);
	
	delete_chord(chord);
		
}

void init(int n, Peerptr& chord)
{
	if (chord != NULL)							// if there is an existing chord system
		delete_chord(chord);								// delete existing chord
		
	chord = create_peer(0, n);						// create P0
		
	for (int i = 0; i < n + 1; i++)					// populate FT
		chord -> finger_table[i] = chord;			// all entires on FT = current peer address
}

Peerptr create_peer(int peer_id, int n)
{
	Peerptr temp = new Peer;						// create new peer, assign pointer
	temp -> ID = peer_id;							// store peer ID							
	int FT_size = n + 1;								// create FT size of n + 1 to store peer ID in FT[0]
	temp -> finger_table = new Peerptr [FT_size];	//	initialize size of FT
																		// Peerptr array of dynamic size to store 
																 		// address of peers according to FT entires
	return temp;
}

void add_peer(Peerptr chord, int peer_id, int n)
{
	cout << "\n[P0]: Receives request to add peer " << peer_id << endl;
	
	int chord_size = pow(2, n);						// size of chord
	
	if (peer_id >= chord_size)
		cout << "Invalid ID: Larger than cord size" << endl;
	else
	{
		Peerptr new_p = create_peer(peer_id, n);							// creation of peer struct obj with pointer
		Peerptr insert_p = resolve(chord, peer_id, n, chord_size);	//use resolve function to look for insertion point
			
		if (insert_p -> ID == peer_id)				// peer with ID already exist
			cout << "Invalid ID: ID already exist" << endl;
		else
		{
			cout << "Inserted Peer " << peer_id << endl;
			cout << "[P" << peer_id << "]: Populate FT" << endl;
			
			new_p -> finger_table[0] = insert_p -> finger_table[0];	// update current peer's predecessor
			cout << "[P" << peer_id << "]: FT[0] = " << insert_p -> finger_table[0] -> ID << endl;	
			
			insert_p -> finger_table[0] = new_p;							// update current peer's successor's predecessor
			
			new_p -> finger_table[1] = insert_p;	// FT[1] = successor
			cout << "[P" << peer_id << "]: FT[1] = " << insert_p -> ID << endl;
				
			int FT_size = n + 1;														
			for (int i = 2; i < FT_size; i++)		// populate remaining entires in FT
			{ 
				int k = new_p -> ID;						// calculate k = p + 2^(i-1)
				k += pow(2, i-1);	
				
				new_p -> finger_table[i] 
					= succ_k(chord, new_p -> finger_table[1], k, chord_size);
					
				cout << "[P" << peer_id << "]: FT[" << i << "] = " << new_p -> finger_table[i] -> ID << endl;
			}
		}
		recompute_FT(chord, new_p, new_p -> finger_table[0] -> ID, n, chord_size);	// recompute affected FT entries of peers
		transferItems_forAddPeer(new_p, new_p -> finger_table[0] -> ID);				// transfer items to new peer																
		
		cout << "PEER " << peer_id << " ADDED" << endl;
		print_chord(chord);
		
	}	
}

void delete_peer(Peerptr& chord, int peer_id, int n)
{
	cout << "\n[P0]: Receives request to delete peer " << peer_id << endl;
	
	int chord_size = pow(2, n);					// size of chord
	
	if (peer_id >= chord_size || peer_id < 0)
		cout << "ID not found" << endl;
	else
	{
		Peerptr delete_p;
		
		if (peer_id == 0)
		{
			delete_p = chord;
			chord = chord -> finger_table[1];	
		}
		else
			delete_p = resolve(chord, peer_id, n, chord_size);		//use resolve function to look for peer to be deleted
		
		delete_p -> finger_table[1] -> finger_table[0] = delete_p -> finger_table[0];		// cur -> next -> prev = cur -> prev
		delete_p -> finger_table[0] -> finger_table[1] = delete_p -> finger_table[1];		// cur -> prev -> next = cur -> next

		cout << "[P" << peer_id << "]: Updated links of predecessor and sucessor peer" << endl;
		recompute_FT(chord, delete_p -> finger_table[1], delete_p -> finger_table[0] -> ID, n, chord_size);			
		
		cout << "[P" << peer_id << "]: Transferred all data item to successor" << endl;
		multimap <int, string> :: iterator itr;
		for (itr = delete_p -> peer_storage.begin(); itr != delete_p -> peer_storage.end(); itr++)
				delete_p -> finger_table[1] -> peer_storage.insert({itr -> first, itr -> second});
		
		delete[] delete_p -> finger_table;
		delete_p -> peer_storage.clear();
		delete delete_p;
		
		cout << "PEER " << peer_id << " REMOVED" << endl;
		print_chord(chord);
	}
}

Peerptr succ_k(Peerptr& chord, Peerptr peer, int k, int chord_size)				
{
	Peerptr walk, end, succ;
	
	if (k >= chord_size)								// if derived key value >= chord size
	{
		k %= chord_size;								//	mod k with chord size
		walk = chord;									// start "walking" from P0
		end = chord;
	}
	else
	{
		walk = peer;									// else start from current peer
		end = peer;
	}

	bool node_found = false;	
	do								    					// loop ends when all nodes visited
	{											 			// or when node for key is found
		Peerptr cur = walk;
		walk = walk -> finger_table[1];			// FT[1] = successor
		
		if (k <= cur -> ID)									// if key is <= to ID of current peer
		{
			node_found = true;
			succ = cur;
		}
	}while (walk != end && node_found == false);	
		
	if (node_found == false)						// if unable to resolve after walking all peers
	{															// i.e. keys between 0 and predessors; 
																// comparison of these keys and peer ID value 0 will not result in true condition
		succ = chord;
	}
	
	return succ;
}

Peerptr resolve(Peerptr& chord, int k, int n, int chord_size)
{
	/*
	Textbook definition:
	The core of what a node does during a lookup is encoded in localSuccNode(k). 
	When handed a key k, it will either return 
		itself, 
		its immediate successor FT[1], 
		or go through the finger table to search the entry satisfying FT[i] ≤ k < FT[i+1].
	*/
	cout << "Resolving " << k << endl;
	vector <int> path;
	
	Peerptr walk = chord; 
	path.push_back(chord -> ID);
	
	if (k >= chord_size);
		k %= chord_size;
		
	int FT_size = n + 1;
	
	bool find_resolved_p = false;
	bool find_forwardreq_p = false;
	Peerptr resolved_peer;
	int walk_counter = 0;
	
	do
	{
		Peerptr cur = walk;
		walk = walk -> finger_table[1];
		
		if (walk == chord && walk_counter == 0)						// if there is only one peer in the chord
		{			
			find_resolved_p = true;
			resolved_peer = cur;
			path.push_back(cur -> ID);
		}
		else
		{
			if (k > cur -> finger_table[0] -> ID && k <= cur -> ID)	// if peer receiving resolve request is responsible for k	
			{
				find_resolved_p = true;
				resolved_peer = cur;
				path.push_back(cur -> ID);
			}
			else											// iterate the rest of the FT to forward resolve request
			{
				int FT_counter = 1;
				while (find_forwardreq_p == false && FT_counter < n)	
				{
						if (cur -> finger_table[FT_counter] -> ID <= k 		// forward peer condition = FT[i] ≤ k < FT[i+1]
							&& k < cur -> finger_table[FT_counter+1] -> ID)	
						{
							walk = cur -> finger_table[FT_counter];
							path.push_back(cur -> finger_table[FT_counter] -> ID);
							find_forwardreq_p == true;
						}
						FT_counter++;
				}			
			}
		}
		walk_counter++;
	}while(walk != chord && find_resolved_p == false);
	
	if (find_resolved_p == false)					// if unable to resolve after walking all peers
	{															// i.e. keys between 0 and predecessors; 
																// comparison of these keys and peer ID value 0 will not result in true condition
		resolved_peer = chord;
		path.push_back(chord -> ID);
	}
	
	cout << "Path : ";
	for (int i = 0; i < path.size()-1; i++)	// print resolving path
		cout << path[i] << " > ";
	cout << path[path.size()-1] << endl;

	return resolved_peer;
}

void recompute_FT(Peerptr& chord, Peerptr& new_p, int new_p_prevID, int n, int chord_size)
{
	int FT_size = n + 1;
	Peerptr walk = chord;
	
	do
	{
		Peerptr cur = walk;
		walk = walk -> finger_table[1];

		for (int i = 1; i < FT_size; i++)
		{
			int k = cur -> ID;									// calculate k = p + 2^(i-1)
			k += pow(2, i-1);	
			
			if (k >= chord_size);
				k %= chord_size;

			if (k > new_p_prevID && k <= new_p -> ID)		// if new_p_prevID < k <= new_p -> ID
			{
				cout << "[P" << cur -> ID << "]: " 
					  << "Recompute FT[" << i << "] = " << new_p -> ID    
					  << endl;
				cur -> finger_table[i] = new_p;
			}
		}
	}while(walk != chord);
	
}

void delete_chord(Peerptr& chord)
{
	Peerptr walk = chord -> finger_table[1];
	
	do
	{
		Peerptr cur = walk;								// deletion start from next immediate note from P0
		walk = walk -> finger_table[1];
		
		delete[] cur -> finger_table;
		cur -> peer_storage.clear();
		delete cur;
	
	}while(walk -> finger_table[1]  != chord);	// loop ends if next Peer is P0
		
	delete [] chord -> finger_table;				//deletion of P0
	chord -> peer_storage.clear();
	delete chord;	
}

void print_peer(Peerptr& chord, int peer_id, int n)
{
	cout << "\n[P0]: Received request to print peer " << peer_id << endl;
	int chord_size = pow(2, n);
	Peerptr find_peer = resolve(chord, peer_id, n, chord_size);
	print_storage(find_peer);
	
	for (int i = 1; i < n + 1; i++)
		cout << find_peer -> finger_table[i] -> ID << " ";
	cout << endl;
	
}

void print_chord(Peerptr& chord)
{
	Peerptr walk = chord;
	
	do
	{
		Peerptr cur = walk;
		walk = walk -> finger_table[1];							
		
		cout << "\e[7mP" << cur -> ID << " > ";
	}while(walk != chord);
	
	cout << "P" << walk -> ID << "\e[0m" << endl;
}

void print_storage(Peerptr& peer_to_print)
{
	cout << "\nDATA AT INDEX NODE " << peer_to_print -> ID << ": " << endl;
	int size = peer_to_print -> peer_storage.size();
		
	if (size != 0)
	{
		multimap <int, string> :: iterator itr;
		for (itr = peer_to_print -> peer_storage.begin(); itr != peer_to_print -> peer_storage.end(); itr++)
			cout << itr -> second << endl;
	}
	else
		cout << "**** EMPTY ****" << endl;
}

int hash_data(string data_str, int n)
{
	int key = 0;
	int len = data_str.length();

	for(int i = 0; i < len; i++)
		key = ((key << 5) + key) ^ data_str[i];
	
	int result = pow(2, n);
	
	key = key % result;
	if(key < 0)
		key += result;
	
	return key;
}

void store(Peerptr& chord, string data, int n)
{
	cout << "\n[P0]: Recevied request to store data {" << data << "}" << endl;
	
	int k = hash_data(data, n);
	int chord_size = pow(2, n);					// size of chord
	
	Peerptr peer_to_store = resolve(chord, k, n, chord_size);
	
	peer_to_store -> peer_storage.insert({k, data});	
	
	cout << "INSERTED [Data = " << data << "], [Key = " << k << "] AT PEER " << peer_to_store -> ID << endl;
	
}

void transferItems_forAddPeer(Peerptr& new_p, int new_p_prevID)
{
	Peerptr new_p_next = new_p -> finger_table[1];	// successor of new peer added
	
	int storage_size = new_p_next -> peer_storage.size();		
		
	if (storage_size != 0)									// if storage is not empty
	{
		multimap <int, string> :: iterator itr;
		for (itr = new_p_next -> peer_storage.begin(); itr != new_p_next -> peer_storage.end(); itr++)
		{
			if (itr -> first > new_p_prevID && itr -> first <= new_p -> ID)	// if key < new_p_prev and >= new_p
			{	
				int key = itr -> first;
				string data = itr -> second;
				cout << "[P" << new_p_next -> ID << "] : Transfer item {" 
					  <<	key << ", " << data << "} to Peer " << new_p -> ID << endl;																	
				new_p -> peer_storage.insert({itr -> first, itr -> second});		// transfer key and data item to new_p and 	
				
				cout << "[P" << new_p_next -> ID << "] : Delete item {" 
					  <<	key << ", " << data << "}" << endl;									
				new_p_next -> peer_storage.erase(itr -> first);							// delete key and data item from new_p_next
			}
		}
	}
}

void delete_item(Peerptr& chord, string data, int n)
{
	cout << "\n[P0]: Received request to delete data item {"<< data <<"}" << endl;
	
	int key = hash_data(data, n);					// calculate key of data item to be deleted
	int chord_size = pow(2, n);					// calculate chord size
	
	Peerptr find_peer = resolve(chord, key, n, chord_size);		// resolve peer with calculated key and data item
	
	multimap <int, string> :: iterator itr;
	itr = find_peer -> peer_storage.find(key);	// locate data item in multimap storage of peer
	
	find_peer -> peer_storage.erase(itr);		// delete data item	
	
	cout << "REMOVED [Data = " << data << "], [Key = " << key << "] FROM PEER " << find_peer -> ID << endl;
}

void read(string filename, Peerptr& chord, int& n, int size)
{

	ifstream file;
	file.open(filename);
	
	string line, command, command_value, value = "";
	vector<string> vals;
	
	bool ignore = false, first = true;
	
	int commandID = -1;
	
	if(file.fail())
	{
		file.clear();
		file.close();
		exit(-1);
	}
	else 
	{
		while(!file.eof())
		{
			getline(file,line);
			vals = split(line, ' ');
			
			for (std::vector<string>::iterator it = vals.begin(); it != vals.end(); ++it)
			{
				if(first)
				{
					command = *it;
					commandID = check_command(command);
					first = false;
				}
				else 
				{
					value = *(it);
					if(!ignore)
					{
						if(value[0] == '#' || commandID == -1)
							ignore = true;
	
						if(!ignore)
							construct_command(commandID, value, command_value);
					}
				}
			}
			
			if(commandID != -1)
			{
				execute_command(commandID, command_value, chord, n);
			}
			
			commandID = -1;
			first = true;
			ignore = false;
			command_value = "";
			vals.clear();
		}
	}
	
	file.close();	
}

void construct_command(int commandID, string value, string& commandVal)
{
	switch(commandID)
	{
		case 0:
		case 1:
		case 2:
		case 4:
			commandVal = value;
		break;
		
		case 3:
		case 5:
			if(commandVal.length() > 0)
				commandVal = commandVal + " " + value;
			else
				commandVal = value;
		break;
	}
}

int check_command(string val)
{
	if(val == "init")
		return 0;
	if(val == "newpeer")
		return 1;
	if(val == "removepeer")
		return 2;
	if(val == "store")
		return 3;
	if(val == "print")
		return 4;
	if(val == "delete")
		return 5;

	return -1;
}

void execute_command(int commandID, string commandVal, Peerptr& chord, int& n)
{
	int ID;
	
	switch(commandID)
	{
		case 0:
			n = stol(commandVal);
			init(n, chord);
			break;
		case 1:
			ID = stol(commandVal);
			add_peer(chord, ID, n);
			break;	
		case 2:
			ID = stol(commandVal);
			delete_peer(chord, ID, n);
			break;
		case 3:
			store(chord, commandVal, n);
			break;
		case 4:
			ID = stol(commandVal);
			print_peer(chord, ID, n);
			break;
		case 5:
			delete_item(chord, commandVal, n);
		break;

	}	
}

vector<string> split(const string &s, char delim) 
{
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) 
        tokens.push_back(item);
    
    return tokens;
   
}


























