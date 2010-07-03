#include <QStatusBar>
#include <QDockWidget>
#include <QMenuBar>
#include <QSettings>
#include <iostream>
#include "TableMainWindow.h"

namespace Qisis {
  /**
   * Constructs a new TableMainWindow object
   * 
   * @param title 
   * @param parent 
   */
  TableMainWindow::TableMainWindow (QString title, QWidget *parent) : Qisis::MainWindow(title, parent) {
    p_parent = parent;
    connect(p_parent,SIGNAL(closeWindow()),this,SLOT(writeSettings()));
    p_title = title;
    p_table = NULL;
    p_visibleColumns = -1;
    p_currentRow = 0;
    p_currentIndex = 0;
    createTable();
    readSettings();
  }


  /** 
   * This creates the table main window.  The table and docking 
   * area are created here. It also adds the two default menus to 
   * the menu bar. Programmers can add more menus to the menu bar 
   * once an instance of this class is established. 
   */  
  void TableMainWindow::createTable() {
    #if defined(__APPLE__)
      setWindowFlags(Qt::Tool);
    #endif

    #if !defined(__APPLE__)
      setWindowFlags(Qt::Dialog);
    #endif

    statusBar()->setSizeGripEnabled(true);
    // Create the table widget 
    p_table = new QTableWidget(this);
    p_table->setAlternatingRowColors(true);
    setCentralWidget(p_table);   

   // Create the dock area 
    p_dock = new QDockWidget("Columns",this);
    p_dock->setObjectName("dock");
    p_dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    p_dock->setMinimumWidth(190);
    p_listWidget = new QListWidget(p_dock);
    p_dock->setWidget(p_listWidget);
    addDockWidget(Qt::LeftDockWidgetArea,p_dock,Qt::Vertical);
    connect(p_listWidget,SIGNAL(itemChanged(QListWidgetItem *)),
            this,SLOT(syncColumns()));
			
    // Create the file menu 
    QMenuBar *menuBar = this->menuBar();
    QMenu *fileMenu = menuBar->addMenu("&File");
    
    p_save = new QAction(this);
    p_save->setText("Save...");
    p_save->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(p_save,SIGNAL(activated()),this,SLOT(saveTable()));
    p_save->setDisabled(true);
    
    QAction *saveas = new QAction(this);
    saveas->setText("Save As...");
    connect(saveas,SIGNAL(activated()),this,SLOT(saveAsTable()));
    
    QAction *load = new QAction(this);
    load->setText("Load...");
    connect(load,SIGNAL(activated()),this,SLOT(loadTable()));
    
    QAction *del = new QAction(this);
    del->setText("Delete Selected Row(s)");
    del->setShortcut(Qt::Key_Delete);
    connect(del,SIGNAL(activated()),this,SLOT(deleteRows()));
    
    QAction *clear = new QAction(this);
    clear->setText("Clear table");
    connect(clear,SIGNAL(activated()),this,SLOT(clearTable()));
    
    QAction *close = new QAction(this);
    close->setText("Close");
    connect(close,SIGNAL(activated()),this,SLOT(hide()));
    
    fileMenu->addAction(p_save);
    fileMenu->addAction(saveas);
    fileMenu->addAction(load);
    fileMenu->addAction(del);
    fileMenu->addAction(clear);
    fileMenu->addAction(close);

    //2009-01-12
    //If we have the Mainwindow flag set to Qt::Tool so that on Macs the 
    //table window always stays on top, then we can not access the
    //menu bar to the table window, so we need to add the file options
    //for the table to the tool bar.
    #if defined(__APPLE__)
     QToolBar *toolBar = new QToolBar();
     toolBar->addAction(p_save);
     toolBar->addAction(saveas);
     toolBar->addAction(load);
     toolBar->addAction(del);
     toolBar->addAction(clear);
     toolBar->addAction(close);
     this->addToolBar(toolBar);
   #endif 

    // Create the view menu
    QMenu *viewMenu = menuBar->addMenu("&View");
    QAction *cols = new QAction(this);
    cols->setText("Columns");
    connect(cols,SIGNAL(activated()),p_dock,SLOT(show()));
    viewMenu->addAction(cols);

    this->setMenuBar(menuBar);
    installEventFilter(this);
  }
  

