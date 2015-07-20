#ifndef UTIL
#define UTIL

#include <cstdlib>
#include <climits>
#include <ctime>
#include <vector>
using namespace std;

size_t getHashSize(size_t);

template<class Key, class Node>
class HashMap{

typedef pair<Key, Node> HashNode;

public:
  HashMap(size_t num){
    _size = getHashSize(num);
    bucket = new vector<HashNode> [_size];
  }
  ~HashMap(){delete [] bucket;}

  Node insert(Key k, Node n){
    size_t num = getBucketNum(k);
    for(size_t i=0; i < bucket[num].size(); ++i){
      if(bucket[num][i].first() == k())
        return bucket[num][i].second;
    }
    bucket[num].push_back(HashNode(k, n));
    return 0;
  }

  vector<HashNode>& operator [](size_t i){
    return bucket[i];
  }

  size_t size(){return _size;}

private:
  vector<HashNode>* bucket;
  size_t _size;
  size_t getBucketNum(Key& key){
    return key() % _size;
  }
};

class RandGenerator{
  private:
  public:
    RandGenerator(){
      srand(time(0));
    }

    RandGenerator(int seed){
      srand(seed);
    }
    
    void reset(int seed = time(0)){
      srand(seed);
    }

    int getRand(){
      return rand() * (double)INT_MAX / RAND_MAX;
    }
};

class IntKey{
  private:
    int k;
  public:
    IntKey(int i):k(i){}
    int operator()(){return k;}
};
#endif
