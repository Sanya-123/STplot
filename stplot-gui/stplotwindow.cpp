#include "stplotwindow.h"
#include "./ui_stplotwindow.h"

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <iostream>
#include <format>
#include <elfio/elfio_dump.hpp>

extern "C" {
#include "stlink.h"
#include "usb.h"
// #include <chipid.h>
// #include <helper.h>
// #include "logging,h"
}


using namespace ads;

STPlotWindow::STPlotWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::STPlotWindow)
{
    ui->setupUi(this);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    QVBoxLayout* Layout = new QVBoxLayout(ui->dockContainer);
    Layout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_DockManager = new ads::CDockManager(ui->dockContainer);
    Layout->addWidget(m_DockManager);

    // Create example content label - this can be any application specific
    // widget
    QLabel* l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

    // Create a dock widget with the title Label 1 and set the created label
    // as the dock widget content
    ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
    DockWidget->setWidget(l);

    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed
    ui->menuView->addAction(DockWidget->toggleViewAction());


    // Add the dock widget to the top dock widget area
    m_DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);

    // Create an example editor
//    QPlainTextEdit* te = new QPlainTextEdit();
//    te->setPlaceholderText("Please enter your text here into this QPlainTextEdit...");
//    DockWidget = new ads::CDockWidget("Editor 1");
//    DockWidget->setWidget(te);
    ui->menuView->addAction(DockWidget->toggleViewAction());
    m_DockManager->addDockWidget(ads::BottomDockWidgetArea, DockWidget);

    QObject::connect(ui->pushButton, SIGNAL (clicked()), this, SLOT (open_elf()));
}

STPlotWindow::~STPlotWindow()
{
    delete ui;
}


void STPlotWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
    if (m_DockManager)
    {
        m_DockManager->deleteLater();
    }
}

void STPlotWindow::connect(){
    stlink_t* sl = NULL;
    enum connect_type connect = CONNECT_HOT_PLUG;
    int32_t freq = 100000;
    sl = stlink_open_usb(UDEBUG, connect, NULL, freq);
    // if (sl == NULL){ 
    //     return(-1); 
    // }
}

void STPlotWindow::open_elf(){
    ELFIO::elfio reader;

    if ( !reader.load("/home/kasper/firmware/slopehelper/inclinometer_v1_1/bootable/inclinometer_v1_1.elf") ) {
        std::cout << "File not found or it is not an ELF file\n";
        return;
    }

    ELFIO::Elf_Half sec_num = reader.sections.size();

    for ( int i = 0; i < sec_num; ++i ) {
        ELFIO::section* psec = reader.sections[i];
        // Check section type
        if ( psec->get_type() == ELFIO::SHT_SYMTAB ) {
            const ELFIO::symbol_section_accessor symbols( reader, psec );
            unsigned int sym_n = 0;
            for ( unsigned int j = 0; j < symbols.get_symbols_num(); ++j ) {
                std::string         name;
                ELFIO::Elf64_Addr   value;
                ELFIO::Elf_Xword    size;
                unsigned char       bind;
                unsigned char       type;
                ELFIO::Elf_Half     section_index;
                unsigned char       other;

                // Read symbol properties
                symbols.get_symbol( j, name, value, size, bind, type,
                                   section_index, other );
                if (type == ELFIO::STT_OBJECT){
                    std::cout << j
                          << " " << name
                          << " " << value
                          << " " << size
                          << std::endl;


//                    symbols.generic_get_symbol_ptr<Elf32_Sym>(j)
                    this->ui->symbolTable->insertRow(sym_n);
                    QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromStdString(name));
                    this->ui->symbolTable->setItem(sym_n, 0, nameItem);
//                    QTableWidgetItem *valueItem = new QTableWidgetItem(QString::fromStdString(format("{}", value)));
//                    this->ui->symbolTable->setItem(sym_n, 1, valueItem);
//                    QTableWidgetItem *sizeItem = new QTableWidgetItem(QString::fromStdString(format("{}", size)));
//                    this->ui->symbolTable->setItem(sym_n, 2, sizeItem);
                    sym_n++;

                }
            }
        }
    }

    ELFIO::dump::header( std::cout, reader );
    ELFIO::dump::section_headers( std::cout, reader );
    ELFIO::dump::segment_headers( std::cout, reader );
    ELFIO::dump::symbol_tables( std::cout, reader );
    ELFIO::dump::notes( std::cout, reader );
    ELFIO::dump::modinfo( std::cout, reader );
    ELFIO::dump::dynamic_tags( std::cout, reader );
    ELFIO::dump::section_datas( std::cout, reader );
    ELFIO::dump::segment_datas( std::cout, reader );
}
