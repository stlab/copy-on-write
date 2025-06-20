/**
 * @file basic_usage.cpp
 * @brief Basic usage example of stlab::copy_on_write
 *
 * This example demonstrates the key features of copy_on_write:
 * - Efficient copying through shared data
 * - Copy-on-write semantics when modifying
 * - Identity checking and uniqueness testing
 */

#include <cassert>
#include <iostream>
#include <stlab/copy_on_write.hpp>
#include <string>

int main() {
    std::cout << "=== stlab::copy_on_write Basic Usage Example ===\n\n";

    // Create a copy-on-write string
    std::cout << "1. Creating copy-on-write string...\n";
    stlab::copy_on_write<std::string> cow_str("Hello, World!");
    std::cout << "   Original: \"" << cow_str.read() << "\"\n";

    // Make a copy - this shares the same underlying data
    std::cout << "\n2. Making a shared copy...\n";
    auto shared_copy = cow_str;
    std::cout << "   Copy: \"" << shared_copy.read() << "\"\n";

    // Check if they share the same data
    std::cout << "\n3. Checking identity and uniqueness...\n";
    std::cout << "   Do they share the same data? " << std::boolalpha
              << cow_str.identity(shared_copy) << "\n";
    std::cout << "   Is cow_str unique? " << std::boolalpha << cow_str.unique() << "\n";
    std::cout << "   Is shared_copy unique? " << std::boolalpha << shared_copy.unique() << "\n";

    // Verify they share the same data
    assert(cow_str.identity(shared_copy)); // true
    assert(!cow_str.unique());             // false (shared)
    assert(!shared_copy.unique());         // false (shared)

    // Modify through write() - this triggers copy-on-write
    std::cout << "\n4. Modifying original (triggers copy-on-write)...\n";
    cow_str.write() += " Modified!";
    std::cout << "   Original after modification: \"" << cow_str.read() << "\"\n";
    std::cout << "   Copy remains unchanged: \"" << shared_copy.read() << "\"\n";

    // Now they have different data
    std::cout << "\n5. Checking identity after modification...\n";
    std::cout << "   Do they share the same data? " << std::boolalpha
              << cow_str.identity(shared_copy) << "\n";
    std::cout << "   Is cow_str unique? " << std::boolalpha << cow_str.unique() << "\n";
    std::cout << "   Is shared_copy unique? " << std::boolalpha << shared_copy.unique() << "\n";

    // Verify they now have different data
    assert(!cow_str.identity(shared_copy)); // false (different data)
    assert(cow_str.unique());               // true (now unique)
    assert(shared_copy.unique());           // true (now unique)

    std::cout << "\n6. Demonstrating swap functionality...\n";
    stlab::copy_on_write<std::string> another_cow("Goodbye, World!");
    std::cout << "   Before swap: cow_str=\"" << cow_str.read() << "\", another_cow=\""
              << another_cow.read() << "\"\n";

    swap(cow_str, another_cow);
    std::cout << "   After swap:  cow_str=\"" << cow_str.read() << "\", another_cow=\""
              << another_cow.read() << "\"\n";

    std::cout << "\n=== Example completed successfully! ===\n";
    return 0;
}
