#pragma once

#include <stdexcept>
#include <ostream>


template<class T, class DEL_STRATEGY>
class weak_ptr_wrapper;

class PTR_STRATEGY;
class ARRAY_STRATEGY;


class ref_counter {
private:
    long shared_refs;
    long weak_refs;

public:
    ref_counter() :
        shared_refs(1),
        weak_refs(0)
    {}

    void inc_shared() {
        ++shared_refs;
    }

    void dec_shared() {
        if (shared_refs == 0) {
            throw std::logic_error("shared references: already zero");
        }

        --shared_refs;
    }

    void inc_weak() {
        ++weak_refs;
    }

    void dec_weak() {
        if (weak_refs == 0) {
            throw std::logic_error("weak references: already zero");
        }

        --weak_refs;
    }

    long get_shared_refs() const {
        return shared_refs;
    }

    long get_weak_refs() const {
        return weak_refs;
    }

    bool has_refs() const {
        return shared_refs + weak_refs > 0;
    }
};


template<class T, class DEL_STRATEGY>
class shared_ptr_wrapper : DEL_STRATEGY {
private:

    T* obj_ptr;
    ref_counter* ref_cntr;

    void check_delete_ptr() {
        if (ref_cntr == nullptr) {
            return;
        }

        ref_cntr->dec_shared();

        if (ref_cntr->get_shared_refs() == 0) {
            dispose(obj_ptr);
        }

        if (!ref_cntr->has_refs()) {
            delete ref_cntr;
        }

        reset_no_delete();
    }

    void reset_no_delete() {
        obj_ptr = nullptr;
        ref_cntr = nullptr;
    }

    void reset_ref_counter(T* ptr, ref_counter* cntr) {
        obj_ptr = ptr;
        ref_cntr = cntr;
    }

    void set_ptr(T* ptr) {
        ref_cntr = new ref_counter();
        obj_ptr = ptr;
    }

    template<class T, class ... Args>
    friend shared_ptr_wrapper<T, PTR_STRATEGY> make_shared(Args && ... args);

    template<class U>
    friend shared_ptr_wrapper<U, ARRAY_STRATEGY> make_shared_array(size_t size);

    template<class U, class _DEL_STR>
    friend class weak_ptr_wrapper;

public:
    shared_ptr_wrapper() {
        set_ptr(nullptr);
    }

    shared_ptr_wrapper(nullptr_t) {
        set_ptr(nullptr);
    }

    template<class Y>
    explicit shared_ptr_wrapper(Y* ptr) {
        set_ptr(ptr);
    }

    shared_ptr_wrapper(const shared_ptr_wrapper<T, DEL_STRATEGY> & other) {
        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        ref_cntr->inc_shared();
    }

    shared_ptr_wrapper(shared_ptr_wrapper<T, DEL_STRATEGY> && other) {
        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        other.reset_no_delete();
    }

    shared_ptr_wrapper<T, DEL_STRATEGY> & operator=(const shared_ptr_wrapper<T, DEL_STRATEGY> & other) {
        reset();

        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        ref_cntr->inc_shared();

        return *this;
    }

    shared_ptr_wrapper<T, DEL_STRATEGY> & operator=(shared_ptr_wrapper<T, DEL_STRATEGY> && other) {
        reset();

        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        other.reset_no_delete();

        return *this;
    }

    shared_ptr_wrapper(const weak_ptr_wrapper<T, DEL_STRATEGY> & ptr) {
        obj_ptr = ptr.obj_ptr;
        ref_cntr = ptr.ref_cntr;

        ref_cntr->inc_shared();
    }

    template<class U, class _DEL_STR>
    shared_ptr_wrapper(const shared_ptr_wrapper<U, _DEL_STR> & other) {
        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        ref_cntr->inc_shared();
    }

    template<class U, class _DEL_STR>
    shared_ptr_wrapper(shared_ptr_wrapper<U, _DEL_STR> && other) {
        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        other.reset_no_delete();
    }

