#include "json_builder.h"
#include <memory>
#include <algorithm>

namespace json {

Builder::DictValueContext Builder::StartDict() {

    Node* node_ptr = new Node(Dict());
    nodes_stack_.push_back(std::move(node_ptr));
    if(dict_stack_.size() == 0){
        dict_stack_.push_back(dict_stack_.size());
    }else if(in_dict_ && ! in_array_ ){
        dict_stack_.push_back(dict_stack_.size());
    }

    if(first_run_){
        first_run_ = false;
    }
   // std::cout << "start dict size" <<  dict_stack_.size() << std::endl;
    in_dict_ = true;
    return BaseContext{ *this };
}

Builder::ArrayItemContext Builder::StartArray() {
    Node* node_ptr = new Node(Array());
    nodes_stack_.push_back(std::move(node_ptr));
    array_stack_.push_back(array_stack_.size());
    if(array_stack_.size() == 0){
        array_stack_.push_back(array_stack_.size());
    }else if(in_array_ && ! in_dict_ ){
        array_stack_.push_back(array_stack_.size());
    }

    //in_array_ = true;
    if(first_run_){
        first_run_ = false;
    }
    //std::cout << "start array size" <<  array_stack_.size() << std::endl;
        in_array_ = true;
    return BaseContext{ *this };
}

Builder::BaseContext Builder::EndDict() {
    int skip = dict_stack_.front();
    //std::cout << "dict " << in_dict_ << " array " << in_array_ << std::endl;
    if(in_dict_ && !in_array_ ){
        dict_stack_.pop_front();
    }

    auto dict_iter = nodes_stack_.rbegin();
    auto f_iter = nodes_stack_.rbegin();
    for(int i=0; i <= skip; ++i ){
        dict_iter = std::find_if(f_iter,nodes_stack_.rend(),[](Node* node){
            return node->IsDict();
        });
        f_iter = dict_iter;
        ++f_iter;
    }

    Dict dict = Dict();
    bool key = true;
    Node* key_node;
    Node* value_node;

    for(auto iter = dict_iter.base(); iter != nodes_stack_.end(); ++iter){
        if(key){
            key_node = *iter;
            key = false;
        }else if(!key){
            value_node = *iter;
            if(key_node->IsString()){
                dict.insert({std::move(key_node->AsString()),std::move(*value_node)});
                key_node = nullptr;
                key = true;
            }
        }

    }
    *dict_iter = new Node(std::move(dict));
    nodes_stack_.erase(dict_iter.base(),nodes_stack_.end());
    in_dict_ = false;
    return *this;
}

Builder::BaseContext Builder::EndArray() {
    int skip = array_stack_.front();
    if(in_array_ && ! in_dict_ ){
        array_stack_.pop_front();
    }

    auto array_iter = nodes_stack_.rbegin();
    auto f_iter = nodes_stack_.rbegin();
    for(int i=0; i <= skip; ++i){
        array_iter = std::find_if(f_iter,nodes_stack_.rend(),[](Node* node){
            return node->IsArray();
        });
        f_iter = array_iter;
        ++f_iter;
    }
    Array array = Array();

    for(auto iter = array_iter.base(); iter != nodes_stack_.end(); ++iter){
        Node * node = *iter;
        array.push_back(std::move(*node));
    }

    *array_iter = new Node(std::move(array));
    nodes_stack_.erase(array_iter.base(),nodes_stack_.end());
    in_array_ = false;
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
    //std::cout << "stack size: " << nodes_stack_.size()  << std::endl;

    Node node = *nodes_stack_.back();
    if(first_run_){
        throw std::logic_error("Build first run");
    }
    if(nodes_stack_.empty()){
        throw std::logic_error("nodes_stack_ empty");
    }

    root_ = std::move(node);
    //}
    return root_;
}



} // end namespace
