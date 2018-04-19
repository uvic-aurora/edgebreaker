#include <iostream>
#include <algorithm>
#include <cassert>
#include "Circ_list.hpp"

//////////////////////////////////////////////////////////
// A doubly linked node class 
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: Circ_node(T val);
Description:
    Circ_list class constructor
*******************************************************************************/
template <class T>
Circ_node<T>::Circ_node(T val) : data(val), prev(nullptr), next(nullptr) {}


/*******************************************************************************
Function: Circ_node(T val, Circ_node<T>* prev_node, Circ_node<T>* next_node);
Description:
    Circ_list class constructor
*******************************************************************************/
template <class T>
Circ_node<T>::Circ_node(T val, Circ_node<T>* prev_node, Circ_node<T>* next_node)
  : data(val), prev(prev_node), next(next_node) {}



//////////////////////////////////////////////////////////
// A circular doubly linked list class 
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: Circ_list();
Description:
    Circ_list class default constructor
*******************************************************************************/
template <class T>
Circ_list<T>::Circ_list() : head(nullptr), length(0) {}


/*******************************************************************************
Function: Circ_list(Circ_node<T>* node);
Description:
    Circ_list class constructor
*******************************************************************************/
template <class T>
Circ_list<T>::Circ_list(Circ_node<T>* node) : head(node) {
    Circ_node<T>* current = head -> next;
    
    // Update the list length
    length = 1;
    while (current != head) {
        ++length;
        current = current -> next;
    }
}


/*******************************************************************************
Function: Circ_list(const Circ_list<T>& c);
Description:
    Circ_list class copy constructor
*******************************************************************************/
template <class T>
Circ_list<T>::Circ_list(const Circ_list<T>& c) : head(c.head), length(c.length) {}


/*******************************************************************************
Function: Circ_list(Circ_list<T>&& c);
Description:
    Circ_list class move constructor
*******************************************************************************/
template <class T>
Circ_list<T>::Circ_list(Circ_list<T>&& c) : head(nullptr), length(0) {
    // Swap the current list with list c
    std::swap(head, c.head);
    std::swap(length, c.length);
}


/*******************************************************************************
Function: Circ_list<T>& operator=(Circ_list<T>&& c);
Description:
    Circ_list class move assignment operator
*******************************************************************************/
template <class T>
Circ_list<T>& Circ_list<T>::operator=(Circ_list<T>&& c) {
    // Clear current list
    head = nullptr;
    length = 0;
    
    // Swap the current list with list c
    std::swap(head, c.head);
    std::swap(length, c.length);
}


/*******************************************************************************
Function: ~Circ_list();
Description:
    Circ_list class destructor
*******************************************************************************/
template <class T>
Circ_list<T>::~Circ_list() {
    // clear();
    
    // if (length == 0) {
        // return;
    // }
    
    // Circ_node<T>* current = head;
    // while (length != 1) {
        // Circ_node<T>* next_node = current -> next;
        
        // next_node -> prev = current -> prev;
        // current -> prev -> next = current;
        
        // delete current;
        // --length;
        
        // current = next_node;
    // }
    
    // current = nullptr;
    // length = 0;
}


/*******************************************************************************
Function: bool empty() const;
Description:
    Test whether list is empty, returns true if the list contains no elements
*******************************************************************************/
template <class T>
bool Circ_list<T>::empty() const {
    return length == 0;
}


/*******************************************************************************
Function: int size() const;
Description:
    Returns the number of the elements contained in the list
*******************************************************************************/
template <class T>
int Circ_list<T>::size() const {
    return length;
}


/*******************************************************************************
Function: void clear();
Description:
    Removes all elements from the list and leaving the list with a size of 0
*******************************************************************************/
template <class T>
void Circ_list<T>::clear() {
    if (length == 0) {
        // List is empty
        head = nullptr;
        return;
    }
    else {
        Circ_node<T>* del_node = head;
        
        while (length != 1) {
            Circ_node<T>* next_node = del_node -> next;
            
            next_node -> prev = del_node -> prev;
            del_node -> prev -> next = next_node;
            
            delete del_node;
            --length;

            del_node = next_node;
        }
        
        delete del_node;
        
        assert(length == 1);
        --length;
        
        head = nullptr;
    }
}


