#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H
/*****************************************************************************
 * @file         textbubble.h
 * @brief
 * @description  文字气泡框内容
 * @author       ACA
 * @date         2024/08/30
 * @history
 *****************************************************************************/
#include <QTextEdit>
#include "bubbleframe.h"

class TextBubble : public BubbleFrame {
    Q_OBJECT

  public:
    TextBubble(ChatRole role, const QString& text, QWidget* parent = nullptr);

  protected:
    virtual bool eventFilter(QObject* o, QEvent* e) override;

  private:
    void adjustTextHeight();
    void setPlainText(const QString& text);
    void initStyleSheet();
    QTextEdit* _text_edit;
};

#endif  // TEXTBUBBLE_H
