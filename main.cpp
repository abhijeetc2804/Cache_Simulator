#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include<set>
#include <queue>
#include<deque>
#include<map>
#include <list>  
#include <unordered_map>

using namespace std;

using ll =long long ;
using ld =long double;

#define pb push_back
#define forn(i, n) for (int i = 0; i < int(n); i++)
#define vec vector
#define str string
#define all(v) v.begin(),v.end()
#define rall(v) v.rbegin(),v.rend()
#define Tp template <typename T>
#define Tp_ template <typename T1,typename T2>
#define minHeap priority_queue<ll, vector<ll>, greater<ll>>; // Min-Heap
#define maxHeap priority_queue<ll>;  //Max-Heap
typedef vector<ll> vll;
typedef pair<ll,ll> pll;
typedef vector<pair<ll,ll>> vpll;

// #define Abhijeet 1

#ifdef Abhijeet
#define deb(x) cout<<#x<<"="<<x<<"\n";
#else 
#define deb(x)
#endif
#ifdef Abhijeet
#define deb2(x,y) cout<<#x<<"="<<x<<","<<#y<<"="<<y<<"\n";
#else 
#define deb2(x,y)
#endif
#ifdef Abhijeet
#define debm(x) {int sz=x.size();int i=0;cout<<"["<<#x<<":"<<" {";for(auto&it:x){cout<<it.first<<": "<<it.second;if(i<sz-1){cout<<", ";}i++;}cout << "}" << "]\n";}
#else 
#define debm(x)
#endif
#ifdef Abhijeet
#define debs(x) {int sz=x.size();int i=0;cout<<"["<<#x<<":"<<" {";for(auto&it:x){cout<<it;if(i<sz-1){cout<<", ";}i++;}cout << "}" << "]\n";}
#else 
#define debs(x)
#endif

class Block {
    private: 
        long long tag;
        bool valid;
        int lastUsed;
        bool dirty;

    public:
    Block() {
        tag = 0;
        valid = false;
        dirty = false;
        lastUsed = 0;
    }

    void setTag(unsigned long newTag) {
        tag = newTag;
    }

    void setValid(bool newValue) {
        valid = newValue;
    }

    void setDirty(bool newValue) {
        dirty = newValue;
    }

    void setLastUsed(int time) {
        lastUsed = time;
    }

    long long getTag() const {
        return tag;
    }

    bool isValid() const {
        return valid;
    }

    bool isDirty() const {
        return dirty;
    }

    int getLastUsed() const {
        return lastUsed;
    }
};

class Set {
    public:
        vector<Block> vec_of_blocks;
        set<pair<int,int>> my_set;  // Stores pairs of (lastUsed or insertionOrder, index)
        unordered_map<int,int> times,vals;// (time of last edit) and (tag)
        map<int,int> dirty_mp;
        Set() {} ;
        explicit Set(int numBlocks) : vec_of_blocks(numBlocks) {}

        Block& getBlock(int idx){
            return vec_of_blocks[idx];
        }
        int getsize(){
            return vec_of_blocks.size();
        }
};

int my_log2(long long x){
    if(x<=1){
        return 0;
    }
    return 1+my_log2(x/2);
}

class Cache {
    public: 
        vector<Set> vec_of_sets;
        int num_of_sets;
        int num_of_blocks_per_set;
        int num_of_hits, num_of_misses;
        int block_sz;
        int load_hits,load_misses,store_hits,store_misses;
        int curr_time;
        int loads,stores;
        char writeback_or_thru;
        string eviction_policy;
        int cycles=0;
        bool write_allocate;
        

    Cache(int numSets, int blocksPerSet, int blockSize, char writeBackOrThrough, const std::string& evictionPolicy, bool writeAllocate)
        : num_of_sets(numSets), num_of_blocks_per_set(blocksPerSet), block_sz(blockSize),
          writeback_or_thru(writeBackOrThrough), eviction_policy(evictionPolicy), write_allocate(writeAllocate),
          num_of_hits(0), num_of_misses(0), curr_time(0),loads(0),stores(0),
          load_hits(0),load_misses(0),store_hits(0),store_misses(0)
           {
        vec_of_sets.resize(num_of_sets, Set(num_of_blocks_per_set));
    }

