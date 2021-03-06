/* Copyright 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "iotjs_def.h"
#include "iotjs_handlewrap.h"


namespace iotjs {


HandleWrap::HandleWrap(JObject& jobject, uv_handle_t* handle)
    : JObjectWrap(jobject)
    , __handle(handle) {
  __handle->data = this;
}


HandleWrap::~HandleWrap() {
  IOTJS_ASSERT(__handle == NULL);
}


HandleWrap* HandleWrap::FromHandle(uv_handle_t* handle) {
  HandleWrap* wrap = reinterpret_cast<HandleWrap*>(handle->data);
  IOTJS_ASSERT(wrap != NULL);
  return wrap;
}


// Close handle.
void HandleWrap::Close(OnCloseHandler on_close_cb) {
  if (__handle != NULL) {
    uv_close(__handle, on_close_cb);
    __handle = NULL;
  } else {
    DDLOG("Attempt to close uninitialized or already closed handle");
  }
}


static void OnDestroyHandleClosed(uv_handle_t* handle) {
  HandleWrap* wrap = reinterpret_cast<HandleWrap*>(handle->data);
  IOTJS_ASSERT(wrap != NULL);

  delete wrap;
}


void HandleWrap::Destroy(void) {
  Environment* env = Environment::GetEnv();
  if (__handle == NULL || env->state() == Environment::kExiting) {
    // Delete handle here immediately without waiting on close if the handle was
    // already closed or program is about to exit.
    if (__handle != NULL) {
      Close(NULL);
    }
    delete this;
  } else {
    Close(OnDestroyHandleClosed);
  }
}


} // namespace iotjs
