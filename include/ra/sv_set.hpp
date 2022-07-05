#include <algorithm>
#include <functional>
#include <memory>
#include <new>
#include <utility>

namespace ra::container {

    // A class representing a set of unique elements (which uses
    // a sorted array).
    template <class Key, class Compare = std::less<Key>>
    class sv_set {
        public:
            // Note:
            // In the time complexity specifications of various functions below,
            // it is assumed that the following operations for the Compare type
            // are constant time: default construction, destruction, copy
            // construction and assignment, and move construction and assignment.

            // A dummy type used to indicate that elements in a range are
            // both ordered and unique.
            struct ordered_and_unique_range {};

            // The type of the elements held by the container. This is
            // simply an alias for the template parameter Key.
            using value_type = Key;
            using key_type = Key;

            // The type of the function/functor used to compare two keys.
            // This is simply an alias for the template parameter Compare.
            using key_compare = Compare;

            // An unsigned integral type used to represent sizes.
            using size_type = std::size_t;

            // The mutable (random-access) iterator type for the container.
            // This type must support all of the functionality associated
            // with a random-access iterator.
            using iterator = Key*;

            // The non-mutable (random-access) const_iterator type for
            // the container.
            // This type must support all of the functionality associated
            // with a random-access iterator.
            using const_iterator = const Key*;

            // Default construct a set.
            //
            // Creates an empty set (i.e., a set containing no elements)
            // with a capacity of zero (i.e., no allocated storage for
            // elements).
            //
            // Time complexity:
            // Constant.
            sv_set() noexcept(std::is_nothrow_default_constructible_v<key_compare>) : arr_(nullptr), size_(0), capacity_(0) {}

            // Construct a set with the specified comparison object.
            //
            // Creates an empty set (i.e., a set containing no elements)
            // with a capacity of zero (i.e., no allocated storage for
            // elements). The comparison object associated with the set is
            // set to comp.
            //
            // Time complexity:
            // Constant.
            explicit sv_set(const Compare& comp) : arr_(nullptr), size_(0), capacity_(0), comp_(comp) {}

            // Construct a set from a range.
            //
            // Create a set consisting of the n elements in the range starting at
            // first, where the elements in the range must be both unique and
            // ordered with respect to the comparison operation embodied by the
            // key_compare type. If the specified range is not both ordered and
            // unique, the behavior of this function is undefined.
            // The comparison object associated with the set is set to comp.
            //
            // Time complexity:
            // Linear in n.
            //
            // Template constraints:
            // The type InputIterator must meet the requirements of an input
            // iterator.
            //
            // Note:
            // The parameter of type ordered_and_unique_range is always ignored.
            // This parameter is only present to allow for future expansion
            // (i.e., adding a constructor that does not require an ordered
            // and unique range).
            template <class InputIterator>
            sv_set(ordered_and_unique_range, InputIterator first, std::size_t n, const Compare& comp = Compare()) : sv_set() {
                arr_ = static_cast<key_type*>(::operator new(n * sizeof(key_type)));

                try {
                    std::uninitialized_copy_n(first, n, arr_);
                } catch (...) {
                    ::operator delete(arr_);
                    throw;
                }

                size_ = n;
                capacity_ = n;
                comp_ = comp;
            }

            // Move construct a set.
            //
            // Creates a new set by moving from the specified set other. After
            // construction, the source set (i.e., other) is guaranteed to be
            // empty.
            //
            // Time complexity:
            // Constant.
            sv_set(sv_set&& other) noexcept(std::is_nothrow_move_constructible_v<key_compare>) {
                arr_ = other.begin();
                size_ = other.size();
                capacity_ = other.capacity();

                other.arr_ = nullptr;
                other.size_ = 0;
                other.capacity_ = 0;
            }

