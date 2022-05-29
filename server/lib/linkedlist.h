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
        // size_t insertAtHead(string item, FILE *fileContent);
        // size_t insertAtIndex(size_t index, T item);
        // T removeTail();
        void printList();
        // T removeHead();
        // T removeAtIndex(size_t index);
        bool containsItem(string channel);
        Node *itemAtIndex(int index);
        bool replaceItem(string channel, vector<unsigned char> newContentBytes);
        // remove(string item);
        vector<unsigned char> getLatestMessage(string channel);
    };

    LinkedList::LinkedList()
    {
        // head = new Node();
        // tail = new Node();
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
        Node *current = this->head;
        for (int i = 0; i < this->length; i++)
        {
            if (current->channel.compare(channel) == 0)
            {
                return current->messageBytes;
            }
            current = current->next;
        }
        perror("No message published!");
    }

    // template <class T>
    // size_t LinkedList::insertAtHead(T item)
    // {
    //     Node* node = new Node(item);
    //     if (node == NULL) {
    //         return 1;
    //     }
    //     //if list is empty.
    //     if(this->head == NULL) {
    //         this->head = node;
    //         this->tail = node;
    //     } else {
    //         node->next 	= this->head;
    //         this->head 	= node;
    //     }
    //     this->length++;
    //     return 0;
    // }

    // template <class T>
    // size_t LinkedList::insertAtIndex(size_t index, T item)
    // {
    //     if(index==0)
    //     {
    //         return insertAtHead(item);
    //     }
    //     Node* current = this->head;
    //     Node* items = new Node(item);
    //     if(items==NULL)
    //     {
    //         return 1;
    //     }
    //     for(int i =0;i<index-1;i++)
    //     {
    //         current = current->next;
    //     }
    //     items->next = current->next;
    //     current->next = items;
    //     return 0;
    // }

    // template <class T>
    // T LinkedList::removeTail()
    // {
    //     Node* temp;
    //     int i = 0;
    //     T item;
    //     if(this->tail == NULL)
    //     {
    //         // List is Empty
    //         return 0;
    //     }
    //     else
    //     {
    //         temp = this->head;

    //         // Iterate to the end of the list
    //         while(temp->next != this->tail) {
    //             temp = temp->next;
    //         }

    //         item = this->tail->item;

    //         Node* old_tail = this->tail;
    //         this->tail = temp;
    //         this->tail->next = NULL;
    //         free(old_tail);
    //     }
    //     this->length--;
    //     return item;
    // }

    // template <class T>
    // T LinkedList::removeHead()
    // {
    //     T item;
    //     if(this->head == NULL)
    //     {
    //         // List is Empty
    //         return 0;
    //     }
    //     else
    //     {
    //         item = this->head->item;
    //         Node* old_head = this->head;
    //         this->head = this->head->next;
    //         free(old_head);
    //     }
    //     this->length--;
    //     return item;
    // }

    // template<class T>
    // T LinkedList::remove(T item)
    // {
    //     int index = -1;
    //     T temp = 0;
    //     Node* current = this->head;
    //     for(int i=0;i<this->length;i++)
    //     {
    //         index++;
    //         if(current->item==item)
    //         {
    //             temp = this->removeAtIndex(index);
    //             return temp;
    //         }
    //         current = current->next;
    //         if(current==NULL)
    //         {
    //             break;
    //         }
    //     }
    //     return temp;
    // }

    // template <class T>
    // T LinkedList::removeAtIndex(size_t index)
    // {
    //     if (index < 0 || index >= this->length)
    //     {
    //         return 0;
    //     }
    //     if (index == 0)
    //     {
    //         return removeHead();
    //     }
    //     if (index == length - 1)
    //     {
    //         return removeTail();
    //     }
    //     Node* cN = this->head;
    //     size_t limit = this->length;
    //     for (size_t i = 0; i < limit; i++)
    //     {
    //         if (i + 1 == index)
    //         {
    //             T returnVal = cN->next->item;
    //             free(cN->next);
    //             cN->next = cN->next->next;
    //             this->length--;
    //             return returnVal;
    //         }
    //     }
    //     return 0;
    // }

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

    void LinkedList::printList()
    {
        size_t limit = this->length;
        if (!this->length)
        {
            printf("Empty");
        }
        Node *cP = this->head;
        for (size_t i = 0; i < limit; i++)
        {
            printf("%s -> ", cP->channel);
            cP = cP->next;
        }
    }

}