  /** 
   * sets the status message in the lower lefthand corner of the 
   * window. 
   * 
   * @param message
   */
  void TableMainWindow::setStatusMessage(QString message){
    this->statusBar()->showMessage(message);
  }


  /** 
   *  
   * Adds a new column to the table when a new curve is added to the plot.  Also 
   * adds the item to the dock area. 
   * 
   * @param setOn 
   * @param heading 
   * @param menuText 
   * @param insertAt 
   * @param o 
   * @param toolTip 
   */
  void TableMainWindow::addToTable (bool setOn, const QString &heading,
                                    const QString &menuText, int insertAt, 
                                    Qt::Orientation o, QString toolTip) {
   // Insert the new column
   int startCol = p_table->columnCount();

   for (int i=0; !heading.section(":",i,i).isEmpty(); i++) {
     QString htext = heading.section(":",i,i);

     //if (!htext.contains("\n")) htext = "\n" + htext;
     if(insertAt >= 0) {
       p_table->insertColumn(insertAt);
       int width = p_settings->value("Col" + QString::number(insertAt), 100).toInt();
       p_table->setColumnWidth(insertAt, width);
     } else{
       p_table->insertColumn(startCol+i);
       int width = p_settings->value("Col" + QString::number(startCol+i), 100).toInt();
       p_table->setColumnWidth(startCol+i, width);
     }
     QTableWidgetItem *header = new QTableWidgetItem(htext);
     if(insertAt >= 0) {

       if(o == Qt::Horizontal) {
         p_table->setHorizontalHeaderItem(insertAt,header);
       } else{
         p_table->setVerticalHeaderItem(insertAt,header);
       }
     }else{

       if(o == Qt::Horizontal) {
         p_table->setHorizontalHeaderItem(startCol+i,header);
       }else{
         p_table->setVerticalHeaderItem(startCol+i,header);
       }
     }

   }
   
   int endCol = p_table->columnCount() - 1;
   
   // Insert the column name into the columns dock area
   if (!menuText.isEmpty()) {
     QListWidgetItem *item = new QListWidgetItem();
     
     item->setText(menuText);
     if(toolTip.isEmpty()) {
       item->setToolTip(heading);
     }
     else {
       item->setToolTip(toolTip);
     }

     if(insertAt >=0) {
       p_listWidget->insertItem(insertAt, item);
     }else{
       p_listWidget->insertItem(endCol, item);
     }

     item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
     
     if(p_trackItems) {
       readItemSettings(menuText, item);
     } else{
       item->setCheckState(setOn?Qt::Checked:Qt::Unchecked);
     }

     p_itemList.push_back(item);
     p_startColumn.push_back(startCol);
     p_endColumn.push_back(endCol);
    }     
  }
  

 /**
  * This method hides and shows the columns according to which
  * items the user has selected to be view-able on the left hand 
  * side list. (dock area)
  */
  void TableMainWindow::syncColumns() {
    if (this->isHidden()) { return; }

    p_visibleColumns = 0;
    for (int i=0; i<p_listWidget->count(); i++) {
      QListWidgetItem *item = p_listWidget->item(i);
      int index = p_itemList.indexOf(item);

      if (index != -1) {

        for (int col = p_startColumn[index]; col <= p_endColumn[index]; col++) {

          if (item->checkState() == Qt::Checked) {
            p_table->setColumnHidden(col,false);
            p_visibleColumns++;
          }
          else {
            p_table->setColumnHidden(col,true);
          }

        }
      }
    }
  }
  

