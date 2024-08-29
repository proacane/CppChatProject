#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>

namespace Ui {
    class ChatPage;
}

class ChatPage : public QWidget {
    Q_OBJECT

  public:
    explicit ChatPage(QWidget* parent = nullptr);
    ~ChatPage();

  private:
    Ui::ChatPage* ui;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
};

#endif  // CHATPAGE_H
