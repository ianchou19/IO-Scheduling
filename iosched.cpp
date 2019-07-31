#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cstring>
#include <bits/stdc++.h>
#include <map>

using namespace std;


// Global variables

string sche_algo_type;
int timer = 0;
int total_time = 0;
int tot_movement = 0;
double tot_turnaround = 0;
double tot_waittime = 0;
int max_waittime = 0;
int direction = 1;   //up == 1, down == -1
bool v = false;
vector<pair<int,int>> io_operation;
map<int,int> all_io_requests_map;


// Structs
typedef struct {     //IO re
    int IO_number;
    int arrive_time;
    int target_track;
    int pre_track;
    int begin_time;
    int finish_time;
    bool active;
} io_item;


// Global Instances
io_item* act_io = new io_item();
vector<io_item*> all_io_requests;
vector<io_item*> io_queue;
vector<io_item*> active_queue;


// Classes

class ScheduleAlgo {
    public:
        virtual io_item* select_io_request() = 0;     //this funciton will return a victim frame
};


class FIFO : public ScheduleAlgo {
    public:
        io_item* select_io_request() {
            io_item* io_it = io_queue.front();
            io_queue.erase(io_queue.begin());
            return io_it;
        }
};


class SSTF : public ScheduleAlgo {
    public:
        io_item* select_io_request() {
            int distance;
            int shortest = INT_MAX;
            io_item* shortest_io_it;

            for (int i = 0; i < io_queue.size(); i++) {
                distance = abs(io_queue[i]->target_track - act_io->target_track);

                if (distance < shortest) {
                    shortest = distance;
                    shortest_io_it = io_queue[i];
                }
            }

            for (int i = 0; i < io_queue.size(); i++) {
                if (io_queue[i]->IO_number == shortest_io_it->IO_number) {
                    io_queue.erase(io_queue.begin() + i);
                }
            }

            return shortest_io_it;
        }
};


class LOOK : public ScheduleAlgo {
    public:
        io_item* select_io_request() {
            int distance;
            int shortest = INT_MAX;
            io_item* next_io_it;

            for (int i = 0; i < io_queue.size(); i++) {
                distance = abs(io_queue[i]->target_track - act_io->target_track);

                if (direction == 1) {
                    if (io_queue[i]->target_track >= act_io->target_track && distance < shortest) {
                        shortest = distance;
                        next_io_it = io_queue[i];
                    }
                }
                else if (direction == -1) {
                    if (io_queue[i]->target_track <= act_io->target_track && distance < shortest) {
                        shortest = distance;
                        next_io_it = io_queue[i];
                    }
                }
            }

            if (shortest == INT_MAX) {   //need to change direction
                if (direction == 1) {
                    direction = -1;
                }
                else if (direction == -1) {
                    direction = 1;
                }

                next_io_it = select_io_request();
            }

            for (int i = 0; i < io_queue.size(); i++) {
                if (io_queue[i]->IO_number == next_io_it->IO_number) {
                    io_queue.erase(io_queue.begin() + i);
                }
            }

            return next_io_it;
        }
};


class CLOOK : public ScheduleAlgo {
    public:
        io_item* select_io_request() {
            int distance;
            int shortest = INT_MAX;
            io_item* next_io_it;

            for (int i = 0; i < io_queue.size(); i++) {
                distance = abs(io_queue[i]->target_track - act_io->target_track);

                if (io_queue[i]->target_track >= act_io->target_track && distance < shortest) {
                    shortest = distance;
                    next_io_it = io_queue[i];
                }
            }

            if (shortest == INT_MAX) {   //need to go the right most track
                int smallest_track = INT_MAX;

                for (int i = 0; i < io_queue.size(); i++) {
                    if (io_queue[i]->target_track < smallest_track) {
                        smallest_track = io_queue[i]->target_track;
                        next_io_it = io_queue[i];
                    }
                }
            }

            for (int i = 0; i < io_queue.size(); i++) {
                if (io_queue[i]->IO_number == next_io_it->IO_number) {
                    io_queue.erase(io_queue.begin() + i);
                }
            }

            return next_io_it;
        }
};


class FLOOK : public ScheduleAlgo {
    public:
        io_item* select_io_request() {
            int distance;
            int shortest = INT_MAX;
            io_item* next_io_it;

            if (active_queue.size() == 0) {
                //cout << "active_queue's size: " << active_queue.size();

                //if Active Queue is empty, then go to Add Queue (io_queue)
                active_queue = io_queue;
                io_queue.clear();

                //cout << " --> " << active_queue.size() << endl;
            }

            /*
            if (direction == 1) {
                cout << "direction up" << endl;
            } else {cout << "direction down" << endl;}
            */

            for (int i = 0; i < active_queue.size(); i++) {
                //cout << "act_io's target_track: " << act_io->target_track << endl;
                distance = abs(active_queue[i]->target_track - act_io->target_track);

                if (direction == 1) {
                    if (active_queue[i]->target_track >= act_io->target_track && distance < shortest) {
                        //cout << "pick: " << active_queue[i]->IO_number << "    " << active_queue[i]->target_track << endl;
                        shortest = distance;
                        next_io_it = active_queue[i];
                    }
                    //else {cout << active_queue[i]->IO_number << "    " << active_queue[i]->target_track << endl; }
                }
                else if (direction == -1) {
                    if (active_queue[i]->target_track <= act_io->target_track && distance < shortest) {
                        //cout << "pick: " << active_queue[i]->IO_number << "    " << active_queue[i]->target_track << endl;
                        shortest = distance;
                        next_io_it = active_queue[i];
                    }
                    //else {cout << active_queue[i]->IO_number << "    " << active_queue[i]->target_track << endl; }
                }
            }

            if (shortest == INT_MAX) {
                if (direction == 1) {
                    direction = -1;
                }
                else if (direction == -1) {
                    direction = 1;
                }

                //cout << "Get: () --> change direction to " << direction << endl;
                next_io_it = select_io_request();
            }

            for (int i = 0; i < active_queue.size(); i++) {
                if (active_queue[i]->IO_number == next_io_it->IO_number) {
                    active_queue.erase(active_queue.begin() + i);
                }
            }

            return next_io_it;
        }
};