  /** 
   * Use this method to sync the table with the dock widget list
   * if the table orientation is horizontal.
   * 
   */
  void TableMainWindow::syncRows() {
    if (this->isHidden()) return;

    p_visibleColumns = 0;
    for (int i=0; i<p_listWidget->count(); i++) {
      QListWidgetItem *item = p_listWidget->item(i);
      int index = p_itemList.indexOf(item);

      if (index != -1) {

        for (int col=p_startColumn[index]; col<=p_endColumn[index]; col++) {

          if (item->checkState() == Qt::Checked) {
            p_table->setRowHidden(col,false);
            p_visibleColumns++;
          }
          else {
            p_table->setRowHidden(col,true);
          }

        }
      }
    }
  }
  

  /**
   * This method deletes a column from the table
   * 
   * @param item 
   */
  void TableMainWindow::deleteColumn(int item){
   if(p_table == NULL) { std::cout << "deleteColumn err" << std::endl; return; }
   p_itemList.removeAt(item);
   p_table->setColumnCount(p_table->columnCount() -1);
   p_itemList.clear();
   bool vis = p_table->isVisible();
   p_table = NULL;
   p_listWidget = NULL;
   this->close();

   if(vis) showTable();
 
  }
  

  /** 
   * This method checks to see if the table has been created. If
   * not it calls the createTable method before calling show.
   * 
   */
  void TableMainWindow::showTable(){
    syncColumns();
    if(p_table == NULL) createTable();
    this->show();
  }
  

  /** 
   * This method clears all items from each row and column.
   * 
   */
  void TableMainWindow::clearTable () {
    if(p_table->rowCount() == 0) return;
    for (int r=0; r<p_table->rowCount(); r++) {
      for (int c=0; c<p_table->columnCount(); c++) {
        p_table->item(r,c)->setText("");
      }
    }

    p_table->scrollToItem(p_table->item(0,0),QAbstractItemView::PositionAtTop);
    p_currentRow = 0;
    p_currentIndex = 0;
  }


  /** 
   * This method is called when the user selects a row or rows
   * uses the delete button or selects the delete row menu item
   * from the file menu.
   * 
   */
  void TableMainWindow::deleteRows () {
    QList<QTableWidgetItem*> list = p_table->selectedItems();
    QList<int> selectedRows;

    for(int i = 0; i < list.size(); i++) {
      if(selectedRows.size() == 0) {
        selectedRows.push_back(p_table->row(list[i]));
      } else if(!selectedRows.contains(p_table->row(list[i]))){
        selectedRows.push_back(p_table->row(list[i]));
      }
    }
    
    qSort(selectedRows.begin(), selectedRows.end());
    for(int d = selectedRows.size(); d > 0; d--) {
      p_table->removeRow(selectedRows[d-1]);
    }
  
    // get the number of rows that are filled in
    int filledRows = 0;
    for(int r = 0; r <  p_table->rowCount(); r++ ) {
      for(int c =0; c < p_table->columnCount(); c++) {
        if(!p_table->item(r,c) || p_table->item(r,c)->text() != "") {
          filledRows++;
          break;
        }
      }
    }
 
    p_currentRow = filledRows;    
  }


  /** 
   * This method clears the text of the given row.
   * 
   * @param row
   */
  void TableMainWindow::clearRow (int row){
    if(!this->isVisible()) return;
  
    for (int c=0; c<p_table->columnCount(); c++) {
      p_table->item(row,c)->setText("");
    }
  }

  /**
   * This method will select a file, set it as the current file and save the
   * table.
   * 
   */
  void TableMainWindow::saveAsTable() {
    QString fn = QFileDialog::getSaveFileName((QWidget*)parent(),
                                            "Choose filename to save under",
                                            ".",
                                            "Text Files (*.txt)");
    QString filename;

    //Make sure the filename is valid
    if (!fn.isEmpty()) {
      if(!fn.endsWith(".txt")) {
        filename = fn + ".txt";
      } else {
        filename = fn;
      }
    }
    //The user cancelled, or the filename is empty
    else {
      return;
    }

    p_currentFile.setFileName( filename );

    p_save->setEnabled(true);
    saveTable();
  }

