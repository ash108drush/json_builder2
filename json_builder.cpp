#include "json_builder.h"
#include <memory>
#include <algorithm>

namespace json {

Builder::DictValueContext Builder::StartDict() {

    Node* node_ptr = new Node(Dict());
    nodes_stack_.push_back(std::move(node_ptr));
    return BaseContext{ *this };
}

Builder::ArrayItemContext Builder::StartArray() {
    Node* node_ptr = new Node(Array());
    nodes_stack_.push_back(std::move(node_ptr));
    array_stack_.push_back(true);
    return BaseContext{ *this };
}

Builder::BaseContext Builder::EndDict() {
    auto dict_ptr = std::find_if(nodes_stack_.rbegin(),nodes_stack_.rend(),[](Node* node){
        return node->IsDict();
    });
    Dict dict = Dict();
    bool is_value=true;
    std::string key="";
    Node * value_ptr;
    for(auto iter = nodes_stack_.rbegin(); iter != dict_ptr; ++iter){
        if(is_value){
            value_ptr = *iter;
            is_value = false;
        }else {
            Node * key_node = *iter;
            if(key_node->IsString()){
                key = key_node->AsString();
            }
            dict.insert({key,*value_ptr});
            is_value = true;
        }
    }

    *dict_ptr = new Node(dict);
    nodes_stack_.erase(dict_ptr.base(),nodes_stack_.end());
    return *this;
}

Builder::BaseContext Builder::EndArray() {
    array_stack_.pop_back();

    int skip = array_stack_.size();
    auto array_iter = nodes_stack_.rbegin();
    std::cout << "skip" << skip << std::endl;
    for(int i=0; i <= skip; ++i){
     array_iter = std::find_if(array_iter,nodes_stack_.rend(),[](Node* node){
        return node->IsArray();
    });
    }
    Array array = Array();
    Node * value_ptr;
    for(auto iter = nodes_stack_.rbegin(); iter != array_iter; ++iter){
        Node * node = *iter;
        array.push_back(*node);
    }
     std::reverse(array.begin(), array.end());
    *array_iter = new Node(array);

     nodes_stack_.erase(array_iter.base(),nodes_stack_.end());

    return *this;
}

Builder::ValueContext Builder::Value(Node node){

        Node* node_ptr = new Node(node);
        nodes_stack_.push_back(std::move(node_ptr));

    return BaseContext{ *this };
}

Builder::KeyContext Builder::Key(std::string string){

    Node* node_ptr = new Node(string);
    nodes_stack_.push_back(std::move(node_ptr));

    return BaseContext{ *this };
}

Node Builder::Build(){
   // if(nodes_stack_.size() == 1){
    std::cout << "stack size: " << nodes_stack_.size()  << std::endl;
        Node node = *nodes_stack_.back();

        root_ = node;
    //}
    return root_;
}



} // end namespace
