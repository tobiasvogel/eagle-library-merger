#ifndef LIBRARYMERGER_H
#define LIBRARYMERGER_H

#include <QDialog>
#include <QDrag>

QT_BEGIN_NAMESPACE
namespace Ui { class LibraryMerger; }
QT_END_NAMESPACE

enum file_warning_t { DOES_NOT_EXIST, NOT_ACCESSIBLE, NOT_READABLE, WRONG_TYPE, WRONG_MIME, NOT_A_FILE, IS_DIR, NO_PERMISSION };

typedef enum file_warning_t file_warning;

class LibraryMerger : public QDialog {
   Q_OBJECT

public:
   Q_ENUM( file_warning );

   LibraryMerger( QWidget *parent = nullptr );
   ~LibraryMerger();


protected:
   void mousePressEvent( QMouseEvent *event );
   void dragEnterEvent( QDragEnterEvent *event );
   void dragMoveEvent( QDragMoveEvent *event );
   void dropEvent( QDropEvent *event );

   void addFileToList( QUrl fileUrl );

   void addFilesFromLineEdit( void );

   void showFileWarning( file_warning warning, QString fileName );

   void browseForInputFile( void );

private:
   Ui::LibraryMerger *ui;
   QColor fileDropFrameBackgroundColor;
   QColor fileDropFrameBorderColor;
   QColor fileDropFrameActiveBgColor;

   bool _show_DOES_NOT_EXIST = true;
   bool _show_NOT_ACCESSIBLE = true;
   bool _show_NOT_READABLE = true;
   bool _show_WRONG_TYPE = true;
   bool _show_WRONG_MIME = true;
   bool _show_NOT_A_FILE = true;
   bool _show_IS_DIR = true;
   bool _show_NO_PERMISSION = true;
   bool _show_UNKNOWN_ERROR = true;

   QString libraryMimeType = "application/xml";
};
#endif // LIBRARYMERGER_H