    void accessMem(char type,long long address){
        curr_time++;
        if(type=='s'){
            stores++;
        }
        else{
            loads++;
        }
        // deb(address);
        int set_idx=(address/block_sz)%num_of_sets;
        bool isWrite=(type=='s');
        long long tag=address/(block_sz);
        Set& s1 = vec_of_sets[set_idx];
        // if(writeback_or_thru=='b' && type=='s'){
        //     s1.dirty_mp[tag]=1;
        // }
        if(s1.vals.find(tag)!=s1.vals.end()){
            if(type=='l'){
                load_hits++;
                cycles++;
            }
            else{
                store_hits++;
                if(writeback_or_thru=='t'){
                    cycles+=101;
                }
                else{
                    cycles++;
                    s1.dirty_mp[tag]=1;
                }
            }
            if(eviction_policy=="lru"){
                int t1=s1.times[tag];
                s1.my_set.erase({t1,tag});
                s1.times[tag]=curr_time;
                s1.vals[tag]=tag;
                s1.my_set.insert({curr_time,tag});
            }
            else{
                // do nothing
            }
        }
        else if(s1.my_set.size()<num_of_blocks_per_set){
            if(type=='l'){
                load_misses++;
                cycles+=(1+25*block_sz);
                s1.my_set.insert({curr_time,tag});
                s1.vals[tag]=tag;
                s1.times[tag]=curr_time;
            }
            else{
                store_misses++;
                if(writeback_or_thru=='b' && write_allocate){
                    s1.dirty_mp[tag]=1;
                }
                if(write_allocate && writeback_or_thru=='b'){
                    cycles+=(1+25*block_sz);
                    s1.my_set.insert({curr_time,tag});
                    s1.vals[tag]=tag;
                    s1.times[tag]=curr_time;
                }
                else if(write_allocate && writeback_or_thru=='t'){
                    cycles+=(101+25*block_sz);
                    s1.my_set.insert({curr_time,tag});
                    s1.vals[tag]=tag;
                    s1.times[tag]=curr_time;
                }
                else {
                    cycles+=100;
                }
            }
        }
        else{
            if(type=='l'){
                load_misses++;
                cycles+=(1+25*block_sz);
                pair<int,int> p1=*s1.my_set.begin();
                if(s1.dirty_mp[p1.second]){
                    cycles+=25*block_sz;
                    s1.dirty_mp[p1.second]=0;
                }
                s1.my_set.erase(p1);
                s1.times.erase(p1.second);
                s1.vals.erase(p1.second);
                s1.my_set.insert({curr_time,tag});
                s1.times[tag]=curr_time;
                s1.vals[tag]=tag;
            }
            else{
                store_misses++;
                if(writeback_or_thru=='b' && write_allocate){
                    s1.dirty_mp[tag]=1;
                }
                if(write_allocate && writeback_or_thru=='b'){
                    cycles+=(1+25*block_sz);
                    pair<int,int> p1=*s1.my_set.begin();
                    if(s1.dirty_mp[p1.second]){
                        cycles+=25*block_sz;
                        s1.dirty_mp[p1.second]=0;
                    }
                    s1.my_set.erase(p1);
                    s1.times.erase(p1.second);
                    s1.vals.erase(p1.second);
                    s1.my_set.insert({curr_time,tag});
                    s1.times[tag]=curr_time;
                    s1.vals[tag]=tag;
                }
                else if(write_allocate && writeback_or_thru=='t'){
                    cycles+=(101+25*block_sz);
                    pair<int,int> p1=*s1.my_set.begin();
                    if(s1.dirty_mp[p1.second]){
                        cycles+=25*block_sz;
                        s1.dirty_mp[p1.second]=0;
                    }
                    s1.my_set.erase(p1);
                    s1.times.erase(p1.second);
                    s1.vals.erase(p1.second);
                    s1.my_set.insert({curr_time,tag});
                    s1.times[tag]=curr_time;
                    s1.vals[tag]=tag;
                }
                else {
                    cycles+=100;
                }
            }
        }
    }

    void print_results(){
        cout<<"Total loads: "<<loads<<"\n";
        cout<<"Total stores: " << stores << "\n";
        cout<<"Load hits: "<<load_hits<<"\n"; 
        cout<<"Load misses: "<<load_misses<<"\n"; 
        cout<<"Store hits: "<<store_hits<<"\n"; 
        cout<<"Store misses: "<<store_misses<<"\n"; 
        cout<<"Total cycles: " << (cycles) << std::endl;
    }
};


int main(int argc, char* argv[]) {
    if (argc != 7) {
        cout<<argc<<"\n";
        std::cerr << "Usage: " << argv[0] << " <numSets> <numBlocksPerSet> <blockSize> <writeBackOrThrough> <evictionPolicy> <writeAllocate>" << std::endl;
        return 1;
    }
    int numSets = stoi(argv[1]);
    int blocksPerSet = stoi(argv[2]);
    int blockSize = stoi(argv[3]);
    bool writeAllocate = (argv[4][0]=='w');
    char writeBackOrThrough = argv[5][6];// 'b' or 't'(write-back or write-through)
    string evictionPolicy = argv[6];

    deb2(writeAllocate,writeBackOrThrough);
    Cache cache(numSets, blocksPerSet, blockSize, writeBackOrThrough, evictionPolicy, writeAllocate);

    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        char action; // 'l' or 's'
        unsigned long address;
        iss >> action >> std::hex >> address;
        // deb2(action,address);
        cache.accessMem(action, address);
    }

    cache.print_results();
    return 0;
}