#ifndef NBG_UTILS_ITERATOR_HPP
#define NBG_UTILS_ITERATOR_HPP

#include <type_traits>
#include <iterator>

namespace nbg {
    template<typename T>
    struct has_const_iterator {
        private:
            typedef char                      yes;
            typedef struct { char array[2]; } no;

            template<typename C> static yes test(typename C::const_iterator*);
            template<typename C> static no  test(...);
        public:
            static const bool value = sizeof(test<T>(0)) == sizeof(yes);
            typedef T type;
    };

    template <typename T>
    struct has_begin_end {
        private:
            template<typename C> static char (&f(typename std::enable_if<
            std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::begin)),
            typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

            template<typename C> static char (&f(...))[2];

            template<typename C> static char (&g(typename std::enable_if<
            std::is_same<decltype(static_cast<typename C::const_iterator (C::*)() const>(&C::end)),
            typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

            template<typename C> static char (&g(...))[2];
        public:
            static const bool begin_value = sizeof(f<T>(0)) == 1;
            static const bool end_value = sizeof(g<T>(0)) == 1;

            static const bool value = begin_value && end_value;
            typedef T type;
    };

    template<typename T> 
    struct is_container : std::integral_constant<bool, has_const_iterator<T>::value && has_begin_end<T>::value> 
    { };

    template<typename T, typename Enable = void>
    struct basic_iterator;

    template<typename T>
    struct basic_iterator<T, typename std::enable_if_t<is_container<T>::value>> {
        using container_type    = T;

        using iterator          = typename container_type::const_iterator;
        using value_type        = typename std::iterator_traits<iterator>::value_type;
        using pointer           = value_type const*;
        using reference         = value_type const&;
        
        using difference_type   = typename container_type::difference_type;
        using iterator_category = std::input_iterator_tag;

        basic_iterator(iterator itr) : _itr(itr) {}

        reference operator*() const noexcept {
            return *_itr;
        }

        iterator operator->() const noexcept {
            return _itr;
        }

        //Prefix increment
        basic_iterator& operator++() noexcept {
            _itr++;
            return *this;
        }

        //Postfix decrement
        basic_iterator operator++(int) noexcept {
            basic_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        friend bool operator==(const basic_iterator& a, const basic_iterator& b) noexcept {
            return a._itr == b._itr;
        }

        friend bool operator!=(const basic_iterator& a, const basic_iterator& b) noexcept {
            return a._itr != b._itr;
        }

        private:
            iterator _itr;
    };
}

#endif