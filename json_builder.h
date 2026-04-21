#include <iostream>
#include <map>
#include <set>
#include <string>
#include "json.h"

namespace json{



class Builder {
    class BaseContext;
    class DictValueContext;
    class ArrayItemContext;
    class ValueContext;
    class KeyContext;
public:
    Builder()=default;
    DictValueContext StartDict();
    ArrayItemContext StartArray();
    ValueContext Value(Node node);
    KeyContext Key(std::string string);
    BaseContext EndDict();
    BaseContext EndArray();
    Node Build();

private:
    Node root_;
    std::vector<Node*> nodes_stack_;
    std::vector<bool> array_stack_;
    std::vector<bool> dict_stack_;

    class BaseContext {
    public:
        BaseContext(Builder& builder)
            : builder_(builder)
        {
        }
        DictValueContext StartDict() {
            return builder_.StartDict();
        }
        ArrayItemContext StartArray() {
            return builder_.StartArray();
        }
        ValueContext Value(Node node) {
            return builder_.Value(node);
        }
        KeyContext Key(std::string string) {
            return builder_.Key(string);
        }
        BaseContext EndDict() {
            return builder_.EndDict();
        }
        BaseContext EndArray() {
            return builder_.EndArray();
        }
        Node Build() {
            return builder_.Build();
        }

    private:
        Builder& builder_;
    };

    class DictValueContext : public BaseContext {
    public:
        DictValueContext(BaseContext base)
            : BaseContext(base)
        {
        }
        BaseContext EndArray() = delete;
        DictValueContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
    };

    class ArrayItemContext : public BaseContext {
    public:
        ArrayItemContext(BaseContext base) : BaseContext(base) {}
        BaseContext EndDict() = delete;
    };

    class ValueContext : public BaseContext {
    public:
        ValueContext(BaseContext base)
            : BaseContext(base)
        {
        }
       // BaseContext Build();
        //BaseContext EndArray() = delete;
        //DictValueContext StartDict() = delete;
        ArrayItemContext StartArray() = delete;
    };

    class KeyContext : public BaseContext {
    public:
        KeyContext(BaseContext base)
            : BaseContext(base)
        {
        }
        BaseContext EndArray() = delete;
        DictValueContext StartDict() = delete;
        Node Build() = delete;
       // ArrayItemContext StartArray() = delete;
    };


};

} //end namespace

