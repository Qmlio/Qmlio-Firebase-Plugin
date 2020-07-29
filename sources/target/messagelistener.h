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
#ifndef MESSAGELISTNER_H
#define MESSAGELISTNER_H
#include <QObject>
#include <firebase/messaging.h>
#include <firebasedatabase.h>
namespace qmlio {
namespace googleFirebase {

class MessageListener : public QObject, public firebase::messaging::Listener {
  Q_OBJECT
public:
  explicit MessageListener(QObject *parent = nullptr);

  virtual void
  OnMessage(const ::firebase::messaging::Message &message) override;

  virtual void OnTokenReceived(const char *token) override;
signals:
  void tokenRecieved(const QString &token);
  void messageRecieved(const QVariantMap &data);
public slots:
};
} // namespace googleFirebase
} // namespace qmlio
#endif // MESSAGELISTNER_H
