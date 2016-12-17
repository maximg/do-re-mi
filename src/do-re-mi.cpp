// Copyright (C) M.Golov 2016
#include <iostream>
#include <string>
 
struct InputData { int value; };
struct TransformedData { double value; };
 
InputData producer() { return InputData{42}; }
TransformedData transformer(InputData input) { return TransformedData{(double)input.value}; }
std::string finalizer(TransformedData input) { return std::to_string(input.value); }
 
int main(int argc, char **argv)
{
    InputData input = producer();
    TransformedData transformed = transformer(input);
    std::string output = finalizer(transformed);
    std::cout << "Output: " << output << "\n";
    return 0;
}
