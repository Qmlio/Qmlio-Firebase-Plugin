
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
#include "messagelistener.h"
#include <QDebug>
    using namespace ::qmlio::googleFirebase;
MessageListener::MessageListener(QObject *parent) : QObject(parent) {}

void MessageListener::OnMessage(const firebase::messaging::Message &message) {

  QVariantMap data;
  if (message.notification) {

    if (message.notification) {
      if (!message.notification->title.empty()) {
        const QString key = QStringLiteral("title");
        const QString value =
            QString::fromStdString(message.notification->title.c_str());
        data.insert(key, value);
      }
      if (!message.notification->body.empty()) {
        const QString key = QStringLiteral("body");
        const QString value =
            QString::fromStdString(message.notification->body.c_str());
        data.insert(key, value);
      }
      if (!message.notification->icon.empty()) {
        const QString key = QStringLiteral("icon");
        const QString value =
            QString::fromStdString(message.notification->icon.c_str());
        data.insert(key, value);
      }
      if (!message.notification->tag.empty()) {
        const QString key = QStringLiteral("tag");
        const QString value =
            QString::fromStdString(message.notification->tag.c_str());
        data.insert(key, value);
      }
      if (!message.notification->color.empty()) {
        const QString key = QStringLiteral("color");
        const QString value =
            QString::fromStdString(message.notification->color.c_str());
        data.insert(key, value);
      }
      if (!message.notification->sound.empty()) {
        const QString key = QStringLiteral("sound");
        const QString value =
            QString::fromStdString(message.notification->sound.c_str());
        data.insert(key, value);
      }
      if (!message.notification->click_action.empty()) {
        const QString key = QStringLiteral("clickAction");
        const QString value =
            QString::fromStdString(message.notification->click_action.c_str());
        data.insert(key, value);
      }
    }

    if (message.notification_opened) {
      const QString key = QStringLiteral("opened");
      data.insert(key, true);
    }
  }
  for (const auto &field : message.data) {
    const QString key = QString::fromStdString(field.first);
    const QString value = QString::fromStdString(field.second);
    data.insert(key, value);
  }
  emit messageRecieved(data);
}

void MessageListener::OnTokenReceived(const char *token) {
  emit tokenRecieved(QString(token));
}
