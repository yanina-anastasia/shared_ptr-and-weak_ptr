#include <string>
#include "shared_ptr.h"

#define BOOST_TEST_MODULE MyTest
#include <boost\test\unit_test.hpp>


BOOST_AUTO_TEST_CASE(test_constructor) {
    int* ptr = new int(5);
    shared_ptr<int> shared_pointer(ptr);
    BOOST_CHECK_EQUAL(shared_pointer.get(), ptr);
}

BOOST_AUTO_TEST_CASE(test_constructor_copy) {
    shared_ptr<int> ptr = make_shared<int>(5);
    shared_ptr<int> copy_ptr(ptr);
    BOOST_CHECK_EQUAL(copy_ptr, ptr);
    BOOST_CHECK_EQUAL(copy_ptr.get(), ptr.get());
}

BOOST_AUTO_TEST_CASE(test_use_count) {
    auto first_ptr = make_shared<int>(1);
    auto second_ptr = first_ptr;
    BOOST_CHECK_EQUAL(first_ptr.use_count(), 2);
    BOOST_CHECK_EQUAL(second_ptr.use_count(), 2);
}

BOOST_AUTO_TEST_CASE(test_get) {
    auto first_ptr = make_shared<int>(1);
    auto second_ptr = first_ptr;
    BOOST_CHECK_EQUAL(first_ptr.get(), second_ptr.get());
}

BOOST_AUTO_TEST_CASE(test_swap) {
    auto first_ptr = make_shared<int>(1);
    auto second_ptr = make_shared<int>(5);
    first_ptr.swap(second_ptr);
    BOOST_CHECK_EQUAL(*first_ptr, 5);
    BOOST_CHECK_EQUAL(*second_ptr, 1);
}

BOOST_AUTO_TEST_CASE(test_equal) {
    auto first_ptr = make_shared<int>(1);
    auto second_ptr = first_ptr;
    BOOST_REQUIRE(first_ptr == second_ptr);
}

BOOST_AUTO_TEST_CASE(test_not_equal) {
    auto first_ptr = make_shared<int>(1);
    auto second_ptr = make_shared<int>(1);
    BOOST_REQUIRE(first_ptr != second_ptr);
}

BOOST_AUTO_TEST_CASE(test_unique) {
    auto first_ptr = make_shared<int>(1);
    auto second_ptr = first_ptr;
    BOOST_REQUIRE(!first_ptr.unique());
    second_ptr = make_shared<int>(5);
    BOOST_REQUIRE(first_ptr.unique());
    BOOST_REQUIRE(second_ptr.unique());
}

BOOST_AUTO_TEST_CASE(test_operator_ref) {
    auto first_ptr = make_shared<int>(1);
    BOOST_CHECK_EQUAL(*first_ptr, 1);
    auto second_ptr = first_ptr;
    BOOST_CHECK_EQUAL(*second_ptr, 1);
}

BOOST_AUTO_TEST_CASE(test_operator_ref_extra) {
    auto first_ptr = make_shared<std::string>("Test");
    BOOST_CHECK_EQUAL(first_ptr->size(), 4);
    auto second_ptr = first_ptr;
    BOOST_REQUIRE(!second_ptr->empty());
}

BOOST_AUTO_TEST_CASE(test_array_use_count) {
    auto first_ptr = make_shared_array<int>(5);
    BOOST_CHECK_EQUAL(first_ptr.use_count(), 1);
    auto second_ptr = first_ptr;
    BOOST_CHECK_EQUAL(first_ptr.use_count(), 2);
    BOOST_CHECK_EQUAL(second_ptr.use_count(), 2);
}

BOOST_AUTO_TEST_CASE(test_array_equal) {
    auto first_ptr = make_shared_array<int>(5);
    auto second_ptr = make_shared_array<int>(3);
    BOOST_REQUIRE(first_ptr != second_ptr);
    auto third_ptr = first_ptr;
    BOOST_REQUIRE(first_ptr == third_ptr);
}

BOOST_AUTO_TEST_CASE(test_bool) {
    shared_ptr<int> ptr;
    BOOST_REQUIRE(!(bool)ptr);
    ptr = make_shared<int>(5);
    BOOST_REQUIRE((bool)ptr);
}

BOOST_AUTO_TEST_CASE(test_shared_reset) {
    int* ptr = new int(5);
    int* new_ptr = new int(7);
    shared_ptr<int> shared_pointer(ptr);
    BOOST_CHECK_EQUAL(shared_pointer.get(), ptr);
    shared_pointer.reset(new_ptr);
    BOOST_CHECK_EQUAL(shared_pointer.get(), new_ptr);
}

int counter;

