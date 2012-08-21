/*
 * Copyright © 2012 OSLL osll@osll.spb.ru
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * The advertising clause requiring mention in adverts must never be included.
 */
/*! ---------------------------------------------------------------
 * \file MainWindow.cpp
 * \brief MainWindow implementation
 *
 * PROJ: OSLL/epat
 * ---------------------------------------------------------------- */

#include "MainWindow.h"
#include "QFileDialog"

#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), m_binary(qApp->applicationFilePath()),
    m_slw(QSharedPointer<SymbolsListWidget>(new SymbolsListWidget()))
{
    initMenuBar();
    setCentralWidget(m_slw.data());
}

void MainWindow::initMenuBar()
{
    QAction* actOpen = new QAction(0);
    actOpen->setText("&Open");
    connect(actOpen, SIGNAL(triggered()), SLOT(slotOpenFile()));

    QMenu* menuFile = new QMenu("&File");
    menuFile->addAction(actOpen);

    menuBar()->addMenu(menuFile);
}

void MainWindow::slotOpenFile()
{
    m_binary = QFileDialog::getOpenFileName(0, "Open file", "", "");
    m_slw->setBinary(m_binary);
}
