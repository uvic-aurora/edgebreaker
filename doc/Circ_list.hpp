// Circ_list.hpp
// Description:
//     This head file includes the circular doubly linked list data structure 
//     which is used by the decompression program.

#ifndef circlist_hpp
#define circlist_hpp

#include <iostream>


// Forward classes declaration
template <class> class Circ_list;
template <class> class Circ_list_circulator;
template <class> class Circ_list_const_circulator;


/*******************************************************************************
Class: Circ_node
Description:
    The circular doubly linked list node class.
*******************************************************************************/
template <class T>
class Circ_node {
public:    
    // Friend circular doubly linked list class
    friend class Circ_list<T>;
    
    // Friend circulator class for circular doubly linked list class 
    friend class Circ_list_circulator<T>;
    
    // Friend constant circulator class for circular doubly linked list class 
    friend class Circ_list_const_circulator<T>;

 
private: 
    // The node's data
    T data;
    
    // The previous node
    Circ_node<T>* prev;
    
    // The next node
    Circ_node<T>* next;
    
    
    // A constructor create a Circ_node object with given parameters
    // Parameters: 
    //      val: The circ_node value
    Circ_node(T val);
    
    // A constructor create a Circ_node object with given parameters
    // Parameters: 
    //      val: The circ_node value
    //      prev_node: The previous circ_node
    //      next_node: The next circ_node
    Circ_node(T val, Circ_node<T>* prev_node, Circ_node<T>* next_node);
};


/*******************************************************************************
Class: Circ_list
Description:
    The circular doubly linked list data structure which is used by the 
    decompression program. This circular doubly linked list is moveable but not 
    copyable. The type T used in decompression program is Vertex_index.
*******************************************************************************/
template <class T>
class Circ_list {
public:
    
    // Circulator and constant circulator for circular doubly linked list class 
    using Circulator = Circ_list_circulator<T>;
    using Const_circulator = Circ_list_const_circulator<T>;
    
    // Default constructor create an empty container, with no elements
    Circ_list();
    
    // A constructor create a Circ_list object with given parameters
    // Parameters: 
    //      circ_node: A doubly linked node 
    Circ_list(Circ_node<T>* node);
    
    // Move constructor
    Circ_list(Circ_list<T>&& c);

    // Move assignment operator
    Circ_list<T>& operator=(Circ_list<T>&& c);
    
    // Destructor
    ~Circ_list();

	// Test whether list is empty, returns true if the list contains no elements
    bool empty() const;
    
    // Returns the number of the elements contained in the list
    int size() const;
    
    // Removes all elements from the list and leaving the list with a size of 0
    void clear();
    
    // Inserts a copy of value at the end of the list
    // Parameters: 
    //      value: Value to be copied to the new element
    void push_back(T value);
    
    // Erases the element at position pos. It reduces the list size by one
    // Parameters: 
    //      pos: Circulator pointing to a single element to be removed from the
    //           list. pos must be a valid circulator of *this
    // Returns:
    //      An circulator pointing to the element that followed the element 
    //      erased by the function call.
    Circulator erase(Const_circulator pos);
    
    // The list is extended by inserting new element before the element at the
    // specified position. This effectively increases the list size by one
    // Parameters: 
    //      pos: Circulator pointing to a single element to be removed from the
    //           list. pos must be a valid circulator of *this
    //      value: Value to be copied to the new element
    // Returns:
    //      An circulator that points to the inserted element
    Circulator insert(Const_circulator pos, T value);
    
	// Returns an circulator pointing to an element in the list
    // Returns:
    //      An circulator that points to an element in the list
	Circulator begin();
    
    // Returns an constant circulator pointing to an element in the list
    // Returns:
    //      An constant circulator that points to an element in the list
	Const_circulator begin() const;
    
    // Prints all the internal list elements to the given output stream. 
    // This function is only used for debugging purpose
    // Parameters: 
    //      ofile_stream: Given output stream to print all the internal 
    //                    list elements 
    void print(std::ostream& ofile_stream) const;
    
