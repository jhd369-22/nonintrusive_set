#include <ra/parent_from_member.hpp>

namespace ra::intrusive {

// Per-node list management information class.
// This type contains per-node list management information (i.e., the
// successor and predecessor in the list). This class has the list class
// template as a friend. This type must contain pointers (of type
// list_hook*) to the next and previous node in the list.
class list_hook {
public:

// Default construct a list hook.
// This constructor creates a list hook that does not belong to any
// list.
list_hook();

// Copy construct a list hook.
// This constructor creates a list hook that does not belong to any
// list. The argument to the constructor is ignored. The copy
// construction operation is defined only so that types with list hooks
// are copy constructible. The list class itself never copies (or
// moves) a list hook.
list_hook(const list_hook&);

// Copy assign a list hook.
// The copy assignment operator is defined as a no-op. The argument to
// the operator is ignored. The copy assignment operation is defined
// only so that types with list hooks are copy assignable. The list
// class itself never copies (or moves) a list hook.
list_hook& operator=(const list_hook&);

// Destroy a list hook.
// The list hook being destroyed must not belong to a list. If the
// list hook belongs to a list, the resulting behavior is undefined.
~list_hook();
};

// Intrusive doubly-linked list (with sentinel node).
template <class T, list_hook T::* Hook>
class list {
public:

// The type of the elements in the list.
using value_type = T;

// The pointer-to-member associated with the list hook object.
static constexpr list_hook T::* hook_ptr = Hook;

// The type of a mutating reference to a node in the list.
using reference = T&;

// The type of a non-mutating reference to a node in the list.
using const_reference = const T&;

// The mutating (bidirectional) iterator type for the list. This type
// must provide all of the functionality of a bidirectional iterator.
// If desired, the Boost Iterator library may be used to implement
// this type.
using iterator = /* implementation defined */;

// The non-mutating (bidirectional) iterator type for the list. This
// type must provide all of the functionality of a bidirectional
// iterator. If desired, the Boost Iterator library may be used to
// implement this type.
using const_iterator = /* implementation defined */;

// An unsigned integral type used to represent sizes.
using size_type = std::size_t;

// Default construct a list.
//
// Creates an empty list.
//
// Time complexity:
// Constant.
list();

// Destroy a list.
//
// Erases any elements from the list and then destroys the list.
//
// Time complexity:
// Either linear or constant.
~list();

// Move construct a list.
//
// The elements in the source list (i.e., other) are moved from the
// source list to the destination list (i.e., *this), preserving their
// relative order. After the move, the source list is empty.
//
// Time complexity:
// Constant.
list(list&& other);

// Move assign a list.
//
// The elements of the source list (i.e., other) are swapped with the
// elements of the destination list (i.e., *this). The relative order
// of the elements in each list is preserved.
//
// Precondition:
// The objects *this and other are distinct.
//
// Time complexity:
// Constant.
list& operator=(list&& other);

// Do not allow the copying of lists.
list(const list&) = delete;
list& operator=(const list&) = delete;

// Swap the elements of two lists.
//
// Swaps the elements of *this and x.
// Swapping the elements of a list with itself has no effect.
//
// Time complexity:
// Constant.
void swap(list& x);

// Returns the number of elements in the list.
//
// Time complexity:
// Constant.
size_type size() const;

// Inserts an element in the list before the element referred to
// by the iterator pos.
// An iterator that refers to the inserted element is returned.
//
// Time complexity:
// Constant.
iterator insert(iterator pos, value_type& value);

// Erases the element in the list at the position specified by the
// iterator pos.
// An iterator that refers to the element following the erased element
// is returned if such an element exists; otherwise, end() is
// returned.
//
// Time complexity:
// Constant.
iterator erase(iterator pos);

// Inserts the element with the value x at the end of the list.
//
// Time complexity:
// Constant.
void push_back(value_type& x);

// Erases the last element in the list.
//
// Precondition:
// The list is not empty.
//
// Time complexity:
// Constant.
void pop_back();

// Returns a reference to the last element in the list.
//
// Precondition:
// The list is not empty.
//
// Time complexity:
// Constant.
reference back();
const_reference back() const;

// Erases any elements from the list, yielding an empty list.
//
// Time complexity:
// Either linear or constant.
void clear();

// Returns an iterator referring to the first element in the list
// if the list is not empty and end() otherwise.
//
// Time complexity:
// Constant.
const_iterator begin() const;
iterator begin();

// Returns an iterator referring to the fictitious one-past-the-end
// element.
//
// Time complexity:
// Constant.
const_iterator end() const;
iterator end();

};
}