            // Move assign a set.
            //
            // Assigns the value of the specified set other to *this via a move
            // operation. After the move operation, the source set (i.e., other)
            // is guaranteed to be empty.
            //
            // Iterator/reference invalidation:
            // Move assignment may invalidate iterators/references to elements
            // in the moved-from and moved-to containers.
            //
            // Time complexity:
            // Linear in size().
            //
            // Preconditions:
            // The objects *this and other are distinct.
            sv_set& operator=(sv_set&& other) noexcept(std::is_nothrow_move_assignable_v<key_compare>) {
                if (this != &other) {
                    clear();
                    ::operator delete(arr_);

                    arr_ = other.begin();
                    size_ = other.size();
                    capacity_ = other.capacity();

                    other.arr_ = nullptr;
                    other.size_ = 0;
                    other.capacity_ = 0;
                }

                return *this;
            }

            // Copy construct a set.
            //
            // Creates a new set by copying from the specified set other.
            //
            // Time complexity:
            // Linear in other.size().
            sv_set(const sv_set& other) : sv_set() {
                if (other.size() > 0) {
                    arr_ = static_cast<key_type*>(::operator new(other.size() * sizeof(key_type)));

                    try {
                        std::uninitialized_copy_n(other.begin(), other.size(), arr_);
                    } catch (...) {
                        ::operator delete(arr_);
                        throw;
                    }

                    size_ = other.size();
                    capacity_ = other.capacity();
                    comp_ = other.key_comp();
                }
            }

            // Copy assign a set.
            //
            // Assigns the value of the specified set other to *this.
            //
            // Iterator/reference invalidation:
            // Copy assignment may invalidate iterators/references to elements in
            // the copied-to container.
            //
            // Time complexity:
            // Linear in size() and other.size().
            sv_set& operator=(const sv_set& other) {
                if (this != &other) {
                    clear();
                    ::operator delete(arr_);

                    if (other.size() > 0) {
                        arr_ = static_cast<key_type*>(::operator new(other.size() * sizeof(key_type)));

                        try {
                            std::uninitialized_copy_n(other.begin(), other.size(), arr_);
                        } catch (...) {
                            ::operator delete(arr_);
                            throw;
                        }

                        size_ = other.size();
                        capacity_ = other.capacity();
                        comp_ = other.key_comp();
                    }
                }

                return *this;
            }

            // Destroy a set.
            //
            // Erases all elements in the container and destroys the container.
            //
            // Time complexity:
            // Linear in size().
            ~sv_set() {
                clear();
                ::operator delete(arr_);
            }

            // Get the comparison object for the container.
            //
            // Return value:
            // Returns the comparison object for the container.
            //
            // Time complexity:
            // Constant.
            key_compare key_comp() const noexcept {
                return comp_;
            }

            // Get an iterator referring to the first element in a set.
            //
            // Return value:
            // Returns an iterator referring to the first element in the set if
            // the set is not empty and end() otherwise.
            //
            // Time complexity:
            // Constant.
            const_iterator begin() const noexcept {
                return arr_;
            }

            iterator begin() noexcept {
                return arr_;
            }

            // Get an iterator referring to the one-past-the-end position in a
            // set.
            //
            // Return value:
            // Returns an iterator referring to the fictitious one-past-the-end
            // element for the set.
            //
            // Time complexity:
            // Constant.
            const_iterator end() const noexcept {
                return arr_ + size_;
            }

            iterator end() noexcept {
                return arr_ + size_;
            }

            // Get the size of a set.
            //
            // Return value:
            // Returns the number of elements in the set (i.e., the size
            // of the set).
            //
            // Time complexity:
            // Constant.
            size_type size() const noexcept {
                return size_;
            }

            // Get the capacity of a set.
            //
            // Return value:
            // Returns the number of elements for which storage is
            // available (i.e., the capacity of the set). This value is
            // always at least as great as size().
            //
            // Time complexity:
            // Constant.
            size_type capacity() const noexcept {
                return capacity_;
            }

