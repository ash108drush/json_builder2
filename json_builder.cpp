#include "json_builder.h"
#include <memory>
#include <algorithm>

namespace json {

Builder::DictValueContext Builder::StartDict() {

    Node* node_ptr = new Node(Dict());
    nodes_stack_.push_back(std::move(node_ptr));
    dict_stack_.push_back(dict_stack_.size());
    if(first_run_){
        in_dict_ = true;
        first_run_ = false;
    }


    return BaseContext{ *this };
}

Builder::ArrayItemContext Builder::StartArray() {
    Node* node_ptr = new Node(Array());
    nodes_stack_.push_back(std::move(node_ptr));
    array_stack_.push_back(array_stack_.size());
    //in_array_ = true;
    if(first_run_){
        in_array_ = true;
        first_run_ = false;
    }
    return BaseContext{ *this };
}

Builder::BaseContext Builder::EndDict() {
    int skip = dict_stack_.front();
    dict_stack_.pop_front();
    auto dict_iter = nodes_stack_.rbegin();
    auto f_iter = nodes_stack_.rbegin();
    for(int i=0; i <= skip; ++i ){
        dict_iter = std::find_if(dict_iter,nodes_stack_.rend(),[](Node* node){
           return node->IsDict();
       });
        f_iter = dict_iter;
        ++f_iter;
    }

    Dict dict = Dict();
    bool is_value=true;
    std::string key="";
    Node * value_ptr;

    for(auto iter = nodes_stack_.rbegin(); iter != dict_iter; ++iter){
        if(is_value){
            value_ptr = *iter;
            is_value = false;
        }else {
            Node * key_node = *iter;
            if(key_node->IsString()){
                key = key_node->AsString();
            }else{

            }
            dict.insert({key,*value_ptr});
            is_value = true;
        }
    }
   *dict_iter = new Node(dict);
   nodes_stack_.erase(dict_iter.base(),nodes_stack_.end());
    return *this;
}

Builder::BaseContext Builder::EndArray() {
    int skip = array_stack_.front();
    array_stack_.pop_front();
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
      //  if(first_run_){
      //      first_run_ = false;
      //  }
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

        root_ = node;
    //}
    return root_;
}



} // end namespace
