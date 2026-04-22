#include <iostream>
#include <map>
#include <set>
#include <string>
#include <deque>
#include <memory>
#include "json.h"

namespace json{

class Builder {
    class BaseContext;
    class DictValueContext;
    class ArrayItemContext;
    class ValueContext;
    class KeyContext;
    class BuildContext;
public:
    Builder() = default;
    DictValueContext StartDict();
    ArrayItemContext StartArray();
    ValueContext Value(Node node);

    KeyContext Key(std::string){
        throw std::logic_error("key after constructor");
    };
    BaseContext EndDict(){
        throw std::logic_error("enddict after constructor");
    };
    BaseContext EndArray(){
            throw std::logic_error("endarray after constructor");
    };
    Node Build(){

        throw std::logic_error("build after constructor");
    };

    bool ValueIsRoot(){
        bool tmp = first_run_;
        first_run_ = false;
        return tmp;
    }

private:
    Node root_;
    std::vector<std::unique_ptr<Node>> nodes_stack_;
    std::deque<int> array_stack_;
    std::deque<int> dict_stack_;

    bool is_key_ = false;
    bool is_value_ = false;
    bool in_dict_ = false;
    bool in_array_ = false;
    bool first_run_ = true;


    class BaseContext {
    public:
        BaseContext(Builder& builder)
            : builder_(builder){

        }
        KeyContext Key(std::string string);
        BaseContext EndDict();
        BaseContext EndArray();
        Node Build();

        DictValueContext StartDict() {
            return builder_.StartDict();
        }
        ArrayItemContext StartArray() {
             return builder_.StartArray();
        }
        ValueContext Value(Node node) {
            if(builder_.in_dict_ && builder_.is_value_){
                throw std::logic_error("Double value in dict");
            }
            // std::cout << "value called" << builder_.in_dict_ << builder_.is_value_<< std::endl;

            return builder_.Value(node);
        }

        bool ValueIsRoot(){
            return builder_.ValueIsRoot();
        }

        Builder& GetBuilder(){
            return builder_;
        }
    private:
        Builder& builder_;

    };

     class BuildContext : public BaseContext {
     public:
         BuildContext(BaseContext base)
             : BaseContext(base) {
         }
     };

    class DictValueContext : public BaseContext {
    public:
        DictValueContext(BaseContext base)
            : BaseContext(base)
        {
        }
        BaseContext EndArray() {
            throw std::logic_error("DictValueContext EndArray");
        }
        DictValueContext StartDict() {
            // return BaseContext {*this}.StartDict();
            throw std::logic_error("DictValueContext StartDict");
        }
        ArrayItemContext StartArray() {
            throw std::logic_error("DictValueContext StartArray");
        }
        ValueContext Value(Node){
            throw std::logic_error("DictValueContext Value");
        }
    };

    class ArrayItemContext : public BaseContext {
    public:
        ArrayItemContext(BaseContext base) : BaseContext(base) {}
        BaseContext EndDict() {
            throw std::logic_error("Array Item EndDict");
        }
    };

    class ValueContext : public BaseContext {
    public:
        ValueContext(BaseContext base)
            : BaseContext(base)
        {
        }

        ArrayItemContext StartArray() {
            throw std::logic_error("StartArray after value");
        }
        DictValueContext StartDict(){
            if(!BaseContext {*this}.GetBuilder().in_array_){
                throw std::logic_error("StartDict after Value");
            }
            return BaseContext {*this}.StartDict();
       }
        ValueContext Value(Node node){
            return BaseContext {*this}.Value(node);
        }

        Node Build() {
            if(ValueIsRoot()){
                return BaseContext {*this}.Build();
            }else{
                throw std::logic_error("Value Build");
            }

        }
    };

    class KeyContext : public BaseContext {
    public:
        KeyContext(BaseContext base)
            : BaseContext(base){
        }
        KeyContext Key(std::string){
            throw std::logic_error("Key Key");
        }
        BaseContext EndArray() {
            throw std::logic_error("Key End array");
        };
        //  DictValueContext StartDict() = delete;
        Node Build() {
            throw std::logic_error("Key build failed");
        }

    };


};

} //end namespace

