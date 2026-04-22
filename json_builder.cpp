#include "json_builder.h"
#include <memory>
#include <algorithm>

namespace json {

Builder::DictValueContext Builder::StartDict() {
    std::unique_ptr<Node> node_ptr = std::make_unique<Node>(Dict());
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

    std::unique_ptr<Node> node_ptr = std::make_unique<Node>(Array());
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

Builder::BaseContext Builder::BaseContext::EndDict() {
    int skip = builder_.dict_stack_.front();
    //std::cout << "dict " << in_dict_ << " array " << in_array_ << std::endl;
    if(builder_.in_dict_ && !builder_.in_array_ ){
        builder_.dict_stack_.pop_front();
    }

    auto dict_iter = builder_.nodes_stack_.rbegin();
    auto f_iter = builder_.nodes_stack_.rbegin();
    for(int i=0; i <= skip; ++i ){
        dict_iter = std::find_if(f_iter,builder_.nodes_stack_.rend(),[&](std::unique_ptr<Node>&node){
            return node->IsDict();
        });
        f_iter = dict_iter;
        ++f_iter;
    }

    Dict dict = Dict();
    bool key = true;
    std::unique_ptr<Node> key_node = nullptr;
    std::unique_ptr<Node> value_node = nullptr;

    for(auto iter = dict_iter.base(); iter != builder_.nodes_stack_.end(); ++iter){
        if(key){
            key_node = std::move(*iter);
            key = false;
        }else if(!key){
            value_node = std::move(*iter);
            if(key_node->IsString()){
                dict.insert({std::move(key_node->AsString()),std::move(*value_node)});
                key_node = nullptr;
                key = true;
            }
        }

    }
    *dict_iter = std::move(std::make_unique<Node>(dict));//  new Node(std::move(dict));
    builder_.nodes_stack_.erase(dict_iter.base(),builder_.nodes_stack_.end());
    builder_.in_dict_ = false;
    return *this;
}

Builder::BaseContext Builder::BaseContext::EndArray() {
    int skip = builder_.array_stack_.front();
    if(builder_.in_array_ && ! builder_.in_dict_ ){
        builder_.array_stack_.pop_front();
    }

    auto array_iter = builder_.nodes_stack_.rbegin();
    auto f_iter = builder_.nodes_stack_.rbegin();
    for(int i=0; i <= skip; ++i){
        array_iter = std::find_if(f_iter,builder_.nodes_stack_.rend(),[&](std::unique_ptr<Node>& node){
            return node->IsArray();
        });
        f_iter = array_iter;
        ++f_iter;
    }
    Array array = Array();

    for(auto iter = array_iter.base(); iter != builder_.nodes_stack_.end(); ++iter){
        std::unique_ptr<Node> node = std::move(*iter);
        array.push_back(std::move(*node));
    }

    *array_iter = std::move(std::make_unique<Node>(array));
    builder_.nodes_stack_.erase(array_iter.base(),builder_.nodes_stack_.end());
    builder_.in_array_ = false;
    return *this;
}

Builder::ValueContext Builder::Value(Node node){
    std::unique_ptr<Node> node_ptr = std::make_unique<Node>(node);
    nodes_stack_.push_back(std::move(node_ptr));
    is_value_ =true;
    is_key_ = false;
    return BaseContext{ *this };
}

Builder::KeyContext Builder::BaseContext::Key(std::string string){

    std::unique_ptr<Node> node_ptr = std::make_unique<Node>(string);
    builder_.nodes_stack_.push_back(std::move(node_ptr));
    builder_.is_key_ = true;
    builder_.is_value_ = false;

    return BaseContext{ *this };
}

Node Builder::BaseContext::Build(){
    // if(nodes_stack_.size() == 1){
    //std::cout << "stack size: " << nodes_stack_.size()  << std::endl;

    if(builder_.nodes_stack_.empty()){
        throw std::logic_error("nodes_stack_ empty");
    }

    if(builder_.in_array_  || builder_.in_dict_){
        throw std::logic_error("builder in array or in dict");
    }

    if(builder_.nodes_stack_.size() > 1){
        throw std::logic_error("nodes stack size >1");
    }

    if(builder_.nodes_stack_.size() == 0){
        throw std::logic_error("nodes stack size =0");
    }


    Node node = *builder_.nodes_stack_.back();
    if(builder_.first_run_){
        throw std::logic_error("Build first run");
    }
    if(builder_.nodes_stack_.empty()){
        throw std::logic_error("nodes_stack_ empty");
    }

    builder_.root_ = std::move(node);

    return builder_.root_;
}



} // end namespace
