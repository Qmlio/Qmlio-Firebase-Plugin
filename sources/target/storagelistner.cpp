/* Copyright (c) 2020 Qmlio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "storagelistner.h"
#include <QDebug>
using namespace qmlio::googleFirebase;
StorageListner::~StorageListner() {}

StorageListner::StorageListner(const StorageListner &other) {
  this->m_query = other.m_query;
}

StorageListner &StorageListner::operator=(const StorageListner &other) {
  Q_UNUSED(other)
  return *this;
}

StorageListner::StorageListner(StorageQuery *query) {
  qRegisterMetaType<StorageListner>();
  this->m_query = query;
}

void StorageListner::OnProgress(firebase::storage::Controller *control) {
  ControllerWrapper *c = new ControllerWrapper(control);
  emit progress(c);
}

void StorageListner::OnPaused(firebase::storage::Controller *control) {
  ControllerWrapper *c = new ControllerWrapper(control);
  emit paused(c);
}