 /**
  * This method allows the user to save the data from the
  * table to the current file.
  */ 
  void TableMainWindow::saveTable () {
    if(p_currentFile.fileName().isEmpty()) return;

    //if(p_currentFile.exists()) p_currentFile.remove();
    bool success = p_currentFile.open(QIODevice::WriteOnly);
    if(!success) {
      QMessageBox::critical((QWidget *)parent(),
                   "Error", "Cannot open file, please check permissions");
      p_currentFile.setFileName("");
      p_save->setDisabled(true);
      return;
    }

    QString currentText;
    QTextStream t( &p_currentFile );
    QString line = "";
    bool first = true;

    //Write each column's header to the first line in CSV format
    for (int i = 0; i < p_table->columnCount(); i++) {
      if (!p_table->isColumnHidden(i)) {
        QTableWidgetItem *header = p_table->horizontalHeaderItem(i);
        QString temp = header->text();
        temp.replace("\n"," ");
        if(first) {
          line = line + "\"" + temp + "\"";
          first = false;
        } else {
          line = line + ",\"" + temp + "\"";
        }
      }
    }
    //Add the headers to the file
    t << line << endl;

    //Add each row to the file
    for (int i=0; i < p_table->rowCount(); i++) {
      bool first = true;
      line = "";
      
      //Add each column to the line
      for (int j=0; j<p_table->columnCount(); j++) {
 
        if (!p_table->isColumnHidden(j)) {
          if(p_table->item(i,j) == 0) break; 
          currentText = p_table->item(i,j)->text();

          if (first) {
            line = line + currentText;
            first = false;
          }
          else {
            line = line + "," + currentText;
          }
        }
      }  
      //If the line is not empty, add it to the file
      if (line.split(",", QString::SkipEmptyParts).count() != 0)
        t << line << endl;
    }
    p_currentFile.close();
    this->setWindowTitle(p_title + " : " + p_currentFile.fileName());
  }


  /** 
   * This overriden method is called from the constructor so that 
   * when the Tablemainwindow is created, it know's it's size 
   * and location and the dock widget settings. 
   * 
   */
  void TableMainWindow::readSettings(){
    //Call the base class function to read the size and location
    this->MainWindow::readSettings();

    //Now read the settings that are specific to this window.
    std::string appName = p_parent->windowTitle().toStdString();

    //Now read the settings that are specific to this window.
    std::string instanceName = this->windowTitle().toStdString();

    Isis::Filename config("$HOME/.Isis/" + appName + "/" + instanceName + ".config");
    //if(p_settings != 0) delete p_settings;
    p_settings = new QSettings(QString::fromStdString(config.Expanded()), QSettings::NativeFormat);
    bool docFloats = p_settings->value("docFloat", false).toBool();
    p_dock->setFloating(docFloats);

    if(docFloats) {
      QPoint docPos = p_settings->value("docPos", QPoint(300, 100)).toPoint();
      QSize docSize = p_settings->value("docSize", QSize(300, 200)).toSize();
      p_dock->resize(docSize);
      p_dock->move(docPos);
      p_dock->setFocus();
      p_dock->setVisible(true);
    }

  }


  /** 
   * This method reads the 'checked' settings on the items listed 
   * in the dock area which determine which table columns are 
   * visible.  This method is only called if p_trackItems is set 
   * to true. 
   * 
   * @param heading
   * @param item
   */
  void TableMainWindow::readItemSettings(QString heading, QListWidgetItem *item ){
   //Now read the settings that are specific to this window.
    std::string appName = p_parent->windowTitle().toStdString();

    //Now read the settings that are specific to this window.
    std::string instanceName = this->windowTitle().toStdString();

    Isis::Filename config("$HOME/.Isis/" + appName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()), QSettings::NativeFormat);

