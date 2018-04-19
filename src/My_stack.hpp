// My_stack.hpp
// Description:
//      User defined stack data structures. My_stack is used in decompression 
//      program, it can deletes an arbitrary element from the stack. 
//      My_findable_stack is used in the compression program, it can finds the 
//      internal element's position and deletes an arbitrary element from the 
//      stack.

#ifndef mystack_hpp
#define mystack_hpp

#include <iostream>
#include <iterator>
#include <cstdlib>
#include <list>
#include <map>
#include <algorithm>


/*******************************************************************************
Class: My_stack
Description:
    User defined stack data structure, it uses the c++ standard list class 
    inside. This stack can deletes an arbitrary element from the stack. This 
    user defined stack data structure is used in the decompression program.
*******************************************************************************/
template <class T>
class My_stack {
public:
    // The iterator type
    // Attention: 
    //     The push() and clear() function will invalidate the iterator
    using Const_iterator = typename std::list<T>::const_iterator;
    
    // Constructor 
    My_stack() {}
    
    // Destructor
    ~My_stack() {}
    
    // Test whether the stack is empty, returns true if the stack contains 
    // no elements
    bool empty() const;
    
    // Returns the number of the elements contained in the stack
    int size() const;
    
    // Inserts a new element at the top of the stack, above its current top element
    // Parameters: 
    //      value: Value to which the inserted element is initialized.
    void push(T& value);
    
    // Inserts a new element at the top of the stack, above its current top element
    // Parameters: 
    //      value: Value to which the inserted element is initialized.
    void push(T&& value);
    
    // Removes all elements from the stack, leaving the stack with a size of 0
    void clear();
    
    // Returns a reference to the top element in the stack
    // Returns:
    //      A reference to the top element in the stack
    T& top();
    
    // Returns a constant reference to the top element in the stack
    // Returns:
    //      A constant reference to the top element in the stack
    const T& top() const;
    
    // Removes the element on top of the stack, effectively reducing the 
    // stack's size by one
    void pop();
    
    // Returns an constant iterator pointing to the first element in the stack
    // Returns:
    //      An constant iterator that points to the first element in the stack
    Const_iterator begin() const;
    
    // Returns an constant iterator referring to the past-the-end element 
    // in the stack
    // Returns:
    //      An constant iterator to the element past the end of the sequence
    Const_iterator end() const;
    
    // Erases a single element from stack. It reduces the stack size by one
    // Parameters: 
    //      pos: An constant iterator pointing to a single element to be removed 
    //           from the list. pos must be valid
    void erase(Const_iterator pos);
    
    // Prints all the internal standard list elements to the given output stream
    // This function is only used for debugging purpose
    // Parameters: 
    //      ofile_stream: Given output stream to print all the internal 
    //                    standard list elements 
    void print(std::ostream& ofile_stream) const;
    

private:
    // The standard list used as the private member of My_stack class
    std::list<T> list;
};


/*******************************************************************************
Class: My_findable_stack
Description:
    User defined stack data structure, it use the c++ standard list class 
    and standard map class inside. This stack can finds the internal 
    element's position and deletes an arbitrary element from the stack. This 
    user defined stack data structure is used in the compression program.
*******************************************************************************/
template <class T>
class My_findable_stack {
public:
    // The iterator type
    // Attention: 
    //     The push() and clear() function will invalidate the iterator
    using Const_iterator = typename std::list<T>::const_iterator;
    
    // Constructor 
    My_findable_stack() {}
    
    // Destructor
    ~My_findable_stack() {}
    
    // Test whether the stack is empty, returns true if the stack contains 
    // no elements
    bool empty() const;
    
    // Returns the number of the elements contained in the stack
    int size() const;
    
    // Inserts a copy of value at the top of the stack
    // Parameters: 
    //      value: Value to be copied to the new element
    void push(T& value);
    
    // Removes all elements from the stack, leaving the stack with a size of 0
    void clear();
    
    // Returns a reference to the top element in the stack
    // Returns:
    //      A reference to the top element in the stack
    T& top();
    
    // Returns a constant reference to the top element in the stack
    // Returns:
    //      A constant reference to the top element in the stack
    const T& top() const;
    
    // Removes the element on top of the stack, effectively reducing the 
    // stack's size by one
    void pop();
    
    // Returns an constant iterator pointing to the first element in the stack
    // Returns:
    //      An constant iterator that points to the first element in the stack
    Const_iterator begin() const;
    
    // Returns an constant iterator referring to the past-the-end element 
    // in the stack
    // Returns:
    //      An constant iterator to the element past the end of the sequence
    Const_iterator end() const;
    
    // Returns an constant iterator to the element that compares equal to value
    // If no such element is found, the function returns the end iterator
    // Parameters: 
    //      value: Value to search for in the stack
    // Returns:
    //      An constant iterator to the element that compares equal to value. 
    //      If no elements match, the function returns last
    Const_iterator find(T value);
    
    // Erases a single element from stack. It reduces the stack size by one
    // Parameters: 
    //      pos: An constant iterator pointing to a single element to be removed 
    //           from the list. pos must be valid
    void erase(Const_iterator pos);
    
    // Prints all the internal standard list elements to the given output stream
    // This function is only used for debugging purpose
    // Parameters: 
    //      ofile_stream: Given output stream to print all the internal 
    //                    standard list elements 
    void print(std::ostream& ofile_stream) const;
    

private:
    // The standard list stores all the halfedge handles
    std::list<T> list;
    
    // The map stores halfedge handles and list iterator
    //    key = halfedge handles
    //    value = the halfedge handles' iterator in the list
    std::map<T, Const_iterator> map;
};


#include "My_stack_impl.hpp"
#endif