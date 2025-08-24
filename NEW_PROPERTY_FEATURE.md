# Where is the "New Property" feature located?

## Short Answer

**There is no "New Property" feature in `stlab::copy_on_write`.** This appears to be a common misunderstanding about the library's API.

## What You're Probably Looking For

If you're looking for ways to access properties of wrapped objects, `stlab::copy_on_write` provides several access methods:

### Read Access (const)
```cpp
stlab::copy_on_write<MyClass> cow(MyClass{});

// All of these are equivalent:
auto value1 = cow.read().property;    // Explicit read method
auto value2 = (*cow).property;        // Dereference operator
auto value3 = cow->property;          // Arrow operator (most common)
const MyClass& ref = cow;             // Implicit conversion
auto value4 = ref.property;
```

### Write Access (triggers copy-on-write)
```cpp
// Simple modification:
cow.write().property = new_value;

// Advanced modification with optimization:
cow.write(
    [](const MyClass& obj) { 
        MyClass result = obj;
        result.property = new_value;
        return result;
    },
    [](MyClass& obj) { 
        obj.property = new_value; 
    }
);
```

### Introspection
```cpp
bool is_sole_owner = cow.unique();
bool sharing_data = cow1.identity(cow2);
```

## Complete Examples

1. **[access_methods_demo.cpp](example/access_methods_demo.cpp)** - Live demonstration of all access methods
2. **[basic_usage.cpp](example/basic_usage.cpp)** - General copy-on-write usage patterns

## More Information

- **[FAQ.md](docs/FAQ.md)** - Comprehensive FAQ including this question
- **[README.md](README.md)** - Full library documentation with access method examples
- **[Online Documentation](https://stlab.github.io/copy-on-write/)** - Generated API documentation

## Why This Confusion?

The `stlab::copy_on_write` library is designed as a copy-on-write wrapper, not a property-based system. It focuses on:

1. **Efficient copying** through shared data
2. **Lazy copying** only when modification is needed  
3. **Thread-safe** reference counting
4. **Simple API** with clear read/write semantics

If you need property-based access patterns, you would implement those in your wrapped class and access them through the methods shown above.