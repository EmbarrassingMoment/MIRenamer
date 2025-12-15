## 2025-05-23 - [C++ Static Caching in Utility Functions]
**Learning:** For utility functions called in tight loops (like batch renaming), reconstructing static data structures (TArray, FString) on every call causes significant overhead. Using `static` local variables with a cache invalidation check is a highly effective optimization pattern in single-threaded contexts (like Unreal Editor Main Thread tools).
**Action:** When optimizing batch operations, look for "setup" logic inside the loop that can be cached. Ensure cache invalidation logic handles configuration changes.