            // Reserve storage for use by a set.
            //
            // Reserves storage in the container for at least n elements.
            // After this function has been called with a value of n, it
            // is guaranteed that no memory-allocation is needed as long
            // as the size of the container does not exceed n.
            // Calling this function has no effect if the capacity of the
            // container is already at least n (i.e., the capacity of
            // the container is never reduced by this function).
            //
            // Iterator/reference invalidation:
            // The reserve member function may invalidate iterators/references
            // to elements in the container if the capacity of the container is
            // increased.
            //
            // Time complexity:
            // At most linear in size().
            // TODO: why "no memory-allocation is needed as long as the size of the container does not exceed n" ?
            void reserve(size_type n) {
                if (n > capacity_) {
                    key_type* new_arr = static_cast<key_type*>(::operator new(n * sizeof(key_type)));

                    size_type old_size = size_;

                    try {
                        std::uninitialized_copy_n(arr_, size_, new_arr);
                    } catch (...) {
                        ::operator delete(new_arr);
                        throw;
                    }

                    clear();
                    ::operator delete(arr_);

                    arr_ = new_arr;
                    capacity_ = n;
                    size_ = old_size;
                }
            }

            // Minimize the amount of storage used for the elements in a set.
            //
            // Reduces the capacity of the container to the container size.
            // If the capacity of the container is greater than its size,
            // the capacity is reduced to the size of the container.
            // Calling this function has no effect if the capacity of the
            // container does not exceed its size.
            //
            // Iterator/reference invalidation:
            // The shrink_to_fit member function may invalidate
            // iterators/references to elements in the container if the capacity
            // of the container is decreased.
            //
            // Time complexity:
            // At most linear in size().
            void shrink_to_fit() {
                if (capacity_ > size_) {
                    key_type* new_arr = static_cast<key_type*>(::operator new(size_ * sizeof(key_type)));

                    size_type old_size = size_;

                    try {
                        std::uninitialized_copy_n(arr_, size_, new_arr);
                    } catch (...) {
                        ::operator delete(new_arr);
                        throw;
                    }

                    clear();
                    ::operator delete(arr_);

                    arr_ = new_arr;
                    capacity_ = old_size;
                    size_ = old_size;
                }
            }

            // Insert an element in a set.
            //
            // Inserts the element x in the set.
            // If the element x is already in the set, no insertion is
            // performed (since a set cannot contain duplicate values).
            //
            // Return value:
            // The second (i.e., boolean) component of the returned pair
            // is true if and only if the insertion takes place; and the
            // first (i.e., iterator) component of the pair refers to
            // the element with key equivalent to the key of x
            // (i.e., the element inserted if insertion took place or
            // the element found with an equal key if insertion did not
            // take place).
            //
            // Iterator/reference invalidation:
            // The insert member function may invalidate iterators/references that
            // refer to elements in the container only if an insertion is actually
            // performed (i.e., the element to be inserted is not already in the
            // container). If an insertion is performed into a container whose
            // size is less than its capacity, insert may invalidate only the
            // iterators/references that refer to elements in the container with
            // a value greater than the inserted element.
            //
            // Time complexity:
            // Search logarithmic in size() plus insertion linear in either the
            // number of elements with larger keys than x (if size() < capacity())
            // or size() (if size() == capacity()).
            std::pair<iterator, bool> insert(const key_type& x) {
                if (size_ == capacity_) {
                    iterator it = std::lower_bound(arr_, arr_ + size_, x, comp_);

                    if (it == end() || comp_(x, *it)) {
                        key_type* new_arr = static_cast<key_type*>(::operator new(capacity_ * 2 * sizeof(key_type)));

                        try {
                            std::uninitialized_move_n(arr_, it - arr_, new_arr);
                            std::uninitialized_fill_n(new_arr + (it - arr_), 1, x);
                            std::uninitialized_move_n(it, end() - it, new_arr + (it - arr_) + 1);
                        } catch (...) {
                            ::operator delete(new_arr);
                            throw;
                        }

                        it = new_arr + (it - arr_);

                        ::operator delete(arr_);

                        arr_ = new_arr;
                        ++size_;
                        capacity_ *= 2;

                        return std::make_pair(it, true);
                    } else {
                        return std::make_pair(it, false);
                    }

                } else {
                    // iterator it = arr_;
                    // for (; it != end(); ++it) {
                    //     if (x == *it) {
                    //         return std::make_pair(it, false);
                    //     }

                    //     if (comp_(x, *it)) {
                    //         for (iterator it2 = end(); it2 != it - 1; --it2) {
                    //             std::uninitialized_move_n(it2 - 1, 1, it2);
                    //         }
                    //     }
                    // }

                    // std::uninitialized_fill_n(it, 1, x);
                    // ++size_;

                    // return std::make_pair(it, true);

                    iterator it = std::lower_bound(arr_, arr_ + size_, x, comp_);

                    if (it == end() || comp_(x, *it)) {
                        for (iterator it2 = end(); it2 != it - 1; --it2) {
                            std::uninitialized_move_n(it2 - 1, 1, it2);
                        }
                        std::uninitialized_fill_n(it, 1, x);
                        ++size_;

                        return std::make_pair(it, true);
                    } else {
                        return std::make_pair(it, false);
                    }
                }
            }