    template<class U, class _DEL_STR>
    shared_ptr_wrapper<T, DEL_STRATEGY> & operator=(const shared_ptr_wrapper<U, _DEL_STR> & other) {
        reset();

        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        ref_cntr->inc_shared();

        return *this;
    }

    template<class U, class _DEL_STR>
    shared_ptr_wrapper<T, DEL_STRATEGY> & operator=(shared_ptr_wrapper<U, _DEL_STR> && other) {
        reset();

        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        other.reset_no_delete();

        return *this;
    }

    template<class U, class _DEL_STR>
    shared_ptr_wrapper(const weak_ptr_wrapper<U, _DEL_STR> & ptr) {
        obj_ptr = ptr.obj_ptr;
        ref_cntr = ptr.ref_cntr;

        ref_cntr->inc_shared();
    }

    ~shared_ptr_wrapper() {
        check_delete_ptr();
    }

    void reset() {
        check_delete_ptr();
    }

    template<class U>
    void reset(U* ptr) {
        check_delete_ptr();
        set_ptr(ptr);
    }

    void swap(shared_ptr_wrapper<T, DEL_STRATEGY> & other) {
        std::swap(obj_ptr, other.obj_ptr);
        std::swap(ref_cntr, other.ref_cntr);
    }

    T* get() const {
        return obj_ptr;
    }

    T& operator*() const {
        return *obj_ptr;
    }

    T* operator->() const {
        return obj_ptr;
    }

    long use_count() const {
        if (ref_cntr == nullptr) {
            return 0;
        }

        return ref_cntr->get_shared_refs();
    }

    bool unique() const {
        return use_count() == 1;
    }

    explicit operator bool() const {
        return get() != nullptr;
    }


    template <class U, class _DEL_STR>
    bool operator==(const shared_ptr_wrapper<U, _DEL_STR>& other) const {
        return get() == other.get();
    }

    template <class U, class _DEL_STR>
    bool operator!=(const shared_ptr_wrapper<U, _DEL_STR>& other) const {
        return !(*this == other);
    }

    template <class U, class _DEL_STR>
    bool operator<(const shared_ptr_wrapper<U, _DEL_STR>& other) const {
        return get() < other.get();
    }

    template <class U, class _DEL_STR>
    bool operator>(const shared_ptr_wrapper<U, _DEL_STR>& other) const {
        return !(*this < other) && *this != other;
    }

    template <class U, class _DEL_STR>
    bool operator<=(const shared_ptr_wrapper<U, _DEL_STR>& other) const {
        return !(get() > other.get());
    }

    template <class U, class _DEL_STR>
    bool operator>=(const shared_ptr_wrapper<U, _DEL_STR>& other) const {
        return !(get() < other.get());
    }
};

template<class T, class DEL_STRATEGY>
bool operator==(const shared_ptr_wrapper<T, DEL_STRATEGY> & ptr, nullptr_t) {
    return ptr.get() == nullptr;
}

template<class T, class DEL_STRATEGY>
bool operator==(nullptr_t, const shared_ptr_wrapper<T, DEL_STRATEGY> & ptr) {
    return ptr.get() == nullptr;
}

template<class T, class DEL_STRATEGY>
bool operator!=(const shared_ptr_wrapper<T, DEL_STRATEGY> & ptr, nullptr_t) {
    return ptr.get() != nullptr;
}

template<class T, class DEL_STRATEGY>
bool operator!=(nullptr_t, const shared_ptr_wrapper<T, DEL_STRATEGY> & ptr) {
    return ptr.get() != nullptr;
}

template<class T, class DEL_STRATEGY>
class weak_ptr_wrapper {
private:
    T* obj_ptr;
    ref_counter* ref_cntr;

    void check_delete_ptr() {
        ref_cntr->dec_weak();

        if (!ref_cntr->has_refs()) {
            delete ref_cntr;
        }

        reset_no_delete();
    }

    void reset_no_delete() {
        obj_ptr = nullptr;
        ref_cntr = nullptr;
    }

