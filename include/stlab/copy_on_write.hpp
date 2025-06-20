/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef STLAB_COPY_ON_WRITE_HPP
#define STLAB_COPY_ON_WRITE_HPP

/**************************************************************************************************/

#include <atomic>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>

/**************************************************************************************************/

namespace stlab {

/**************************************************************************************************/

/*!
    A copy-on-write wrapper for any type that models Regular.

    Copy-on-write semantics allow for an object to be lazily copied - only creating a copy when
    the value is modified and there is more than one reference to the value.

    This class is thread safe and supports types that model Moveable.
*/
template <typename T> // T models Regular
class copy_on_write {
    struct model {
        std::atomic<std::size_t> _count{1};

        model() noexcept(std::is_nothrow_constructible_v<T>) = default;

        template <class... Args>
        explicit model(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>)
            : _value(std::forward<Args>(args)...) {}

        T _value;
    };

    model* _self;

    template <class U>
    using disable_copy = std::enable_if_t<!std::is_same_v<std::decay_t<U>, copy_on_write>>*;

    template <typename U>
    using disable_copy_assign =
        std::enable_if_t<!std::is_same_v<std::decay_t<U>, copy_on_write>, copy_on_write&>;

public:
    /*!
        @deprecated Use element_type instead. The type of value stored.
    */
    /* [[deprecated]] */ using value_type = T;

    /*!
        The type of value stored.
    */
    using element_type = T;

    /*!
        Default constructs the wrapped value.
    */
    copy_on_write() noexcept(std::is_nothrow_constructible_v<T>) : _self(new model()) {}

    /*!
        Constructs a new instance by forwarding arguments to the wrapped value constructor.
    */
    template <class U>
    copy_on_write(U&& x, disable_copy<U> = nullptr) : _self(new model(std::forward<U>(x))) {}

    /*!
        Constructs a new instance by forwarding multiple arguments to the wrapped value constructor.
    */
    template <class U, class V, class... Args>
    copy_on_write(U&& x, V&& y, Args&&... args)
        : _self(new model(std::forward<U>(x), std::forward<V>(y), std::forward<Args>(args)...)) {}

    /*!
        Copy constructor that shares the underlying data with the source object.
    */
    copy_on_write(const copy_on_write& x) noexcept : _self(x._self) {
        assert(_self && "FATAL (sparent) : using a moved copy_on_write object");

        // coverity[useless_call]
        _self->_count.fetch_add(1, std::memory_order_relaxed);
    }

    /*!
        Move constructor that takes ownership of the source object's data.
    */
    copy_on_write(copy_on_write&& x) noexcept : _self{std::exchange(x._self, nullptr)} {
        assert(_self && "WARNING (sparent) : using a moved copy_on_write object");
    }

    ~copy_on_write() {
        assert(!_self || ((_self->_count > 0) && "FATAL (sparent) : double delete"));
        if (_self && (_self->_count.fetch_sub(1, std::memory_order_release) == 1)) {
            std::atomic_thread_fence(std::memory_order_acquire);
            delete _self;
        }
    }

    /*!
        Copy assignment operator that shares the underlying data with the source object.
    */
    auto operator=(const copy_on_write& x) noexcept -> copy_on_write& {
        // self-assignment is not allowed to disable cert-oop54-cpp warning (and is likely a bug)
        assert(this != &x && "self-assignment is not allowed");
        return *this = copy_on_write(x);
    }

    /*!
        Move assignment operator that takes ownership of the source object's data.
    */
    auto operator=(copy_on_write&& x) noexcept -> copy_on_write& {
        auto tmp{std::move(x)};
        swap(*this, tmp);
        return *this;
    }

    /*!
        Assigns a new value to the wrapped object, optimizing for in-place assignment when unique.
    */
    template <class U>
    auto operator=(U&& x) -> disable_copy_assign<U> {
        if (_self && unique()) {
            _self->_value = std::forward<U>(x);
            return *this;
        }

        return *this = copy_on_write(std::forward<U>(x));
    }

    /*!
        Obtains a non-const reference to the underlying value.

        This will copy the underlying value if necessary so changes to the value do not affect
        other copy_on_write objects sharing the same data.
    */
    auto write() -> element_type& {
        if (!unique()) *this = copy_on_write(read());

        return _self->_value;
    }

