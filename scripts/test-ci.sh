#!/bin/bash

# Local CI test script - mimics what GitHub Actions will do
set -e

echo "🚀 Running local CI test..."
echo

# Clean up previous builds
echo "🧹 Cleaning up previous builds..."
rm -rf build/
echo

# Configure
echo "⚙️  Configuring with CMake..."
cmake --preset test
echo

# Build
echo "🔨 Building..."
cmake --build build/test
echo

# Run tests
echo "🧪 Running tests..."
ctest --test-dir build/test --verbose
echo

# Run example directly
echo "📖 Running example..."
./build/test/basic_usage_example
echo

# Test documentation build (if doxygen is available)
if command -v doxygen &> /dev/null; then
    echo "📚 Testing documentation build..."
    cmake --preset test -DBUILD_DOCS=ON
    cmake --build build/test --target docs
    echo "✅ Documentation built successfully!"
    echo "   View at: file://$(pwd)/build/test/docs/html/index.html"
else
    echo "⚠️  Doxygen not found, skipping documentation test"
fi
echo

echo "✅ All local CI tests passed!"
echo "🎉 Ready to push to GitHub!" 
