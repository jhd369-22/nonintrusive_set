#include <boost/iterator/iterator_facade.hpp>
#include <ra/parent_from_member.hpp>
#include <type_traits>

namespace ra::intrusive {
    class list_hook;
    template <class T, list_hook T::*Hook>
    class list;

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
            list_hook() : next_(nullptr), prev_(nullptr) {}

            // Copy construct a list hook.
            // This constructor creates a list hook that does not belong to any
            // list. The argument to the constructor is ignored. The copy
            // construction operation is defined only so that types with list hooks
            // are copy constructible. The list class itself never copies (or
            // moves) a list hook.
            list_hook(const list_hook&) : next_(nullptr), prev_(nullptr) {}

            // Copy assign a list hook.
            // The copy assignment operator is defined as a no-op. The argument to
            // the operator is ignored. The copy assignment operation is defined
            // only so that types with list hooks are copy assignable. The list
            // class itself never copies (or moves) a list hook.
            list_hook& operator=(const list_hook&) { return *this; }

            // Destroy a list hook.
            // The list hook being destroyed must not belong to a list. If the
            // list hook belongs to a list, the resulting behavior is undefined.
            ~list_hook() {
                next_ = nullptr;
                prev_ = nullptr;
            }

        private:
            // The next node in the list.
            list_hook* next_;
            // The previous node in the list.
            list_hook* prev_;

            // Friend the list class template.
            template <typename T, list_hook T::*H>
            friend class list;