    /*!
        Returns a const reference to the underlying value for read-only access.
    */
    [[nodiscard]] auto read() const noexcept -> const element_type& {
        assert(_self && "FATAL (sparent) : using a moved copy_on_write object");

        return _self->_value;
    }

    /*!
        Implicit conversion to const reference of the underlying value.
    */
    operator const element_type&() const noexcept { return read(); }

    /*!
        Dereference operator that returns a const reference to the underlying value.
    */
    auto operator*() const noexcept -> const element_type& { return read(); }

    /*!
        Arrow operator that returns a const pointer to the underlying value.
    */
    auto operator->() const noexcept -> const element_type* { return &read(); }

    /*!
        Returns true if this is the only reference to the underlying object.

        This is useful to determine if calling write() will cause a copy.
    */
    [[nodiscard]] auto unique() const noexcept -> bool {
        assert(_self && "FATAL (sparent) : using a moved copy_on_write object");

        return _self->_count.load(std::memory_order_acquire) == 1;
    }

    /*!
        @deprecated Use unique() instead. Returns true if this is the only reference to the
       underlying object.
    */
    [[deprecated]] [[nodiscard]] auto unique_instance() const noexcept -> bool { return unique(); }

    /*!
        Returns true if this object and the given object share the same underlying data.
    */
    [[nodiscard]] auto identity(const copy_on_write& x) const noexcept -> bool {
        assert((_self && x._self) && "FATAL (sparent) : using a moved copy_on_write object");

        return _self == x._self;
    }

    /*!
        Efficiently swaps the contents of two copy_on_write objects.
    */
    friend inline void swap(copy_on_write& x, copy_on_write& y) noexcept {
        std::swap(x._self, y._self);
    }

    friend inline auto operator<(const copy_on_write& x, const copy_on_write& y) noexcept -> bool {
        return !x.identity(y) && (*x < *y);
    }

    friend inline auto operator<(const copy_on_write& x, const element_type& y) noexcept -> bool {
        return *x < y;
    }

    friend inline auto operator<(const element_type& x, const copy_on_write& y) noexcept -> bool {
        return x < *y;
    }

    friend inline auto operator>(const copy_on_write& x, const copy_on_write& y) noexcept -> bool {
        return y < x;
    }

    friend inline auto operator>(const copy_on_write& x, const element_type& y) noexcept -> bool {
        return y < x;
    }

    friend inline auto operator>(const element_type& x, const copy_on_write& y) noexcept -> bool {
        return y < x;
    }

    friend inline auto operator<=(const copy_on_write& x, const copy_on_write& y) noexcept -> bool {
        return !(y < x);
    }

    friend inline auto operator<=(const copy_on_write& x, const element_type& y) noexcept -> bool {
        return !(y < x);
    }

    friend inline auto operator<=(const element_type& x, const copy_on_write& y) noexcept -> bool {
        return !(y < x);
    }

    friend inline auto operator>=(const copy_on_write& x, const copy_on_write& y) noexcept -> bool {
        return !(x < y);
    }

    friend inline auto operator>=(const copy_on_write& x, const element_type& y) noexcept -> bool {
        return !(x < y);
    }

    friend inline auto operator>=(const element_type& x, const copy_on_write& y) noexcept -> bool {
        return !(x < y);
    }

    friend inline auto operator==(const copy_on_write& x, const copy_on_write& y) noexcept -> bool {
        return x.identity(y) || (*x == *y);
    }

    friend inline auto operator==(const copy_on_write& x, const element_type& y) noexcept -> bool {
        return *x == y;
    }

    friend inline auto operator==(const element_type& x, const copy_on_write& y) noexcept -> bool {
        return x == *y;
    }

    friend inline auto operator!=(const copy_on_write& x, const copy_on_write& y) noexcept -> bool {
        return !(x == y);
    }

    friend inline auto operator!=(const copy_on_write& x, const element_type& y) noexcept -> bool {
        return !(x == y);
    }

    friend inline auto operator!=(const element_type& x, const copy_on_write& y) noexcept -> bool {
        return !(x == y);
    }
};
/**************************************************************************************************/

} // namespace stlab

/**************************************************************************************************/

#endif

/**************************************************************************************************/
