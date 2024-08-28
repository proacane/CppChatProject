#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H

#include <QLineEdit>

class CustomizeEdit : public QLineEdit {
    Q_OBJECT
  public:
    CustomizeEdit(QWidget* parent = nullptr);
    ~CustomizeEdit() = default;
    void setMaxLength(int max_len);

  private slots:
    void limitTextLength(QString text);

  private:
    // 输入框最长字符长度
    int _max_len;
};

#endif  // CUSTOMIZEEDIT_H
