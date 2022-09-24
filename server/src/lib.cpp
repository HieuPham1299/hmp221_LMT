#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "hmp221.hpp"
#include <iostream>

using std::begin;
using std::end;
#define X8 256
#define X16 65536
#define MASK4 0x000000FF
#define MASK8 0x00000000000000FF

/**
 * @brief Subroutine to slice the original vector into a new byte vector
 *
 * @param bytes original bytes array
 * @param vbegin index where slicing begins
 * @param vend index where slicing ends
 * @return a new vector
 */
vec hmp221::slice(vec &bytes, int vbegin, int vend)
{
  auto start = bytes.begin() + vbegin;
  auto end = bytes.begin() + vend + 1;
  vec result(vend - vbegin + 1);
  copy(start, end, result.begin());
  return result;
}

// ----------------------------------------
// HMP221_U8
// ----------------------------------------

// Serializing a u8 is simple: push the tag onto the vector, then the byte.
vec hmp221::serialize(u8 item)
{
  vec bytes;
  bytes.push_back(HMP221_U8);
  bytes.push_back(item);
  return bytes;
}

u8 hmp221::deserialize_u8(vec bytes)
{
  // To deserialize the u8, we need at least two bytes in the serialized
  // vector: one for the tag and one for the byte.
  if (bytes.size() < 2)
  {
    throw;
  }
  // Check for the correct tag
  if (bytes[0] == HMP221_U8)
  {
    // Directly return the byte. We can do this because the return type of the
    // function is the same as the contents of the vector: u8.
    return bytes[1];
  }
  else
  {
    // Throw if the tag is not a u8
    throw;
  }
}

// ----------------------------------------
// HMP221_S8 and HMP221_S16
// ----------------------------------------

// We can handle S8 and S16 in a single function by checking the length of the
// input string. If it's fewer than 256 characters, we can output a serialized
// S8. If it's up to 2^16, we can output a serialized S16.

vec hmp221::serialize(string item)
{
  vec bytes;
  if (item.size() < 256)
  {
    bytes.push_back(HMP221_S8);
    bytes.push_back((u8)item.size());
    // Push each byte of the string onto the vector
    for (int i = 0; i < item.size(); i++)
    {
      bytes.push_back(item[i]);
    }
  }
  else if (item.size() < 65536)
  {
    bytes.push_back(HMP221_S16);
    u32 string_length = (u32)item.size();
    // Push the first byte of the length onto the vector
    bytes.push_back((u8)(string_length >> 8));
    // Push the second byte of the length onto the vector
    bytes.push_back((u8)(string_length));
    // Push each byte of the string onto the vector
    for (int i = 0; i < item.size(); i++)
    {
      bytes.push_back((u8)item[i]);
    }
  }
  else
  {
    throw;
  }
  return bytes;
}

string hmp221::deserialize_string(vec bytes)
{
  if (bytes.size() < 3)
  {
    throw;
  }
  string deserialized_string("");
  if (bytes[0] == HMP221_S8)
  {
    // The string length is byte 1
    int string_length = bytes[1];
    // The string starts at byte 2
    for (int i = 2; i < (string_length + 2); i++)
    {
      deserialized_string += bytes[i];
    }
  }
  else if (bytes[0] == HMP221_S16)
  {
    // Reconstruct the string length from bytes 1 and 2
    int string_length = (bytes[1] << 8) | bytes[2];
    // The string starts at byte 3
    for (int i = 3; i < (string_length + 3); i++)
    {
      deserialized_string += bytes[i];
    }
  }
  return deserialized_string;
}

// ----------------------------------------
// HMP221_A8 and HMP221_A16
// ----------------------------------------

// Each of the following functions will be very similar. There are two cases
// for the serialize function: one for the x8 and one for the x16 formats.
// In each function, we can leverage the serde functions written above
// to convert items and vectors from one form to another.

vec hmp221::serialize(std::vector<u8> item)
{
  vec bytes;
  if (item.size() < X8)
  {
    bytes.push_back(HMP221_A8);
    u8 size = (u8)item.size();
    bytes.push_back(size);
    for (int i = 0; i < item.size(); i++)
    {
      vec temp = serialize(item[i]);
      for (int j = 0; j < temp.size(); j++)
      {
        bytes.push_back(temp[j]);
      }
    }
  }
  else if (item.size() < X16)
  {
    bytes.push_back(HMP221_A16);
    u32 item_length = (u32)item.size();
    bytes.push_back((u8)(item_length >> 8));
    bytes.push_back((u8)(item_length));
    for (int i = 0; i < item.size(); i++)
    {
      vec elem = serialize(item[i]);
      bytes.insert(end(bytes), begin(elem), end(elem));
    }
  }
  else
  {
    throw;
  }
  return bytes;
}

