// Copyright (C) M.Golov 2016
#include <iostream>
#include <string>
#include <functional>
#include <typeinfo>

template<typename T>
struct TraceConstr
{
    TraceConstr() { std::cout << ">>> " << typeid(T).name() << "\n"; }
    ~TraceConstr() { std::cout << "<<< " << typeid(T).name() << "\n"; }
};

// Framework

template<typename TResult, typename TInput, typename TMeta>
std::function<TResult(TInput)> getFinalizer(TMeta) {
    throw std::runtime_error("getFinalizer must be specialized");
}

template<typename TMeta, typename TContent>
class MetaData
{
    TMeta meta;
public:
    TContent content;

    MetaData(TMeta meta, TContent content) : meta(meta), content(content) {}

    template<typename TResult>
    MetaData<TMeta, TResult> transform(std::function<TResult(TContent)> fx) {
        return MetaData<TMeta, TResult>{meta, fx(content)};
    }

    template<typename TResult>
    TResult finalize() {
        const auto finalizer = getFinalizer<TResult, TContent, TMeta>(meta);
        return finalizer(content);
    }
};


// Specific application

class Backend;

using MetaInfo = struct {
    std::string data;
    Backend *backend;
};

template<typename TContent>
using Meta = MetaData<MetaInfo, TContent>;

struct InputData : public TraceConstr<InputData> {
    explicit InputData(int v) : value(v) {}
    int value;
};
struct TransformedData : public TraceConstr<TransformedData> {
    explicit TransformedData(int v) : value(v) {}
    double value;
};

class Backend {
public:
    explicit Backend(const std::string &id) : id(id) {}

    Meta<InputData> addMeta(const std::string &meta, std::function<InputData()> fx) {
        return Meta<InputData>{MetaInfo{meta, this}, fx()};
    }

    std::string finalize(MetaInfo info, TransformedData input) {
        return std::string("[") + id + ":" + info.data + "]: " + std::to_string(input.value);
    }

private:
    const std::string id;
};

template<>
std::function<std::string(TransformedData)>
getFinalizer<std::string, TransformedData, MetaInfo>(MetaInfo meta) {
    return [meta](TransformedData content){
        return meta.backend->finalize(meta, content);
    };
}

// Usage example

TransformedData transformFxC(const InputData &input) {
    return TransformedData{(double)input.value};
}

int main(int argc, char **argv)
{
    const auto transformFx = [](InputData input) {
        return TransformedData{(double)input.value};
    };

    Backend backend("id");
    Meta<InputData> input = backend.addMeta("meta", [](){return InputData{42};});
    Meta<TransformedData> transformed = input.transform<TransformedData>(transformFxC);
    std::string output = transformed.finalize<std::string>();
    std::cout << "Output: " << output << "\n";
    return 0;
}
