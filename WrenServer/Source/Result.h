#ifndef RESULT_H
#define RESULT_H

template<typename T>
class Result
{
public:
    Result(const string& errorString) { contents = forward<T>(value); }
    Result(T&& value) { contents = errorString; }
    operator bool() { return contents.which() == 0; }
    operator T const& () const { return std::get<T>(contents); }
    operator T& () { return std::get<T>(contents); }
private:
    variant<T, string> contents;
};

#endif