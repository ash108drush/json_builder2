#include <iostream>
#include <map>
#include <set>
#include <string>
#include <deque>
#include "json.h"

namespace json{

class Builder {
    class BaseContext;
    class DictValueContext;
    class ArrayItemContext;
    class ValueContext;
    class KeyContext;
public:
    Builder() = default;
    DictValueContext StartDict();
    ArrayItemContext StartArray();
    ValueContext Value(Node node);
    KeyContext Key(std::string string);
    BaseContext EndDict();
    BaseContext EndArray();
    Node Build();
    bool ValueIsRoot(){
        return first_run_;
    }

private:
    Node root_;
    std::vector<Node*> nodes_stack_;
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
            : builder_(builder)
        {
        }
        DictValueContext StartDict() {
          if(builder_.in_dict_ && builder_.is_value_ ) {
             throw std::logic_error("Start dict in dict after value");
            }
          //std::cout << "start d" << builder_.in_dict_  << "v"<< builder_.is_value_ << std::endl;
            builder_.in_dict_ = true;
            builder_.is_value_ = false;
            builder_.in_array_ = false;
            return builder_.StartDict();
        }
        ArrayItemContext StartArray() {
            builder_.in_dict_ = false;
            builder_.in_array_ = true;
            builder_.is_value_ = false;
            return builder_.StartArray();
        }
        ValueContext Value(Node node) {
            if(builder_.in_dict_ && builder_.is_value_){
                throw std::logic_error("Double value in dict");
            }
           // std::cout << "value called" << builder_.in_dict_ << builder_.is_value_<< std::endl;
            builder_.is_value_ =true;
            builder_.is_key_ = false;
            return builder_.Value(node);
        }
        KeyContext Key(std::string string) {

            builder_.is_key_ = true;
            builder_.is_value_ = false;
            return builder_.Key(string);
        }
        BaseContext EndDict() {
           if(builder_.dict_stack_.size() == 0){
                throw std::logic_error("Build too soon");
            }
          //  std::cout << "end dict " << builder_.dict_stack_.size() << std::endl;
            builder_.in_dict_ = false;
            return builder_.EndDict();
        }
        BaseContext EndArray() {
            if(builder_.array_stack_.size() == 0){
                throw std::logic_error("Build too soon");
            }
         //std::cout << "end array " << builder_.array_stack_.size() << std::endl;
            builder_.in_array_ = false;
            return builder_.EndArray();
        }
        Node Build() {

            if(builder_.in_array_  || builder_.in_dict_){
                throw std::logic_error("Build too soon");
            }

            if(builder_.nodes_stack_.size() > 1){
                throw std::logic_error("Build too soon");
            }

            if(builder_.nodes_stack_.size() == 0){
                throw std::logic_error("Build too soon");
            }
           // if(builder_.first_run_){
          //      throw std::logic_error("Build too soon");
           // }

            return builder_.Build();
        }
         bool ValueIsRoot(){
             return builder_.ValueIsRoot();
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
       // BaseContext Build();
        //BaseContext EndArray() = delete;
        //DictValueContext StartDict() = delete;
        ArrayItemContext StartArray() {
             throw std::logic_error("Value StartArray");
        }
        ValueContext Value(Node node){
            return BaseContext {*this}.Value(node);
        }

        Node Build() {
            if(this->ValueIsRoot()){
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
       // ArrayItemContext StartArray() = delete;
    };


};

} //end namespace