std::vector<u8> hmp221::deserialize_vec_u8(vec bytes)
{
  if (bytes.size() < 3)
  {
    throw;
  }
  int el_size = 2;
  std::vector<u8> result;
  if (bytes[0] == HMP221_A8)
  {
    int size = el_size * bytes[1];
    for (int i = 2; i < (size + 2); i += el_size)
    {
      vec sub_vec = slice(bytes, i, i + el_size);
      u64 element = deserialize_u8(sub_vec);
      result.push_back(element);
    }
  }
  else if (bytes[0] == HMP221_A16)
  {
    int size = el_size * (((int)bytes[1]) << 8 | (int)bytes[2]);
    for (int i = 2; i < (size + 2); i += el_size)
    {
      vec sub_vec = slice(bytes, i + 1, i + el_size);
      u64 element = deserialize_u8(sub_vec);
      result.push_back(element);
    }
  }
  return result;
}

vec hmp221::serialize(struct Message item)
{
  vec bytes;
  bytes.push_back(HMP221_M8);
  bytes.push_back(0x1); // 1 k/v pair
  vec message = serialize((string) "Message");
  bytes.insert(end(bytes), begin(message), end(message));

  // The value is an m8
  bytes.push_back(HMP221_M8);
  bytes.push_back(0x2); // 2 k/v pairs

  // k/v 1 is "name"
  vec fileNamek = serialize((string) "name");
  bytes.insert(end(bytes), begin(fileNamek), end(fileNamek));
  vec fileNamev = serialize(item.channelName);
  bytes.insert(end(bytes), begin(fileNamev), end(fileNamev));

  // k/v 2 is "bytes"
  vec bytesk = serialize((string) "bytes");
  bytes.insert(end(bytes), begin(bytesk), end(bytesk));
  vec bytesv = serialize(item.contentBytes);
  bytes.insert(end(bytes), begin(bytesv), end(bytesv));

  return bytes;
}

struct Message hmp221::deserialize_message(vec bytes)
{
  vec file_bytes_v;
  if (bytes.size() < 5)
  {
    return {NULL, file_bytes_v};
  }
  vec file_slice = slice(bytes, 2, 10);
  string file_string = deserialize_string(file_slice);
  if (file_string.compare("Message") != 0)
  {
    return {NULL, file_bytes_v};
  }

  u8 name_len = bytes[20]; // extract the channel name
  vec namev = slice(bytes, 19, 19 + name_len + 1);
  string name = deserialize_string(namev);

  int file_length_byte = 20 + name_len + 9;
  int file_length = bytes[file_length_byte];
  int offset = 0; // = 0 when size <= 255, = 1 when size > 255
  if (bytes[file_length_byte + 512] != 42)
  {
    file_length <<= 8;
    file_length |= bytes[file_length_byte + 1];
    offset = 1;
  }

  int count = 0;
  int index = file_bytes_v.size() + 2 + offset + file_length_byte;
  while (count < file_length)
  {
    file_bytes_v.push_back(bytes[index]);
    index += 2;
    count += 1;
  }
  std::cout << name << std::endl;
  struct Message deserialized_message = {name, file_bytes_v};
  return deserialized_message;
}

vec hmp221::serialize(struct Request item)
{
  vec bytes;
  bytes.push_back(HMP221_M8);
  bytes.push_back(0x1); // 1 k/v pair
  vec file = serialize((string) "Request");
  bytes.insert(end(bytes), begin(file), end(file));

  // The value is an m8
  bytes.push_back(HMP221_M8);
  bytes.push_back(0x2); // 2 k/v pairs

  // k/v 1 is "name"
  vec fileNamek = serialize((string) "name");
  bytes.insert(end(bytes), begin(fileNamek), end(fileNamek));
  vec fileNamev = serialize(item.name);
  bytes.insert(end(bytes), begin(fileNamev), end(fileNamev));
  return bytes;
}

struct Request hmp221::deserialize_request(vec bytes)
{
  if (bytes.size() < 10)
  {
    throw;
  }
  vec file_slice = slice(bytes, 2, 10);
  string file_string = deserialize_string(file_slice);
  if (file_string != "Request")
  {
    throw;
  }
  u8 name_len = bytes[20]; // extract the length of the file name
  vec namev = slice(bytes, 19, 19 + name_len + 1);
  string name = deserialize_string(namev);
  struct Request deserialized_request = {name};
  return deserialized_request;
}

void hmp221::printVec(vec &bytes)
{
  printf("[ ");
  for (int i = 0; i < bytes.size(); i++)
  {
    printf("%x ", bytes[i]);
  }
  printf("]\n");
}
