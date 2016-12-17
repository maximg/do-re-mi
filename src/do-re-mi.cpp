// Copyright (C) M.Golov 2016
#include <iostream>
#include <string>
#include <functional>

// Definition

template<typename TResult, typename TInput, typename TMeta>
std::function<TResult(TInput)> getFinalizer(TMeta) {
    throw std::runtime_error("getFinalizer must be specialized");
}

using MetaInfo = std::string;

template<typename TContent>
struct Meta {
    using TMeta = MetaInfo;
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


// Usage example

struct InputData { int value; };
struct TransformedData { double value; };

class Backend {
public:
    explicit Backend(const std::string &id) : id(id) {}

    Meta<InputData> addMeta(std::function<InputData()> fx) {
        return Meta<InputData>{"meta", fx()};
    }

    std::string finalize(MetaInfo info, TransformedData input) {
        return std::string("[") + id + ":" + info + "]: " + std::to_string(input.value);
    }

private:
    const std::string id;
};

template<>
std::function<std::string(TransformedData)>
getFinalizer<std::string, TransformedData, Meta<TransformedData>::TMeta>(Meta<TransformedData>::TMeta meta) {
    return [meta](TransformedData content){
        return Backend("xxx").finalize(meta, content);
    };
}

 
TransformedData transformFx(InputData input) { return TransformedData{(double)input.value}; }

int main(int argc, char **argv)
{
    Backend backend("id");
    Meta<InputData> input = backend.addMeta([](){return InputData{42};});
    Meta<TransformedData> transformed = input.transform<TransformedData>(transformFx);
    std::string output = transformed.finalize<std::string>();
    std::cout << "Output: " << output << "\n";
    return 0;
}
