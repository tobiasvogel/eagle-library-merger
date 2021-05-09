#include "librarymerger.h"
#include "ui_librarymerger.h"
#include <QMimeData>
#include <QMimeDatabase>
#include <QMouseEvent>
#include <QDebug>
#include <QColor>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QProcessEnvironment>

LibraryMerger::LibraryMerger( QWidget *parent )
   : QDialog( parent )
   , ui( new Ui::LibraryMerger ) {
   ui->setupUi( this );
   ui->inputVSpacer->changeSize( 1, ( ui->cancelPushButton->height() + ui->outputVSpacer->geometry().height() + ui->buttonLayout->spacing() * 2 ) );
   ui->fileListVSpacer->changeSize( 1, ( ui->cancelPushButton->height() + ui->outputVSpacer->geometry().height() + ui->buttonLayout->spacing() * 2 ) );
   fileDropFrameBackgroundColor = QPalette( this->palette() ).window().color();
   fileDropFrameActiveBgColor = fileDropFrameBackgroundColor.darker();
   fileDropFrameBorderColor = fileDropFrameActiveBgColor.darker();
   ui->fileDropFrame->setStyleSheet( QString( "border: 3px dashed %1;" ).arg( fileDropFrameActiveBgColor.name() ) );

   QByteArray env = qgetenv( "LIBRARY_MIMETYPE" );

   QString envStr = QString( env );

   if ( !envStr.isEmpty() ) {
      printf( "The default library Mime-type (\"%s\") was overriden by environment variable to \"%s\".\n", libraryMimeType.toStdString().c_str(),
              envStr.toStdString().c_str() );
      libraryMimeType = envStr;
   }

   connect( ui->browseInputPushButton, &QPushButton::clicked, this, &LibraryMerger::browseForInputFile );
   connect( ui->addFilePushButton, &QPushButton::clicked, this, &LibraryMerger::addFilesFromLineEdit );
   this->setAcceptDrops( true );
}

LibraryMerger::~LibraryMerger() {
   delete ui;
}

void LibraryMerger::mousePressEvent( QMouseEvent *event ) {

   QDrag *drag = new QDrag( this );

   if ( event->button() == Qt::LeftButton
         && ui->fileDropFrame->geometry().contains( event->pos() ) ) {

      QMimeData *mimeData = new QMimeData;

      drag->setMimeData( mimeData );

      if ( drag->mimeData()->hasUrls() == false ) {
         drag->cancel();

      } else {

         //Qt::DropAction dropAction = drag->exec();
         drag->exec();
      }
   }
}



void LibraryMerger::dragEnterEvent( QDragEnterEvent *event ) {
   ui->fileDropFrame->setStyleSheet( QString( "border: 3px dashed %1; background-color: %2;" ).arg( fileDropFrameBorderColor.name() ).arg(
                                        fileDropFrameActiveBgColor.name() ) );
   event->acceptProposedAction();
}

void LibraryMerger::dragMoveEvent( QDragMoveEvent *event ) {
   if ( ui->fileDropFrame->geometry().contains( event->pos() ) ) {
      ui->fileDropFrame->setStyleSheet( QString( "border: 3px dashed %1; background-color: %2;" ).arg( fileDropFrameBorderColor.name() ).arg(
                                           fileDropFrameActiveBgColor.name() ) );

   } else {
      ui->fileDropFrame->setStyleSheet( QString( "border: 3px dashed %1;" ).arg( fileDropFrameActiveBgColor.name() ) );
   }
}

void LibraryMerger::dropEvent( QDropEvent *event ) {
   ui->fileDropFrame->setStyleSheet( QString( "border: 3px dashed %1;" ).arg( fileDropFrameActiveBgColor.name() ) );

   if ( ui->fileDropFrame->geometry().contains( event->pos() ) ) {
      for ( auto end = event->mimeData()->urls().size(), i = 0; i != end; ++i ) {
         addFileToList( event->mimeData()->urls().at( i ) );
      }

   } else {
      return; // Drop event outside widget
   }
}

void LibraryMerger::addFileToList( QUrl fileUrl ) {
   QMimeDatabase mimeDatabase;

   QFileInfo fInfo( fileUrl.toLocalFile() );

   QMimeType mimeType = mimeDatabase.mimeTypeForFile( fInfo );

   if ( !fileUrl.isLocalFile() ) {
      showFileWarning( NOT_ACCESSIBLE, fileUrl.toLocalFile() );

   } else if ( !fInfo.exists() ) {
      showFileWarning( DOES_NOT_EXIST, fileUrl.toLocalFile() );

   } else if ( fInfo.isDir() ) {
      showFileWarning( IS_DIR, fileUrl.toLocalFile() );

   } else if ( !fInfo.isFile() ) {
      showFileWarning( NOT_A_FILE, fileUrl.toLocalFile() );

   } else if ( !fInfo.isReadable() ) {
      showFileWarning( NO_PERMISSION, fileUrl.toLocalFile() );

   } else if ( fInfo.isExecutable() or fInfo.isSymLink() ) {
      showFileWarning( WRONG_TYPE, fileUrl.toLocalFile() );

   } else if ( ! mimeType.name().contains( libraryMimeType ) ) {
      showFileWarning( WRONG_MIME, fileUrl.toLocalFile() );

   } else {
      ui->filesListWidget->addItem( fileUrl.toLocalFile() );
   }
}

