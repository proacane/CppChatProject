#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H
/*****************************************************************************
 * @file         picturebubble.h
 * @brief
 * @description  图片气泡内容
 * @author       ACA
 * @date         2024/08/30
 * @history
 *****************************************************************************/
#include "bubbleframe.h"
class PictureBubble : public BubbleFrame {
  public:
    PictureBubble(const QPixmap& picture, ChatRole role, QWidget* parent = nullptr);
};

#endif  // PICTUREBUBBLE_H
