#include <linkedlist.h>
#include <functional>
#include <iostream>

using namespace std;

class HashMap {
  private:
    // The backbone of the hash set. This is an array of Linked List pointers.
    linkedlist::LinkedList** array;

    // The number of buckets in the array
    size_t size; 

    // Generate a prehash for an item with a given size
    unsigned long prehash(string channel);

  public:
    // Initialize an empty hash set, where size is the number of buckets in the array
    HashMap(size_t size);

    HashMap();
    // Free all memory allocated by the hash set
    ~HashMap();

    // Hash an unsigned long into an index that fits into a hash set
    unsigned long hash(string channel);

    bool put(string channel, vector<unsigned char> messageBytes);

    // Remove an item from the set. Return true if it was removed, false if it wasn't (i.e. it wasn't in the set to begin with)
    bool remove(string channel);

    // Return true if the item exists in the set, false otherwise
    bool containsKey(string channel);

    // Resize the underlying table to the given size. Recalculate the load factor after resize
    void resize(size_t new_size);

    // Returns the number of items in the hash set
    size_t len();

    // Returns the number of items the hash set can hold before reallocating
    size_t capacity();

    // Print Table. You can do this in a way that helps you implement your hash set.
    void print();

    vector<unsigned char> get(string channel);
};

unsigned long HashMap::prehash(string channel) 
{
  // Reference: https://cp-algorithms.com/string/string-hashing.html
  const int p = 31;
  const int m = 1e9 + 9;
  long long hash_value = 0;
  long long p_pow = 1;
  for (char c : channel)
  {
    hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
    p_pow = (p_pow * p) % m;
  }
  return hash_value;
}

HashMap::HashMap(size_t size)
{
  this->array = new linkedlist::LinkedList*[size];
  for(int i =0;i<size;i++)
  {
    this->array[i] = new linkedlist::LinkedList();
  }
  this->size=size;
}

HashMap::HashMap()
{
  this->array = new linkedlist::LinkedList*[10];
  for(int i =0;i<10;i++)
  {
    this->array[i] = new linkedlist::LinkedList();
  }
  this->size=10;
}

HashMap::~HashMap()
{
  for(int i = 0;i<this->size;i++)
  {
    array[i]->~LinkedList();
  }
}

vector<unsigned char> HashMap::get(string channel) {
  unsigned long hashed = hash(channel);
  bool res = this->array[hashed]->containsItem(channel);
  if (res) {
    return this->array[hashed]->getLatestMessage(channel);
  }
  return (vector<unsigned char>)NULL;
}

unsigned long HashMap::hash(string channel)
{
  unsigned long pre = this->prehash(channel);
  unsigned long s = (unsigned long)(pre%this->size);
  return s;
}

bool HashMap::put(string channel, vector<unsigned char> messageBytes)
{
  if(this->containsKey(channel)==true)
  {
    return false;
  }
  unsigned long hashed = hash(channel);
  this->array[hashed]->insertAtTail(channel, messageBytes);
  int buckets = 0;
  for(int i =0;i<this->size;i++)
  {
    if(this->array[i]->length>0)
    {
      buckets++;
    }
  }
  double loadfactor = (double)(buckets)/(double)(this->size);
  if(loadfactor>=0.70)
  {
    this->resize(this->size*2);
  }
  return true;
}

bool HashMap::containsKey(string channel)
{
  unsigned long hashed = hash(channel);
  bool res = this->array[hashed]->containsItem(channel);
  return res;
}

void HashMap::resize(size_t new_size)
{
  linkedlist::LinkedList** old = this->array;
  linkedlist::Node *element[this->len()];
  size_t limit = this->size;
  size_t currentIndex = 0;
  for(size_t i =0;i<limit;i++)
  {
    if(this->array[i]->length>0)
    {
      for(int j = 0;j<this->array[i]->length;j++)
      {
        element[currentIndex++] = this->array[i]->itemAtIndex(j);
      }
    }
  }
  this->size = new_size;
  this->array = new linkedlist::LinkedList*[new_size];
  for(int i =0;i<new_size;i++)
  {
    this->array[i] = new linkedlist::LinkedList();
  }
  for(int i =0;i<currentIndex;i++)
  {
    this->put(element[i]->channel, element[i]->messageBytes);
  }
  for(int i = 0;i<limit;i++)
  {
    old[i]->~LinkedList();
  }
}

size_t HashMap::len()
{
  size_t len = 0;
  for(int i =0;i<this->size;i++)
  {
    len += this->array[i]->length;
  }
  return len;
}


size_t HashMap::capacity()
{
  return (size_t)(0.7*(double)this->size) - 1;
}

void HashMap::print()
{
  for(int i =0;i<this->size;i++)
  {
    printf("List %d: ",i+1);
    this->array[i]->printList();
    printf("\n");
  }
}
