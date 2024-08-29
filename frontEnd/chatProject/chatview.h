#ifndef CHATVIEW_H
#define CHATVIEW_H
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
class ChatView : public QWidget {
    Q_OBJECT
  public:
    ChatView(QWidget* parent = nullptr);
    void appendChatItem(QWidget* item);                   // 尾插
    void prependChatItem(QWidget* item);                  // 头插
    void insertChatItem(QWidget* before, QWidget* item);  // 中间插

  public:
    virtual bool eventFilter(QObject* watched, QEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;

  private slots:
    // 滑动滚动条
    void slot_VScrollBar_moved(int min, int max);

  private:
    void initStyleSheet();

  private:
    // QWidget *m_pCenterWidget;
    QVBoxLayout* m_pVl;
    // 滚动区域
    QScrollArea* m_pScrollArea;
    // 是否正在加载
    bool _is_appended;
};

#endif  // CHATVIEW_H
