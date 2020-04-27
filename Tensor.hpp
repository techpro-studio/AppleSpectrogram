//
//  Tensor.h
//  audio_test
//
//  Created by Alex on 19.04.2020.
//  Copyright © 2020 Alex. All rights reserved.
//
#include <algorithm>

#ifndef TENSOR_H
#define TENSOR_H



template <typename T, unsigned int ...side>
class Tensor {

public:

    T *value;

    static constexpr unsigned int size = []{
        int result = 1;
        for (auto && a:  {side...}){
            result *= a;
        }
        return result;
    }();

     Tensor(): value(new T[size]) {}

    explicit Tensor(T init): value(new T[size]){
         std::fill(value, value + size, init);
     }

     Tensor(Tensor && other): value(other.value){
         other.value = nullptr;
     }

     Tensor(const Tensor& other): value(new T[size]){
         memcpy(value, other.value, sizeof(T) * size);
     }

     Tensor& operator = (Tensor& other) {
         memcpy(value, other.value, sizeof(T) * size);
         return *this;
     }

     Tensor& operator = (Tensor && other) {
         value = other.value;
         other.value = nullptr;
         return *this;
     }

     ~Tensor(){
         delete[] value;
     }
};




#endif //TENSOR_H