            // Friend the list_iterator class template.
            template <class P>
            friend class list_iterator;
    };

    // List iterator class.
    // This class provides a forward iterator for lists. The iterator is
    // bidirectional and can be used to traverse a list in either direction.
    // The iterator is a random access iterator.
    template <typename P>
    class list_iterator : public boost::iterator_facade<list_iterator<P>, P, boost::bidirectional_traversal_tag> {
        public:
            template <class Other_ptr>
            requires std::is_convertible_v<Other_ptr*, P*>
            list_iterator(const list_iterator<Other_ptr>& other) : node_(other.node_) {}

            using typename boost::iterator_facade<list_iterator<P>, P,
                                                  boost::bidirectional_traversal_tag>::reference;
            using typename boost::iterator_facade<list_iterator<P>, P,
                                                  boost::bidirectional_traversal_tag>::difference_type;

            // Construct a list iterator.
            explicit list_iterator(P* node = nullptr) : node_(node) {}

            // Copy construct a list iterator.
            list_iterator(const list_iterator& other) = default;

            // Copy assign a list iterator.
            list_iterator& operator=(const list_iterator& other) = default;

            // Dereference the list iterator.
            // This operator returns a reference to the node pointed to by the
            // list iterator.
            P& operator*() const {
                return *node_;
            }

            // Dereference the list iterator.
            // This operator returns a pointer to the node pointed to by the
            // list iterator.
            P* operator->() const {
                return node_;
            }

            // Compare two list iterators.
            bool operator==(const list_iterator& other) const {
                return node_ == other.get_node();
            }

            // Compare two list iterators.
            bool operator!=(const list_iterator& other) const {
                return node_ != other.get_node();
            }

            // prefix increment.
            list_iterator& operator++() {
                node_ = node_->next_;
                return *this;
            }

            // postfix increment.
            list_iterator operator++(int) {
                list_iterator tmp(*this);
                node_ = node_->next_;
                return tmp;
            }

            // prefix decrement.
            list_iterator& operator--() {
                node_ = node_->prev_;
                return *this;
            }

            // postfix decrement.
            list_iterator operator--(int) {
                list_iterator tmp(*this);
                node_ = node_->prev_;
                return tmp;
            }

            P* get_node() const {
                return node_;
            }

        private:
            template <class>
            friend class list_iterator;

            friend class boost::iterator_core_access;

            // The node pointed to by the list iterator.
            P* node_;
    };

    // Intrusive doubly-linked list (with sentinel node).
    template <class T, list_hook T::*Hook>
    class list {
        public:
            // The type of the elements in the list.
            using value_type = T;

            // The pointer-to-member associated with the list hook object.
            static constexpr list_hook T::*hook_ptr = Hook;

            // The type of a mutating reference to a node in the list.
            using reference = T&;

            // The type of a non-mutating reference to a node in the list.
            using const_reference = const T&;

            // The mutating (bidirectional) iterator type for the list. This type
            // must provide all of the functionality of a bidirectional iterator.
            // If desired, the Boost Iterator library may be used to implement
            // this type.
            using iterator = list_iterator<T>;

            // The non-mutating (bidirectional) iterator type for the list. This
            // type must provide all of the functionality of a bidirectional
            // iterator. If desired, the Boost Iterator library may be used to
            // implement this type.
            using const_iterator = list_iterator<const T>;

            // An unsigned integral type used to represent sizes.
            using size_type = std::size_t;

            // Default construct a list.
            //
            // Creates an empty list.
            //
            // Time complexity:
            // Constant.
            list() {
                sentinel_.next_ = &sentinel_;
                sentinel_.prev_ = &sentinel_;

                size_ = 0;
            }

            // Destroy a list.
            //
            // Erases any elements from the list and then destroys the list.
            //
            // Time complexity:
            // Either linear or constant.
            ~list() {
                clear();
            }

            // Move construct a list.
            //
            // The elements in the source list (i.e., other) are moved from the
            // source list to the destination list (i.e., *this), preserving their
            // relative order. After the move, the source list is empty.
            //
            // Time complexity:
            // Constant.
            list(list&& other) : sentinel_(other.sentinel_), size_(other.size_) {
                other.sentinel_.next_ = &other.sentinel_;
                other.sentinel_.prev_ = &other.sentinel_;
                other.size_ = 0;
            }

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
            list& operator=(list&& other) {
                if (this != &other) {
                    clear();
                    sentinel_ = other.sentinel_;
                    size_ = other.size_;

                    other.sentinel_.next_ = &other.sentinel_;
                    other.sentinel_.prev_ = &other.sentinel_;
                    other.size_ = 0;
                }
                return *this;
            }

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
            void swap(list& x) {
                if (this != &x) {
                    std::swap(sentinel_, x.sentinel_);
                    std::swap(size_, x.size_);
                }
            }

            // Returns the number of elements in the list.
            //
            // Time complexity:
            // Constant.
            size_type size() const {
                return size_;
            }

            // Inserts an element in the list before the element referred to
            // by the iterator pos.
            // An iterator that refers to the inserted element is returned.
            //
            // Time complexity:
            // Constant.
            iterator insert(iterator pos, value_type& value) {
                pos->prev_->next_ = &value;
                value.hook.next_ = pos;
                value.hook.prev_ = pos->prev_;
                pos->prev_ = &value;
                ++size_;

                return iterator(&value);
            }

            // Erases the element in the list at the position specified by the
            // iterator pos.
            // An iterator that refers to the element following the erased element
            // is returned if such an element exists; otherwise, end() is
            // returned.
            //
            // Time complexity:
            // Constant.
            iterator erase(iterator pos) {
                pos->prev_->next_ = pos->next_;
                pos->next_->prev_ = pos->prev_;
                --size_;

                return iterator(pos->next_);
            }

            // Inserts the element with the value x at the end of the list.
            //
            // Time complexity:
            // Constant.
            void push_back(value_type& x) {
                insert(end(), x);
            }

            // Erases the last element in the list.
            //
            // Precondition:
            // The list is not empty.
            //
            // Time complexity:
            // Constant.
            void pop_back() {
                if (size_ > 0) {
                    erase(--end());
                }
            }

            // Returns a reference to the last element in the list.
            //
            // Precondition:
            // The list is not empty.
            //
            // Time complexity:
            // Constant.
            reference back() {
                return *(--end());
            }
            const_reference back() const {
                return *(--end());
            }

            // Erases any elements from the list, yielding an empty list.
            //
            // Time complexity:
            // Either linear or constant.
            void clear() {
                while (size_ > 0) {
                    erase(begin());
                }
            }

            // Returns an iterator referring to the first element in the list
            // if the list is not empty and end() otherwise.
            //
            // Time complexity:
            // Constant.
            const_iterator begin() const {
                return const_iterator(sentinel_.next_);
            }
            iterator begin() {
                return iterator(sentinel_.next_);
            }

            // Returns an iterator referring to the fictitious one-past-the-end
            // element.
            //
            // Time complexity:
            // Constant.
            const_iterator end() const {
                return const_iterator(&sentinel_);
            }
            iterator end() {
                return iterator(&sentinel_);
            }

        private:
            // The sentinel node.
            list_hook sentinel_;

            size_type size_;
    };
}  // namespace ra::intrusive