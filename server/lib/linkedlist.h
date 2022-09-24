#include <stdio.h>
#include <stdbool.h>
#include "llnode.h"
#include <stdlib.h>

namespace linkedlist
{
    using namespace std;

    class LinkedList
    {
    private:
        linkedlist::Node *head;
        linkedlist::Node *tail;

    public:
        size_t length;
        LinkedList();
        ~LinkedList();
        size_t insertAtTail(string channel, vector<unsigned char> messageBytes);
        void printList();
        bool containsItem(string channel);
        Node *itemAtIndex(int index);
        bool replaceItem(string channel, vector<unsigned char> newContentBytes);
        vector<unsigned char> getLatestMessage(string channel);
    };

    LinkedList::LinkedList()
    {
        this->head = NULL;
        this->tail = NULL;
        this->length = 0;
    }

    LinkedList::~LinkedList()
    {
        Node *temp = this->head;
        while (temp != NULL)
        {
            Node *t = temp;
            temp = temp->next;
            free(t);
        }
    }

    size_t LinkedList::insertAtTail(string channel, vector<unsigned char> messageBytes)
    {
        Node *node = new Node(channel, messageBytes);
        if (node == NULL)
        {
            return 1;
        }
        // if list is empty.
        if (this->head == NULL)
        {
            this->head = node;
            this->tail = node;
        }
        else
        {
            this->tail->next = node;
            this->tail = this->tail->next;
        }
        this->length++;
        return 0;
    }

    bool LinkedList::containsItem(string channel)
    {
        Node *current = this->head;
        for (int i = 0; i < this->length; i++)
        {
            if (current->channel.compare(channel) == 0)
            {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool LinkedList::replaceItem(string channel, vector<unsigned char> newContentBytes)
    {
        Node *current = this->head;
        for (int i = 0; i < this->length; i++)
        {
            if (current->channel.compare(channel) == 0)
            {
                current->messageBytes = newContentBytes;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    vector<unsigned char> LinkedList::getLatestMessage(string channel)
    {
        vector<unsigned char> output;
        Node *current = this->head;
        for (int i = 0; i < this->length; i++)
        {
            if (current->channel.compare(channel) == 0)
            {
                output = current->messageBytes;
            }
            current = current->next;
        }
        return output;
    }

    Node *LinkedList::itemAtIndex(int index)
    {
        Node *current = this->head;
        for (int i = 0; i < index; i++)
        {
            if (current->next != NULL)
            {
                current = current->next;
            }
            else
            {
                exit(1);
            }
        }
        return current;
    }
}
