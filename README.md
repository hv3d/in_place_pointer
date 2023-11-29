# in_place_pointer
Intended to support storing dynamic classes in a std::vector or a std::array

Using as std::vector to store your data guarantees the data being stored continously.

For the dynamic classes, the advice is to use a std::vector<std::shared_ptr<T>>, which leaves no guarantee of data layout.

My idea is to define a maximum size, add a simple charbuf of that size for memory and use inplace constructors and assignment.
