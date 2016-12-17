// Copyright (C) M.Golov 2016
#include <iostream>
#include <string>
#include <functional>
 
struct InputData { int value; };
struct TransformedData { double value; };
 
InputData produceFx() { return InputData{42}; }
TransformedData transformFx(InputData input) { return TransformedData{(double)input.value}; }
std::string finalizeFx(TransformedData input) { return std::to_string(input.value); }

template<typename TContent>
struct Meta {
    std::string meta;
    TContent content;

    template<typename TResult>
    Meta<TResult> transform(std::function<TResult(TContent)> fx) {
        return Meta<TResult>{meta, fx(content)};
    }
};

Meta<InputData> producer() {
    return Meta<InputData>{"meta", produceFx()};
}

std::string finalizer(Meta<TransformedData> input) {
    return std::string("[") + input.meta + "]: " + finalizeFx(input.content); 
}

int main(int argc, char **argv)
{
    Meta<InputData> input = producer();
    Meta<TransformedData> transformed = input.transform<TransformedData>(transformFx);
    std::string output = finalizer(transformed);
    std::cout << "Output: " << output << "\n";
    return 0;
}
