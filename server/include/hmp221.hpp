#include <vector>
#include <string>

#ifndef HMP221_HPP
#define HMP221_HPP

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long u64;
typedef signed char i8;
typedef signed int i32;
typedef signed long i64;
typedef float f32;
typedef double f64;
typedef std::vector<u8> vec;
typedef std::string string;

#define HMP221_U8 0xa2
#define HMP221_S8 0xaa
#define HMP221_S16 0xab
#define HMP221_A8 0xac
#define HMP221_A16 0xad
#define HMP221_M8 0xae

struct Message
{
    string channelName;
    vec contentBytes;
};

struct Request
{
    string name; // The name of the channel
};

namespace hmp221
{

    void printVec(vec &bytes);

    vec serialize(u8 item);
    u8 deserialize_u8(vec bytes);

    vec serialize(string item);
    string deserialize_string(vec bytes);

    vec serialize(std::vector<u8> item);
    std::vector<u8> deserialize_vec_u8(vec bytes);

    // Maps
    vec serialize(struct Message item);
    struct Message deserialize_message(vec bytes);

    vec serialize(struct Request item);
    struct Request deserialize_request(vec bytes);

    // helper method for getting sub vector
    vec slice(vec &bytes, int vbegin, int vend);
}

#endif
