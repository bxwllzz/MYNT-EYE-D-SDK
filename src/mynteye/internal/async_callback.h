// Copyright 2018 Slightech Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef MYNTEYE_INTERNAL_ASYNC_CALLBACK_H_
#define MYNTEYE_INTERNAL_ASYNC_CALLBACK_H_
#pragma once

#include <functional>
#include <memory>
#include <thread>
#include <utility>

#include "mynteye/stubs/global.h"

MYNTEYE_BEGIN_NAMESPACE

template <typename T>
class AsyncCallback : public std::enable_shared_from_this<AsyncCallback<T>> {
 public:
  using callback_t = std::function<void(const T& data)>;
  using pointer = std::shared_ptr<AsyncCallback<T>>;

 private:
  AsyncCallback(callback_t callback, std::size_t max_size);

 public:
  ~AsyncCallback();

  template <typename... P>
  static pointer Create(P&&... all) {
    return pointer(new AsyncCallback<T>(std::forward<P>(all)...));
  }

  callback_t operator()() {
    return std::bind(&AsyncCallback<T>::OnCallback, this->shared_from_this(),
        std::placeholders::_1);
  }

  void OnCallback(const T& data);

 private:
  callback_t callback_;
  std::size_t max_size_;
};

template <typename T>
AsyncCallback<T>::AsyncCallback(callback_t callback, std::size_t max_size)
  : callback_(callback), max_size_(max_size) {
}

template <typename T>
AsyncCallback<T>::~AsyncCallback() {
}

template <typename T>
void AsyncCallback<T>::OnCallback(const T& data) {
  if (callback_ == nullptr) return;
  callback_(data);
}

MYNTEYE_END_NAMESPACE

#endif  // MYNTEYE_INTERNAL_ASYNC_CALLBACK_H_