    QString itemTitle = "item-" + item->text();
    Qt::CheckState state = (Qt::CheckState)settings.value(itemTitle, 0).toInt();
    item->setCheckState(state);
    
  }


  /** 
   * This overriden method is called when the Tablemainwindow 
   * is closed or hidden to write the size and location settings 
   * (and dock widget settings) to a config file in the user's 
   * home directory. 
   * 
   */
  void TableMainWindow::writeSettings(){
    /*Call the base class function to write the size and location*/
    this->MainWindow::writeSettings();
    //if(!this->isVisible()) return;

    /*Now read the settings that are specific to this window.*/
    std::string appName = p_parent->windowTitle().toStdString();

    /*Now read the settings that are specific to this window.*/
    std::string instanceName = this->windowTitle().toStdString();

    Isis::Filename config("$HOME/.Isis/" + appName + "/" + instanceName + ".config");
    QSettings settings(QString::fromStdString(config.Expanded()), QSettings::NativeFormat);

    settings.setValue("docFloat", p_dock->isFloating());
    settings.setValue("docPos", p_dock->pos());
    settings.setValue("docSize", p_dock->size());

    /*Save the column width for the table*/
    for (int c = 0; c < p_table->columnCount(); c++) {
      QString colWidth = "Col" + QString::number(c);
      int width = 0;

      if(p_table->columnWidth(c) == 0) {
        width = 100;
      }else {
        width = p_table->columnWidth(c);
      }
      settings.setValue(colWidth , width);

    }

    /*This writes the settings for the 'checked' property for the list items
    in the dock area which determines which columns are visible.*/

    if(p_trackItems) {
      for(int i = 0; i<p_itemList.size(); i++){
        QString itemTitle = "item-" + p_itemList[i]->text();
        settings.setValue(itemTitle, p_itemList[i]->checkState()); 
      }
    }
  }


  /** 
   * This method loads a text file into the table.
   * 
   */
  void TableMainWindow::loadTable () {
    QString fn = QFileDialog::getOpenFileName((QWidget*)parent(),
                                            "Select file to load",
                                            ".",
                                            "Text Files (*.txt)");

    //If the user cancelled or the filename is empty return
    if (fn.isEmpty()) return;

    p_currentFile.setFileName( fn );
    p_save->setEnabled(true);

    bool success = p_currentFile.open(QIODevice::ReadOnly);
    if(!success) {
      QMessageBox::critical((QWidget *)parent(),
                   "Error", "Cannot open file, please check permissions");
      p_currentFile.setFileName("");
      p_save->setDisabled(true);
      return;
    }

    clearTable();

    QList<int> column = QList<int>();

    //Set all headers to be hidden
    for (int i=0; i < p_listWidget->count() ; i++) {
      p_listWidget->item(i)->setCheckState(Qt::Unchecked);
    }

    // Strip headers off the table into the temp string
    QString temp = p_currentFile.readLine();
    temp.remove("Positive ");
    temp.remove("\"");
    temp.remove("\n");
    QStringList list = temp.split(",");

    // Loop through checking header names and setting relevant columns visible
    for (int i = 0; i < list.count(); i++) { 
      for(int j = 0; j < p_itemList.size(); j++){

        //Special cases
        if(p_itemList[j]->text() == "Ground Range" && (list[i] == "Start Latitude" || list[i] == "Start Longitude" ||
                                                       list[i] == "End Latitude" || list[i] == "End Longitude")) {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Pixel Range" && (list[i] == "Start Sample" || list[i] == "Start Line" ||
                                                       list[i] == "End Sample" || list[i] == "End Line")) {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Pixel Range" && (list[i] == "Sample" || list[i] == "Line")) {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Sample:Line" && (list[i] == "Sample" || list[i] == "Line")) {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Planetocentric Lat" && list[i] == "Planetocentric Latitude") {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Planetographic Lat" && list[i] == "Planetographic Latitude") {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Projected X:Projected Y" && (list[i] == "Projected X" || list[i] == "Projected Y")) {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Radius" && list[i] == "Local Radius") {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "XYZ" && (list[i] == "Point X" || list[i] == "Point Y" || list[i] == "Point Z")) {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Ra:Dec" && (list[i] == "Right Ascension" || list[i] == "Declination")) {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Spacecraft Position" && (list[i] == "Spacecraft X" || list[i] == "Spacecraft Y" || list[i] == "Spacecraft Z")) {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }   
        if(p_itemList[j]->text() == "Ephemeris iTime" && list[i] == "Ephemeris Time") {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Local Solar iTime" && list[i] == "Local Solar Time") {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        if(p_itemList[j]->text() == "Segments Sum" && list[i] == "Segments Sum km") {
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
        //End special cases


        if(p_itemList[j]->text() == list[i]) { 
          p_itemList[j]->setCheckState(Qt::Checked);
          break;
        }
      }  
    
      // Loop through column headers to try to find a match
      bool match = false;
      for (int cols=0; cols < p_table->columnCount(); cols++) {
        QString header = p_table->horizontalHeaderItem(cols)->text();

        header.remove("Positive");
        header.remove("\n");
        header.remove(" ");

        list[i].remove(" ");

        if(header == list[i]) {
          column.push_back(cols);
          match = true;
          break;

        }
      }
      if (!match) column.push_back(-1);
    }

    // Read data into table
    QString str = p_currentFile.readLine();
    
    while (str.count() != 0) {
      // Do we need more rows?
      if (p_currentRow+1 > p_table->rowCount()) {
        p_table->insertRow(p_currentRow);
        for (int c=0; c<p_table->columnCount(); c++) {
          QTableWidgetItem *item = new QTableWidgetItem("");
          p_table->setItem(p_currentRow,c,item);
          if (c == 0) p_table->scrollToItem(item);
        }
      }

      str.remove("\n");

      QStringList list = str.split(",");
 
      for (int i=0; i < column.size(); i++) {
        QTableWidgetItem *newItem = new QTableWidgetItem(list[i]);
        if (column[i] != -1) {
          p_table->setItem(p_currentRow, column[i], newItem);
        }
      }
      p_currentRow++;
      p_currentIndex++;


      str = p_currentFile.readLine();
    }

    p_currentFile.close();
    this->setWindowTitle(p_title + " : " + fn);
    emit fileLoaded();
  }


  /** 
   * This event filter is installed in the constructor.
   * @param o
   * @param e
   * 
   * @return bool
   */
  bool TableMainWindow::eventFilter(QObject *o,QEvent *e) {
    switch (e->type()) {
      case QEvent::Close:{
        this->writeSettings();
      }

      default: {
        return false;
      }
    }
  }


  /**
   * 
   * 
   * 
   * @param event 
   */
  void TableMainWindow::closeEvent(QCloseEvent *event) {
    this->writeSettings(); 
  }


  /**
   * 
   * 
   * 
   * @param event 
   */
  void TableMainWindow::hideEvent(QHideEvent *event) {
    this->writeSettings(); 
  }


  /** 
   * If this property is true, the class will keep track of the 
   * checked/unchecked items in the dock area which determines 
   * which columns are visible in the table. 
   * 
   * @param track
   */
  void TableMainWindow::setTrackListItems(bool track){
    p_trackItems = track;
  }


  /**
   * Returns whether or not we should track items
   * 
   * 
   * @return bool 
   */
  bool TableMainWindow::trackListItems(){
    return p_trackItems;
  }


  /**
   * Sets the current row to row
   * 
   * @param row 
   */
  void TableMainWindow::setCurrentRow(int row){
    p_currentRow = row;
  }


  /**
   * Sets the current index to currentIndex
   * 
   * @param currentIndex 
   */
  void TableMainWindow::setCurrentIndex(int currentIndex){
    p_currentIndex = currentIndex;
  }

}