            // Remove an element from a set.
            //
            // Erases the element referenced by pos from the container.
            // Returns an iterator referring to the element following the
            // erased one in the container if such an element exists or
            // end() otherwise.
            //
            // Iterator/reference invalidation:
            // The erase member function may invalidate iterators/references that
            // refer to elements in the container with a value greater than the
            // erased element.
            //
            // Time complexity:
            // Linear in number of elements with larger keys than x.
            iterator erase(const_iterator pos) {
                size_type i = pos - arr_;

                if (i < size_) {
                    std::destroy_at(pos);
                    std::uninitialized_move_n(arr_ + i + 1, size_ - i - 1, arr_ + i);
                    --size_;
                }

                return arr_ + i;
            }

            // Swap the contents of two sets.
            //
            // Swaps the contents of the container with the contents of the
            // container x.
            //
            // Iterator/reference invalidation:
            // The swap member function may invalidate iterators/references to
            // elements in both of the containers being swapped.
            //
            // Time complexity: Constant.
            void swap(sv_set& x) noexcept(std::is_nothrow_swappable_v<key_compare>) {
                std::swap(arr_, x.arr_);
                std::swap(size_, x.size_);
                std::swap(capacity_, x.capacity_);
                std::swap(comp_, x.comp_);
            }

            // Clear the contents of the set.
            //
            // Erases any elements in the container, yielding an empty container.
            //
            // Time complexity:
            // Linear in size().
            void clear() noexcept {
                std::destroy_n(arr_, size_);
                size_ = 0;
            }

            // Find an element in a set.
            //
            // Searches the container for an element with the key k.
            // If an element is found, an iterator referencing the element
            // is returned; otherwise, end() is returned.
            //
            // Time complexity:
            // Logarithmic.
            iterator find(const key_type& k) {
                return std::lower_bound(arr_, end(), k, comp_);
            }
            const_iterator find(const key_type& k) const {
                return std::lower_bound(arr_, end(), k, comp_);
            }

            // Additional Remarks
            //
            // Iterator/reference invalidation:
            // Each nonmutating (public) member function of sv_set is guaranteed
            // not to invalidate iterators/references to elements in the
            // container. The mutating (public) member functions of sv_set can
            // only invalidate iterators/references as documented herein.
            // Clearly, if an element is removed from the container, all
            // iterators/references that refer to it will be invalidated.
        private:
            key_type* arr_;
            size_type size_;
            size_type capacity_;
            key_compare comp_;
    };
}  // namespace ra::container