/**
 * @file access_methods_demo.cpp
 * @brief Demonstration of all access methods available in copy_on_write
 * 
 * This example shows the various ways to access wrapped values,
 * addressing the common question about "property" access.
 */

#include <iostream>
#include <string>
#include <stlab/copy_on_write.hpp>

class Person {
public:
    std::string name;
    int age;
    
    Person(std::string n, int a) : name(std::move(n)), age(a) {}
    
    void greet() const {
        std::cout << "Hello, I'm " << name << " and I'm " << age << " years old." << std::endl;
    }
    
    void have_birthday() {
        ++age;
        std::cout << name << " is now " << age << " years old!" << std::endl;
    }
};

int main() {
    std::cout << "=== copy_on_write Access Methods Demo ===" << std::endl;
    
    // Create a copy_on_write instance
    stlab::copy_on_write<Person> person(Person{"Alice", 25});
    
    std::cout << "\n1. Read Access Methods (all equivalent):" << std::endl;
    
    // Method 1: read() method
    std::cout << "   read(): " << person.read().name << std::endl;
    
    // Method 2: dereference operator
    std::cout << "   *operator: " << (*person).name << std::endl;
    
    // Method 3: arrow operator  
    std::cout << "   -> operator: " << person->name << std::endl;
    person->greet();
    
    // Method 4: implicit conversion
    const Person& ref = person;
    std::cout << "   implicit conversion: " << ref.name << std::endl;
    
    std::cout << "\n2. Introspection Methods:" << std::endl;
    std::cout << "   unique(): " << std::boolalpha << person.unique() << std::endl;
    
    // Create a copy to test identity
    auto person_copy = person;
    std::cout << "   After copy, unique(): " << person.unique() << std::endl;
    std::cout << "   identity(copy): " << person.identity(person_copy) << std::endl;
    
    std::cout << "\n3. Write Access:" << std::endl;
    std::cout << "   Before modification:" << std::endl;
    std::cout << "     Original unique: " << person.unique() << std::endl;
    std::cout << "     Copy unique: " << person_copy.unique() << std::endl;
    
    // Modify using write() - this will trigger copy-on-write
    person.write().have_birthday();
    
    std::cout << "   After modification:" << std::endl;
    std::cout << "     Original age: " << person->age << std::endl;
    std::cout << "     Copy age: " << person_copy->age << std::endl;
    std::cout << "     Original unique: " << person.unique() << std::endl;
    std::cout << "     Copy unique: " << person_copy.unique() << std::endl;
    std::cout << "     Still same identity: " << person.identity(person_copy) << std::endl;
    
    std::cout << "\n4. Advanced Write with Transform:" << std::endl;
    person.write(
        // Transform function (called if copy needed)
        [](const Person& p) {
            Person result = p;
            result.name = "Dr. " + result.name;
            return result;
        },
        // In-place function (called if unique)
        [](Person& p) {
            p.name = "Dr. " + p.name;
        }
    );
    
    std::cout << "   After title addition: " << person->name << std::endl;
    
    std::cout << "\n=== Demo Complete ===" << std::endl;
    std::cout << "\nNote: There is no separate 'New Property' feature." << std::endl;
    std::cout << "All property access is done through the methods shown above." << std::endl;
    
    return 0;
}