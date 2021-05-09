#include "multidragdropframe.h"
#include <QMouseEvent>
#include <QDebug>

MultiDragDropFrame::MultiDragDropFrame( QDialog *parent ) {
   this->setAcceptDrops( true );
}