    // Transfers the elements in the range [first, last) from Circ_list C into 
    // this list, inserting them before the element pointed by circulator pos
    // No destructor or copy constructors are called
    // Parameters: 
    //      pos: Circulator pointing to the position where the elements of C are 
    //           inserted. pos must point to an element contained by this list
    //      c: A list object of the same type (i.e., with the same template 
    //         parameters, T and Alloc)
    //      first, last: Circulators specifying a range of elements in C. First 
    //                   and last must point to elements contained in list C
    //      count: No. of elements to be transferred 
    void splice(Const_circulator pos, Circ_list& c, Const_circulator first, 
                Const_circulator last, int count);


private:
    // The head node of the circular doubly linked list
	Circ_node<T>* head;
    
    // The length of the list
    int length;
    
    // Copy constructor
    Circ_list(const Circ_list<T>& c) = delete;
    
    // Copy assignment operator
    Circ_list<T>& operator=(const Circ_list<T>& c) = delete;
    
};


/*******************************************************************************
Class: Circ_list_circulator
Description:
    The Circulator class of the circular doubly linked list data structure.
*******************************************************************************/
template <class T>
class Circ_list_circulator {
public:    
    // Friend circular doubly linked list class
    friend class Circ_list<T>;
    
    // Friend constant circulator class for circular doubly linked list class 
    friend class Circ_list_const_circulator<T>;
    
    // Constructor
	Circ_list_circulator() {}
    
    // A constructor create a circulator object with given parameters
    // Parameters: 
    //      node: A pointer pointing to the circular node
	Circ_list_circulator(Circ_node<T>* node) : cur_node(node) {}
    
    // Copy constructor
    Circ_list_circulator(const Circ_list_circulator& other)
      : cur_node(other.cur_node) {}
    
    // Destructor
	~Circ_list_circulator() {}
    
    // Increment operator
	Circ_list_circulator& operator++();
    
    // Decrement operator
	Circ_list_circulator& operator--();
    
    // Indirection
	T& operator*();
    
    // Assignment operator
	Circ_list_circulator& operator=(const Circ_list_circulator& other);
    
    // Test two circulator for equality
    // Returns true if two circulators are equal
	bool operator==(const Circ_list_circulator& other) const;
    
    // Test two circulator for inequality
    // Returns true if two circulators are not equal
    bool operator!=(const Circ_list_circulator& other) const;
    
    
private:
    // The circulator for the circular doubly linked list class
    Circ_node<T>* cur_node;
};


/*******************************************************************************
Class: Circ_list_const_circulator
Description:
    The constant circulator class of the circular doubly linked list data 
    structure.
*******************************************************************************/
template <class T>
class Circ_list_const_circulator {
public:
    // Friend circular doubly linked list class
    friend class Circ_list<T>;
    
    // Constructor
	Circ_list_const_circulator() {}
    
    // A constructor create a constant circulator object with given parameters
    // Parameters: 
    //      node: A pointer pointing to the circ_node  
	Circ_list_const_circulator(Circ_node<T>* node): cur_node(node) {}
    
    // A constructor create a constant circulator object with given parameters
    // Parameters: 
    //      other: A pointer pointing to the  regular circulator 
	Circ_list_const_circulator(Circ_list_circulator<T> other)
      : cur_node(other.cur_node) {}
    
    // Copy constructor
    Circ_list_const_circulator(const Circ_list_const_circulator& other)
      : cur_node(other.cur_node) {}    
    
    // Destructor
	~Circ_list_const_circulator() {}
    
    // Increment operator
	Circ_list_const_circulator& operator++();
    
    // Decrement operator
	Circ_list_const_circulator& operator--();
    
    // Dereference
	const T& operator*();
    
    // Assignment operator
	Circ_list_const_circulator& operator=(const Circ_list_const_circulator& other);
    
    // Test two constant circulator for equality
    // Returns true if two constant circulators are equal
    bool operator==(const Circ_list_const_circulator& other) const;
    
    // Test two constant circulator for inequality
    // Returns true if two constant circulators are not equal
    bool operator!=(const Circ_list_const_circulator& other) const;

    
private:
    // The constant circulator for the circular doubly linked list class
    Circ_node<T>* cur_node;
};


#include "Circ_list_test_impl.hpp"
#endif