void LibraryMerger::addFilesFromLineEdit( void ) {
   QStringList files = ui->inputFileLineEdit->text().split( ";" );

   for ( auto end = files.size(), i = 0; i != end; ++i ) {
      addFileToList( QUrl::fromLocalFile( files.at( i ) ) );
   }

   ui->inputFileLineEdit->setText( "" );
}

void LibraryMerger::showFileWarning( file_warning warning, QString fileName ) {
   QMessageBox msgBox;

   switch ( warning ) {
      case DOES_NOT_EXIST:
         if ( !_show_DOES_NOT_EXIST ) { return; }

         msgBox.setIcon( QMessageBox::Critical );
         msgBox.setText( QString( tr( "The file \"%1\" does not exist." ) ).arg( fileName ) );
         break;

      case NOT_ACCESSIBLE:
         if ( !_show_NOT_ACCESSIBLE ) { return; }

         msgBox.setIcon( QMessageBox::Critical );
         msgBox.setText( QString( tr( "Cannot access the file \"%1\"." ) ).arg( fileName ) );
         break;

      case NOT_READABLE:
         if ( !_show_NOT_READABLE ) { return; }

         msgBox.setIcon( QMessageBox::Critical );
         msgBox.setText( QString( tr( "The file \"%1\" cannot be read." ) ).arg( fileName ) );
         break;

      case WRONG_TYPE:
         if ( !_show_WRONG_TYPE ) { return; }

         msgBox.setIcon( QMessageBox::Warning );
         msgBox.setText( QString( tr( "The file \"%1\" has the wrong file type." ) ).arg( fileName ) );
         break;

      case WRONG_MIME:
         if ( !_show_WRONG_MIME ) { return; }

         msgBox.setIcon( QMessageBox::Warning );
         msgBox.setText( QString( tr( "The file \"%1\" does not seem to be of the correct type." ) ).arg( fileName ) );
         break;

      case IS_DIR:
         if ( !_show_IS_DIR ) { return; }

         msgBox.setIcon( QMessageBox::Warning );
         msgBox.setText( QString( tr( "\"%1\" is a directory. Adding files recursively from within directories is not currently supported." ) ).arg( fileName ) );
         break;

      case NOT_A_FILE:
         if ( !_show_NOT_A_FILE ) { return; }

         msgBox.setIcon( QMessageBox::Critical );
         msgBox.setText( QString( tr( "\"%1\" is not a valid file." ) ).arg( fileName ) );
         break;

      case NO_PERMISSION:
         if ( !_show_NO_PERMISSION ) { return; }

         msgBox.setIcon( QMessageBox::Warning );
         msgBox.setText( QString( tr( "Cannot access the file \"%1\". Insufficient permission." ) ).arg( fileName ) );
         break;

      default:
         if ( !_show_UNKNOWN_ERROR ) { return; }

         msgBox.setIcon( QMessageBox::Critical );
         msgBox.setText( QString( tr( "There was an unknown error accessing the file \"%1\"." ) ).arg( fileName ) );
   }

   msgBox.addButton( QMessageBox::Ok );
   msgBox.setDefaultButton( QMessageBox::Ok );
   QCheckBox dontShowCheckBox( tr( "Do not show this message again" ) );
   dontShowCheckBox.blockSignals( true );
   msgBox.addButton( &dontShowCheckBox, QMessageBox::ResetRole );
   int32_t userReply = msgBox.exec();

   if ( userReply == QMessageBox::Ok ) {
      if ( dontShowCheckBox.checkState() == Qt::Checked ) {
         switch ( warning ) {
            case DOES_NOT_EXIST :
               _show_DOES_NOT_EXIST = false;
               break;

            case  NOT_ACCESSIBLE :
               _show_NOT_ACCESSIBLE = false;
               break;

            case NOT_READABLE :
               _show_NOT_READABLE = false;
               break;

            case WRONG_TYPE:
               _show_WRONG_TYPE = false;
               break;

            case WRONG_MIME:
               _show_WRONG_MIME = false;
               break;

            case NOT_A_FILE:
               _show_NOT_A_FILE = false;
               break;

            case IS_DIR:
               _show_IS_DIR = false;
               break;

            case NO_PERMISSION:
               _show_NO_PERMISSION = false;
               break;

            default:
               _show_UNKNOWN_ERROR = false;

         }

      }
   }
}

void LibraryMerger::browseForInputFile() {
   QStringList files = QFileDialog::getOpenFileNames(
                          this,
                          tr( "Select one or more files to open" ),
                          QDir::homePath(),
                          QString( tr( "Libraries" ) ).append( " (*.lbr *.l#1 *.l#2 *.l#3 *.l#4 *.l#5 *.l#6 *.l#7 *.l#8 *.l#9 *.xml)" ) );
   QString inputFiles;

   for ( auto end = files.size(), i = 0; i != end; ++i ) {
      if ( !inputFiles.isEmpty() ) {
         inputFiles.append( ";" );
      }

      inputFiles.append( files.at( i ) );
   }

   ui->inputFileLineEdit->setText( inputFiles );
}