class A {
public:
    A() {
        ++counter;
    }

    ~A() {
        --counter;
    }
};

BOOST_AUTO_TEST_CASE(counter_test) {
    counter = 0;
    {
        auto first_ptr = make_shared<A>();
        BOOST_CHECK_EQUAL(counter, 1);
        auto second_ptr = first_ptr;
        BOOST_CHECK_EQUAL(counter, 1);
    }

    BOOST_CHECK_EQUAL(counter, 0);
}

BOOST_AUTO_TEST_CASE(test_counter_array) {
    counter = 0;

    {
        auto ptr = make_shared_array<A>(10);
        BOOST_CHECK_EQUAL(counter, 10);
    }

    BOOST_CHECK_EQUAL(counter, 0);
}

BOOST_AUTO_TEST_CASE(test_array_new) {
    counter = 0;
    {
        A* ptr = new A[5];
        shared_array_ptr<A> shared_pointer(ptr);
        BOOST_CHECK_EQUAL(counter, 5);
    }
    BOOST_CHECK_EQUAL(counter, 0);
}

BOOST_AUTO_TEST_CASE(test_constructor_weak) {
    int* ptr = new int(5);
    shared_ptr<int> shared_pointer(ptr);
    weak_ptr<int> weak_pointer(shared_pointer);
    BOOST_CHECK_EQUAL(shared_pointer.use_count(), weak_pointer.use_count());
}

BOOST_AUTO_TEST_CASE(test_constructor_copy_weak) {
    int* ptr = new int(5);
    shared_ptr<int> shared_pointer(ptr);
    weak_ptr<int> weak_pointer_first(shared_pointer);
    weak_ptr<int> weak_pointer_second = weak_pointer_first;
    BOOST_CHECK_EQUAL(weak_pointer_first.lock(), weak_pointer_second.lock());
}
BOOST_AUTO_TEST_CASE(test_weak_ptr_use_count) {
    auto shared_pointer = make_shared<int>(5);
    weak_ptr<int> weak_pointer(shared_pointer);
    BOOST_CHECK_EQUAL(weak_pointer.use_count(), 1);
}

BOOST_AUTO_TEST_CASE(test_weak_ptr_expired) {
    weak_ptr<int> weak_pointer;
    {
        auto shared_pointer = make_shared<int>(5);
        weak_pointer = shared_pointer;
        BOOST_REQUIRE(!weak_pointer.expired());
    }
    BOOST_REQUIRE(weak_pointer.expired());
}

BOOST_AUTO_TEST_CASE(test_weak_ptr_lock) {
    auto shared_pointer = make_shared<int>(5);
    weak_ptr<int> weak_pointer(shared_pointer);
    BOOST_REQUIRE(!weak_pointer.lock().unique());
}

BOOST_AUTO_TEST_CASE(test_weak_ptr_swap) {
    auto shared_pointer_first = make_shared<int>(5);
    weak_ptr<int> weak_pointer_first(shared_pointer_first);
    auto shared_pointer_second = make_shared<int>(7);
    weak_ptr<int> weak_pointer_second(shared_pointer_second);
    weak_pointer_first.swap(weak_pointer_second);
    BOOST_CHECK_EQUAL(*weak_pointer_first.lock(), 7);
    BOOST_CHECK_EQUAL(*weak_pointer_second.lock(), 5);
}

BOOST_AUTO_TEST_CASE(test_weak_ptr_array) {
    auto shared_pointer = make_shared_array<int>(5);
    weak_ptr<int> weak_pointer(shared_pointer);
    BOOST_CHECK_EQUAL(weak_pointer.use_count(), 1);
    BOOST_REQUIRE(!weak_pointer.expired());
}

BOOST_AUTO_TEST_CASE(test_weak_ptr_array_extra) {
    weak_ptr<int> weak_pointer; 
    {
        auto shared_pointer = make_shared_array<int>(5);
        weak_pointer = shared_pointer;
        BOOST_REQUIRE(!weak_pointer.expired());
    }
    BOOST_REQUIRE(weak_pointer.expired());
}

BOOST_AUTO_TEST_CASE(test_exception) {
    counter = 0;
    try {
        auto ptr = make_shared<A>();
        throw std::exception();
        auto ptr1 = make_shared<A>();
    } catch (...) {
        BOOST_CHECK_EQUAL(counter, 0);
    }
}

BOOST_AUTO_TEST_CASE(test_array_exception) {
    counter = 0;
    try {
        auto first_ptr = make_shared_array<A>(1);
        throw std::exception();
        auto second_ptr = make_shared_array<A>(1);
    }
    catch (...) {
        BOOST_CHECK_EQUAL(counter, 0);
    }
}