// Global Instance

ScheduleAlgo* sche_algo;


// Functions

void Simulator() {
    int finish_IO_num = -1;
    int IO_num = 0;
    int pre_track;
    int pending_io_num;
    act_io->finish_time = 1;
    act_io->IO_number = -1;
    act_io->active = false;
    bool in_same_time = false;

    while (all_io_requests_map.size() != finish_IO_num) {
        timer += 1;
        total_time += 1;

        //add new IO request into io_queue
        if (all_io_requests_map.find(timer) != all_io_requests_map.end() && !in_same_time) {  //don't re-add when in the same time unit
            if (v) {cout << timer << ":     " << IO_num << " add " << all_io_requests_map[timer] << endl;}

            io_item* io_it = new io_item();
            io_it->IO_number = IO_num;
            io_it->arrive_time = timer;
            io_it->target_track = all_io_requests_map[timer];

            io_queue.push_back(io_it);
            all_io_requests.push_back(io_it);
            IO_num += 1;
        }

        if (act_io->finish_time == timer) {  //when the active IO access to the target track
            if (act_io->IO_number != -1) {
                if (v) {cout << timer << ":     " << act_io->IO_number << " finish " << (act_io->finish_time - act_io->arrive_time) << endl;}

                tot_turnaround += (act_io->finish_time - act_io->arrive_time);
            }

            finish_IO_num += 1;
            act_io->active = false;   //represent there's no active IO now
        }

        if (sche_algo_type == "f") {
            pending_io_num = active_queue.size() + io_queue.size();
        }
        else {
            pending_io_num = io_queue.size();
        }

        //select a new io request from queue
        if (act_io->active == false && pending_io_num > 0) {
            pre_track = act_io->target_track;

            act_io = sche_algo->select_io_request();    //use Schedule Algo pick next IO request

            act_io->pre_track = pre_track;
            act_io->begin_time = timer;
            act_io->finish_time = timer + abs(act_io->target_track - act_io->pre_track);

            all_io_requests[act_io->IO_number] = act_io;    //update the active io's info into all_io_requests vector
            tot_movement += abs(act_io->target_track - act_io->pre_track);
            act_io->active = true;

            if (v) {cout << timer << ":     " << act_io->IO_number << " issue " << act_io->target_track << " " << act_io->pre_track << endl;}

            tot_waittime += (act_io->begin_time - act_io->arrive_time);
            if (max_waittime < (act_io->begin_time - act_io->arrive_time)) {
                max_waittime = (act_io->begin_time - act_io->arrive_time);
            }

            //when new issue IO's finishing time equals to current time, which means the next IO access the same track
            if (act_io->finish_time == timer) {
                timer -= 1;     //in this case, next round time should keep the same for the new issue IO
                total_time -= 1;
                in_same_time = true;
            } 
            else {in_same_time = false;}
        }
    }
}


void read_file(char * filename) {
    ifstream inFile (filename);
    string line;
    string word;
    pair<int, int> pair;
    vector<int> tmp;
    int io_time;
    int track;

    while(getline(inFile, line)) {
        stringstream ls(line);

        while(ls >> word) {
            //if the first string in a line is #, jump to the next line
            if (word == "#") { break; }

            //if the first char of the first string in a line is #, jump to the next line
            char word_char[word.length() + 1];
            strcpy(word_char, word.c_str());
            if (word_char[0] == '#') { break; }

            tmp.push_back(stoi(word));
        }

        if (tmp.size() == 2) {
            io_time = tmp[0];
            track = tmp[1];

            pair = make_pair(io_time, track);
            io_operation.push_back(pair);

            all_io_requests_map[io_time] = track;
        }

        tmp.clear();
    }
}


void PrintResult(){
    for (int i = 0; i < all_io_requests.size(); i++) {
        printf("%5d: %5d %5d %5d\n", all_io_requests[i]->IO_number, all_io_requests[i]->arrive_time, all_io_requests[i]->begin_time, all_io_requests[i]->finish_time);
    }

    double avg_turnaround = tot_turnaround / all_io_requests_map.size();
    double avg_waittime = tot_waittime / all_io_requests_map.size();
    printf("SUM: %d %d %.2f %.2f %d\n", total_time, tot_movement, avg_turnaround, avg_waittime, max_waittime);
}


int main(int argc, char* argv[]) {
    //v = true;

    int i = 1;

    //Get Commandline arguments
    if (argv[i][2] == 'i') {
        sche_algo = new FIFO();
    }
    else if (argv[i][2] == 'j') {
        sche_algo = new SSTF();
    }
    else if (argv[i][2] == 's') {
        sche_algo = new LOOK();
    }
    else if (argv[i][2] == 'c') {
        sche_algo = new CLOOK();
    }
    else if (argv[i][2] == 'f') {
        sche_algo_type = "f";
        sche_algo = new FLOOK();
    }

    i++;
    char* filename = argv[i];   // Parse input file
    read_file(filename);


    Simulator();    //Begin simulation
    PrintResult();  //Final output

    return 0;
}