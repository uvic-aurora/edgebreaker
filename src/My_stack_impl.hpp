#include <cassert>
#include "My_stack.hpp"



//////////////////////////////////////////////////////////
// Function implementation for My_stack class 
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: bool empty() const;
Description:
    Test whether the stack is empty, returns true if the stack contains no elements
*******************************************************************************/
template <class T>
bool My_stack<T>::empty() const {
    return list.empty();
}


/*******************************************************************************
Function: int size() const;
Description:
    Returns the number of the elements contained in the stack
*******************************************************************************/
template <class T>
int My_stack<T>::size() const {
    return list.size();
}


/*******************************************************************************
Function: void push(T& value);
Description:
    Inserts a new element at the top of the stack, above its current top element
*******************************************************************************/
template <class T>
void My_stack<T>::push(T& value) {
    list.push_back(std::move(value));
}


/*******************************************************************************
Function: void push(T&& value);
Description:
    Inserts a new element at the top of the stack, above its current top element
*******************************************************************************/
template <class T>
void My_stack<T>::push(T&& value) {
    list.push_back(std::move(value));
}


/*******************************************************************************
Function: void clear();
Description:
    Removes all elements from the stack, leaving the stack with a size of 0
*******************************************************************************/
template <class T>
void My_stack<T>::clear() {
    list.clear();
}


/*******************************************************************************
Function: T& top();
Description:
    Returns a reference to the top element in the stack
*******************************************************************************/
template <class T>
T& My_stack<T>::top() {
    return list.back(); 
}


/*******************************************************************************
Function: const T& top() const;
Description:
    Returns a reference to the top element in the stack
*******************************************************************************/
template <class T>
const T& My_stack<T>::top() const {
    return list.back(); 
}


/*******************************************************************************
Function: void pop();
Description:
    Removes the element on top of the stack, effectively reducing its size by one
*******************************************************************************/
template <class T>
void My_stack<T>::pop() {
    list.pop_back();
}


/*******************************************************************************
Function: Const_iterator begin() const;
Description:
    Returns an iterator pointing to the first element in the stack
*******************************************************************************/
template <class T>
typename My_stack<T>::Const_iterator My_stack<T>::begin() const {
    return list.begin();
}


/*******************************************************************************
Function: Const_iterator end() const;
Description:
    Returns an iterator referring to the past-the-end element in the stack
*******************************************************************************/
template <class T>
typename My_stack<T>::Const_iterator My_stack<T>::end() const {
    return list.end();
}


/*******************************************************************************
Function: void erase(Const_iterator pos);
Description:
    Erases a single element from stack. It reduces the stack size by one
*******************************************************************************/
template <class T> 
void My_stack<T>::erase(Const_iterator pos) {
    list.erase(pos);
}


/*******************************************************************************
Function: void print(std::ostream& ofile_stream) const;
Description:
    Prints all the internal standard list elements to the given output 
    stream. This function is only used for debugging purpose.
*******************************************************************************/
template <class T>
void My_stack<T>::print(std::ostream& ofile_stream) const {
    for(auto iter = list.begin(); iter != list.end(); ++iter) {
        ofile_stream << *iter << " ";
    }
    ofile_stream << "\n\n";
}


//////////////////////////////////////////////////////////
// Function implementation for My_findable_stack class 
//////////////////////////////////////////////////////////

/*******************************************************************************
Function: bool empty() const;
Description:
    Test whether the stack is empty, returns true if the stack contains no elements
*******************************************************************************/
template <class T>
bool My_findable_stack<T>::empty() const {
    assert(list.empty() == map.empty());
    return list.empty();
}


/*******************************************************************************
Function: int size() const;
Description:
    Returns the number of the elements contained in the stack
*******************************************************************************/
template <class T>
int My_findable_stack<T>::size() const {
    assert(list.size() == map.size());
    return list.size();
}


/*******************************************************************************
Function: void push(T& value);
Description:
    Inserts a copy of value at the top of the stack
*******************************************************************************/
template <class T>
void My_findable_stack<T>::push(T& value) {
    list.push_back(value);
    
    Const_iterator value_iter = std::find(list.begin(), list.end(), value);
    map.insert(std::pair<T, Const_iterator>(value, value_iter));
}


/*******************************************************************************
Function: void clear();
Description:
    Removes all elements from the stack, leaving the stack with a size of 0
*******************************************************************************/
template <class T>
void My_findable_stack<T>::clear() {
    list.clear();
    map.clear();
}


/*******************************************************************************
Function: T& top();
Description:
    Returns a reference to the top element in the stack
*******************************************************************************/
template <class T>
T& My_findable_stack<T>::top() {
    assert(map.find(list.back()) != map.end());
    return list.back(); 
}


/*******************************************************************************
Function: const T& top() const;
Description:
    Returns a reference to the top element in the stack
*******************************************************************************/
template <class T>
const T& My_findable_stack<T>::top() const {
    assert(map.find(list.back()) != map.end());
    return list.back(); 
}


/*******************************************************************************
Function: void pop();
Description:
    Removes the element on top of the stack, effectively reducing its size by one
*******************************************************************************/
template <class T>
void My_findable_stack<T>::pop() {
    T key = list.back();
    map.erase(key);
    list.pop_back();
}


/*******************************************************************************
Function: Const_iterator begin() const;
Description:
    Returns an iterator pointing to the first element in the stack
*******************************************************************************/
template <class T>
typename My_findable_stack<T>::Const_iterator My_findable_stack<T>::begin() const {
    assert(map.find(*(list.begin())) != map.end());
    return My_findable_stack<T>::Const_iterator(list.begin());
}


/*******************************************************************************
Function: Const_iterator end() const;
Description:
    Returns an iterator referring to the past-the-end element in the stack
*******************************************************************************/
template <class T>
typename My_findable_stack<T>::Const_iterator My_findable_stack<T>::end() const {
    return My_findable_stack<T>::Const_iterator(list.end());
}


/*******************************************************************************
Function: Const_iterator find(T value);
Description:
    Returns an iterator to the element that compares equal to value
    If no such element is found, the function returns the end iterator
*******************************************************************************/
template <class T>
typename My_findable_stack<T>::Const_iterator My_findable_stack<T>::find(T value) {
    typename std::map<T, Const_iterator>::iterator map_iter = map.find(value);
    
    if(map_iter == map.end()) {
        return list.end();
    }
    
    Const_iterator list_pos = map_iter -> second;
    return list_pos;
}


/*******************************************************************************
Function: void erase(Const_iterator pos);
Description:
    Erases a single element from stack. It reduces the stack size by one
*******************************************************************************/
template <class T> 
void My_findable_stack<T>::erase(Const_iterator pos) {
    T value = *pos;
    list.erase(pos);
    map.erase(value);
}


/*******************************************************************************
Function: void print(std::ostream& ofile_stream) const;
Description:
    Prints all the internal standard list elements to the given output 
    stream. This function is only used for debugging purpose.
*******************************************************************************/
template <class T>
void My_findable_stack<T>::print(std::ostream& ofile_stream) const {
    ofile_stream << "----------------------------------------\n";
    ofile_stream << "Standard list part:\n";
    for(auto iter = list.begin(); iter != list.end(); ++iter) {
        ofile_stream << *iter << " ";
    }
    ofile_stream << "\n\n";

    ofile_stream << "Standard map part:\n";
    for(auto iter = map.begin(); iter != map.end(); ++iter) {
        ofile_stream << iter -> first << " " << *(iter -> second) << "\n";
    }
    ofile_stream << "----------------------------------------\n";
}