/*******************************************************************************
Function: void push_back(T value);
Description:
    Inserts a copy of value at the end of the list
*******************************************************************************/
template <class T>
void Circ_list<T>::push_back(T value) {
    if (length == 0) {
        // Push back to an empty list
        head = new Circ_node<T>(value);
        head -> prev = head -> next = head;
        ++length;
    }
    else {
        Circ_node<T>* insert_node = new Circ_node<T>(value);
        insert_node -> prev = head -> prev;
        insert_node -> next = head;
        head -> prev -> next = insert_node;
        head -> prev = insert_node;
        ++length;
    }
}

/*******************************************************************************
Function: void erase(Const_circulator pos);
Description:
    Erases the element at position pos. It reduces the list size by one.
*******************************************************************************/
template <class T>
typename Circ_list<T>::Circulator Circ_list<T>::erase(Const_circulator pos) {
    if (length == 0) {
        std::cerr << "ERROR: Empty list!\n";
    }
    else if (length == 1) {
        if (pos.cur_node == head) {
            head = nullptr;
            length = 0;
            
            return Circ_list_circulator<T>(head);
        }
        else {
            std::cerr << "ERROR: Invalid erase position!\n";
        }
    }
    
    Circ_node<T>* current = pos.cur_node; 
    Circ_node<T>* return_node = current -> next; 
    
    if (current == head) {
        head -> prev -> next = return_node;
        return_node -> prev = head -> prev;
        
        head = return_node;
    }
    else {
        current -> prev -> next = return_node;
        return_node -> prev = current -> prev;
    }
    
    delete current;
    --length;
    
    return Circ_list_circulator<T>(return_node);
}


/*******************************************************************************
Function: Circulator insert(Const_circulator pos, T value);
Description:
    The list is extended by inserting new elements before the element at the
    specified position. This effectively increases the list size by one.
*******************************************************************************/
template <class T>
typename Circ_list<T>::Circulator Circ_list<T>::insert(Const_circulator pos, T value) {
    // Insert into empty list
    if (length == 0) {
        // Ignore the pos parameter
        head = new Circ_node<T>(value);
        head -> prev = head -> next = head;
        ++length;
        
        return Circ_list_circulator<T>(head);
    }
    
    // Insert into non-empty list
    Circ_node<T>* current = pos.cur_node; 
    Circ_node<T>* insert_node = new Circ_node<T>(value);
    
    if (current == head) {
        insert_node -> prev = head -> prev;
        insert_node -> next = head;
        head -> prev -> next = insert_node;
        head -> prev = insert_node;
        
        ++length;
        // head = head -> prev;
        
        return Circ_list_circulator<T>(head);
    }
    else {
        // Update links
        insert_node -> prev = current -> prev;
        insert_node -> next = current;
        current -> prev -> next = insert_node;
        current -> prev = insert_node;
        
        ++length;
        current = current -> prev;

        return Circ_list_circulator<T>(current); 
    }
}


/*******************************************************************************
Function: Circulator begin();
Description:
    Returns an circulator pointing to the first element in the list
*******************************************************************************/
template <class T>
typename Circ_list<T>::Circulator Circ_list<T>::begin() {
    return Circ_list_circulator<T>(head);
}


/*******************************************************************************
Function: Const_circulator begin() const;
Description:
    Returns an constant circulator pointing to the first element in the list
*******************************************************************************/
template <class T>
typename Circ_list<T>::Const_circulator Circ_list<T>::begin() const {
    return Circ_list_const_circulator<T>(head);
}


/*******************************************************************************
Function: void print(std::ostream& ofile_stream) const;
Description:
    Prints all the internal boost.container list elements to the given 
    output stream. This function is only used for debugging purpose.
*******************************************************************************/
template <class T>
void Circ_list<T>::print(std::ostream& ofile_stream) const {
    if (length == 0) {
        return;
    }
    
    Circ_node<T>* current = head;
    ofile_stream << current -> data << " ";
    
    while(current -> next != head) {
        current = current -> next;
        ofile_stream << current -> data << " ";
    }
    
    ofile_stream << "\n";
}


