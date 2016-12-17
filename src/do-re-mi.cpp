// Copyright (C) M.Golov 2016
#include <iostream>
#include <string>
#include <functional>
 
struct InputData { int value; };
struct TransformedData { double value; };
 
InputData produceFx() { return InputData{42}; }
TransformedData transformFx(InputData input) { return TransformedData{(double)input.value}; }


template<typename TResult, typename TInput, typename TMeta>
std::function<TResult(TInput)> getFinalizer(TMeta) {
    throw std::runtime_error("getFinalizer must be specialized");
}

template<typename TContent>
struct Meta {
    using TMeta = std::string;
    TMeta meta;
    TContent content;

    template<typename TResult>
    Meta<TResult> transform(std::function<TResult(TContent)> fx) {
        return Meta<TResult>{meta, fx(content)};
    }

    template<typename TResult>
    TResult finalize() {
        const auto finalizer = getFinalizer<TResult, TContent, TMeta>(meta);
        return finalizer(content);
    }
};

Meta<InputData> producer() {
    return Meta<InputData>{"meta", produceFx()};
}

template<>
std::function<std::string(TransformedData)>
getFinalizer<std::string, TransformedData, Meta<TransformedData>::TMeta>(Meta<TransformedData>::TMeta meta) {
    return [meta](TransformedData content){
        return std::string("[") + meta + "]: " + std::to_string(content.value);
    };
}

int main(int argc, char **argv)
{
    Meta<InputData> input = producer();
    Meta<TransformedData> transformed = input.transform<TransformedData>(transformFx);
    std::string output = transformed.finalize<std::string>();
    std::cout << "Output: " << output << "\n";
    return 0;
}