    template<class U, class DEL_STR>
    friend class shared_ptr_wrapper;

public:
    weak_ptr_wrapper() :
        obj_ptr(nullptr),
        ref_cntr(nullptr)
    {}

    weak_ptr_wrapper(const shared_ptr_wrapper<T, DEL_STRATEGY> & ptr) :
        obj_ptr(ptr.obj_ptr),
        ref_cntr(ptr.ref_cntr) {

        ref_cntr->inc_weak();
    }

    weak_ptr_wrapper(const weak_ptr_wrapper<T, DEL_STRATEGY> & other) {
        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        ref_cntr->inc_weak();
    }

    weak_ptr_wrapper<T, DEL_STRATEGY> & operator=(const weak_ptr_wrapper<T, DEL_STRATEGY> & other) {
        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        ref_cntr->inc_weak();

        return *this;
    }

    template<class U, class DEL_STR>
    weak_ptr_wrapper(const shared_ptr_wrapper<U, DEL_STR> & ptr) :
        obj_ptr(ptr.obj_ptr),
        ref_cntr(ptr.ref_cntr) {

        ref_cntr->inc_weak();
    }

    template<class U, class DEL_STR>
    weak_ptr_wrapper<T, DEL_STRATEGY> & operator=(const weak_ptr_wrapper<U, DEL_STR> & other) {
        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        ref_cntr->inc_weak();
    }

    template<class U, class DEL_STR>
    weak_ptr_wrapper(const weak_ptr_wrapper<U, DEL_STR> & other) {
        obj_ptr = other.obj_ptr;
        ref_cntr = other.ref_cntr;

        ref_cntr->inc_weak();
    }

    ~weak_ptr_wrapper() {
        check_delete_ptr();
    }

    void reset() {
        check_delete_ptr();
    }

    void swap(weak_ptr_wrapper<T, DEL_STRATEGY> & other) {
        std::swap(obj_ptr, other.obj_ptr);
        std::swap(ref_cntr, other.ref_cntr);
    }

    long use_count() const {
        if (ref_cntr == nullptr) {
            return 0;
        }

        return ref_cntr->get_shared_refs();
    }

    bool expired() const {
        if (ref_cntr == nullptr) {
            return true;
        }

        return ref_cntr->get_shared_refs() == 0;
    }

    shared_ptr_wrapper<T, DEL_STRATEGY> lock() const {
        if (expired()) {
            return shared_ptr_wrapper<T, DEL_STRATEGY>();
        }

        return shared_ptr_wrapper<T, DEL_STRATEGY>(*this);
    }
};


class PTR_STRATEGY {
protected:
    template<class T>
    void dispose(T * ptr) {
        delete ptr;
    }
};


class ARRAY_STRATEGY {
protected:
    template<class T>
    void dispose(T * ptr) {
        delete[] ptr;
    }
};


template<class T>
using shared_ptr = shared_ptr_wrapper<T, PTR_STRATEGY>;

template<class T>
using shared_array_ptr = shared_ptr_wrapper<T, ARRAY_STRATEGY>;

template<class T>
using weak_ptr = weak_ptr_wrapper<T, PTR_STRATEGY>;

template<class T>
using weak_array_ptr = weak_ptr_wrapper<T, ARRAY_STRATEGY>;


template<class T, class ... Args>
shared_ptr_wrapper<T, PTR_STRATEGY> make_shared(Args && ... args) {
    shared_ptr_wrapper<T, PTR_STRATEGY> ptr;

    auto cntr = new ref_counter();
    auto obj = new T(args...);
    ptr.reset_ref_counter(obj, cntr);

    return ptr;
}

template<class T>
shared_ptr_wrapper<T, ARRAY_STRATEGY> make_shared_array(size_t size) {
    shared_array_ptr<T> ptr;

    auto cntr = new ref_counter();
    auto obj = new T[size];
    ptr.reset_ref_counter(obj, cntr);

    return ptr;
}

template<class T, class DEL_STR>
std::ostream & operator<<(std::ostream & stream, const shared_ptr_wrapper<T, DEL_STR> & ptr) {
    stream << ptr.get();
    return stream;
}
