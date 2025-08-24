# Frequently Asked Questions

## General Questions

### Q: Where is the "New Property" feature located?

**A:** There is no "New Property" feature in `stlab::copy_on_write`. This appears to be a common misunderstanding. 

The `stlab::copy_on_write` library is a copy-on-write wrapper that provides:

1. **Value access methods** (property-like functionality):
   - `read()` - const access to the wrapped value
   - `write()` - mutable access with copy-on-write semantics
   - `operator*()` - dereference to get const reference
   - `operator->()` - arrow operator for const access
   - Implicit conversion to `const T&`

2. **Introspection methods**:
   - `unique()` - check if this is the only reference
   - `identity(other)` - check if two instances share data

3. **Standard operations**:
   - Copy/move construction and assignment
   - Value assignment with optimization for unique instances
   - Comparison operators

### Q: How do I access properties of the wrapped object?

**A:** Use the provided access methods:

```cpp
stlab::copy_on_write<MyClass> cow(MyClass{});

// Read access (multiple ways):
auto value1 = cow.read().some_property;
auto value2 = (*cow).some_property;
auto value3 = cow->some_property;
const MyClass& ref = cow; // implicit conversion
auto value4 = ref.some_property;

// Write access (triggers copy if not unique):
cow.write().some_property = new_value;

// Or using transform functions:
cow.write(
    [](const MyClass& obj) { 
        MyClass result = obj;
        result.some_property = new_value;
        return result;
    },
    [](MyClass& obj) { 
        obj.some_property = new_value; 
    }
);
```

### Q: Can I add custom properties to copy_on_write?

**A:** No, `stlab::copy_on_write` is a wrapper template that works with any type `T` that models Regular. You cannot add properties to the wrapper itself. Instead:

1. Add properties to your wrapped class `T`
2. Access them through the wrapper using the access methods shown above
3. Use composition to build classes that contain `copy_on_write` members

### Q: Is there a property-based API planned?

**A:** The current API is designed around copy-on-write semantics rather than property-based access. The existing `read()` and `write()` methods provide clear, efficient access to the wrapped value. There are no current plans for a property-based API.

## Usage Questions

### Q: What's the difference between `read()` and `operator*()`?

**A:** Both provide const access to the wrapped value:
- `read()` returns `const T&` explicitly 
- `operator*()` also returns `const T&` but follows pointer-like semantics
- Both are equally efficient and safe to use

### Q: When should I use `write()` vs assignment?

**A:** 
- Use `write()` when you need to modify the existing object in place
- Use assignment (`cow = new_value`) when you want to replace the entire value
- Use `write()` with transform functions for complex modifications that can be optimized

### Q: How do I check if a modification will cause a copy?

**A:** Use the `unique()` method:

```cpp
if (cow.unique()) {
    // Modification will happen in-place, no copy
    cow.write().modify_something();
} else {
    // Modification will trigger a copy
    cow.write().modify_something();
}
```

## Performance Questions

### Q: Are there performance implications of different access methods?

**A:** All read access methods (`read()`, `operator*()`, `operator->()`, implicit conversion) have the same performance - they're all inline and essentially equivalent.

For write access:
- `write()` on a unique instance has minimal overhead
- `write()` on a shared instance triggers a copy
- Assignment operators are optimized for unique instances

### Q: How can I minimize copies?

**A:**
1. Use `unique()` to check before modifications when possible
2. Batch modifications using the transform-based `write()` method
3. Avoid unnecessary copies by using move semantics where appropriate
4. Consider the size of your objects - copy-on-write is most beneficial for moderately sized objects (4KB-1MB)