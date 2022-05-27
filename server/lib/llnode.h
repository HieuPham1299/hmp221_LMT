#include <stdio.h>
#include <string.h>
#include <iostream>

namespace linkedlist {

    using namespace std;

    class Node {
        public:
            string channel;
            vector<unsigned char> messageBytes;
            linkedlist::Node* next;
            Node(string channel);
            Node(string channel, vector<unsigned char> messageBytes);
            ~Node();
    };

    Node::Node(string channel)
    {
        vector<unsigned char> bytes;
        Node(channel, bytes);
    }

    Node::Node(string channel, vector<unsigned char> messageBytes)
    {
        this->channel = channel;
        this->messageBytes = messageBytes;
        this->next = NULL;
    }
    
    Node::~Node()
    {

    }
}