/*******************************************************************************
Function: void splice(Const_circulator pos, Circ_list& c, Const_circulator first, 
                      Const_circulator last, int count);
Description:
    Transfers the elements in the range [first, last) from Circ_list C into 
    this list, inserting them before the element pointed by circulator pos. 
*******************************************************************************/
template <class T>
void Circ_list<T>::splice(Const_circulator pos, Circ_list& c, Const_circulator first, 
Const_circulator last, int count) {
    Circ_node<T>* end = last.cur_node -> prev;
    
    // Update inside c list
    first.cur_node -> prev -> next = last.cur_node;
    last.cur_node -> prev = first.cur_node -> prev;
    c.length -= count;
    c.head = last.cur_node;
    
    // Update the current list
    if (length == 0) {
        // Splice into an empty list
        head = first.cur_node;
        first.cur_node -> prev = end;
        end -> next = first.cur_node;
        length += count;
        
        return;
    }
    
    first.cur_node -> prev = pos.cur_node -> prev;
    end -> next = pos.cur_node;
    pos.cur_node -> prev -> next = first.cur_node;
    pos.cur_node -> prev = end;
    length += count;
    
    // Update the head node
    if(pos.cur_node == head) {
        head = first.cur_node;
    }
}


//////////////////////////////////////////////////////////
// Circulator class for the Circ_list class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: Circ_list_circulator& operator++();
Description:
    Increment operator
*******************************************************************************/
template <class T>
Circ_list_circulator<T>& Circ_list_circulator<T>::operator++() {
    cur_node = cur_node -> next;
    return *this;
}


/*******************************************************************************
Function: Circ_list_circulator& operator--();
Description:
    Decrement operator
*******************************************************************************/
template <class T>
Circ_list_circulator<T>& Circ_list_circulator<T>::operator--() {
    cur_node = cur_node -> prev;
    return *this;
}


/*******************************************************************************
Function: T& operator*();
Description:
    Indirection
*******************************************************************************/
template <class T>
T& Circ_list_circulator<T>::operator*() {
    return cur_node -> data;
}


/*******************************************************************************
Function: Circ_list_circulator& operator=(const Circ_list_circulator& other);
Description:
    Assignment operator
*******************************************************************************/
template <class T>
Circ_list_circulator<T>& Circ_list_circulator<T>::operator=(const Circ_list_circulator& other) {
    cur_node = other.cur_node; 
}


/*******************************************************************************
Function: bool operator==(const Circ_list_circulator& other) const;
Description:
    Test two circulator for equality
*******************************************************************************/
template <class T>
bool Circ_list_circulator<T>::operator==(const Circ_list_circulator<T>& other) const {
    return this -> cur_node == other.cur_node;
}


/*******************************************************************************
Function: bool operator!=(const Circ_list_circulator& other) const;
Description:
    Test two circulator for inequality
*******************************************************************************/
template <class T>
bool Circ_list_circulator<T>::operator!=(const Circ_list_circulator<T>& other) const {
    return this -> cur_node != other.cur_node;
}


//////////////////////////////////////////////////////////
// Constant circulator class for the Circ_list class
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: Circ_list_const_circulator& operator++();
Description:
    Increment operator
*******************************************************************************/
template <class T>
Circ_list_const_circulator<T>& Circ_list_const_circulator<T>::operator++() {
    cur_node = cur_node -> next;
    return *this;
}


/*******************************************************************************
Function: Circ_list_const_circulator& operator--();
Description:
    Decrement operator
*******************************************************************************/
template <class T>
Circ_list_const_circulator<T>& Circ_list_const_circulator<T>::operator--() {
    cur_node = cur_node -> prev;
    return *this;
}


/*******************************************************************************
Function: const T& operator*();
Description:
    Dereference
*******************************************************************************/
template <class T>
const T& Circ_list_const_circulator<T>::operator*() {
    return cur_node -> data;
}


/*******************************************************************************
Function: Circ_list_const_circulator& operator=(const Circ_list_const_circulator& other);
Description:
    Assignment operator
*******************************************************************************/
template <class T>
Circ_list_const_circulator<T>& Circ_list_const_circulator<T>::operator=(const Circ_list_const_circulator<T>& other) {
    cur_node = other.cur_node; 
}


/*******************************************************************************
Function: bool operator==(const Circ_list_const_circulator& other) const;
Description:
    Test two circulator for equality
*******************************************************************************/
template <class T>
bool Circ_list_const_circulator<T>::operator==(const Circ_list_const_circulator<T>& other) const {
    return this -> cur_node == other.cur_node;
}


/*******************************************************************************
Function: bool operator!=(const Circ_list_const_circulator& other) const;
Description:
    Test two circulator for inequality
*******************************************************************************/
template <class T>
bool Circ_list_const_circulator<T>::operator!=(const Circ_list_const_circulator<T>& other) const {
    return this -> cur_node != other.cur_node;
}

